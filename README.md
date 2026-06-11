# Pico

Pico is a production-style runtime validation application built with **Vix.cpp**.

**Website:** https://pico.vixcpp.com
**Runtime:** Vix.cpp
**Purpose:** validate Vix.cpp modules inside one real C++ backend application

Pico is not a framework and it is not a separate runtime. It is a real backend application used to verify that Vix.cpp modules work together under a practical application shape.

It connects HTTP routing, middleware, static files, durable KV storage, SQLite events, ThreadPool jobs, WebSocket runtime behavior, runtime metrics, time utilities, filesystem utilities, Rix CSV export, Rix Auth diagnostics, and Rix PDF generation.

## Why Pico exists

Vix.cpp modules should not only compile independently. They should also work together inside a running application with shared configuration, lifecycle, routing, persistence, observability, and diagnostics.

Pico exists for that purpose.

It is intentionally small, but it is not a toy example. It is structured like a real backend application:

```txt
HTTP app
  -> middleware
  -> routes
  -> controllers
  -> application services
  -> infrastructure wrappers
  -> Vix KV
  -> SQLite
  -> ThreadPool
  -> WebSocket
  -> runtime dashboard
```

Pico is used as a production-oriented integration target for Vix.cpp.

## What Pico validates

Pico currently exercises:

- `vix::App`
- HTTP routing
- middleware
- public static files
- `.env` configuration
- SQLite persistence
- durable KV storage
- runtime metrics
- `vix/time`
- `vix/fs`
- ThreadPool jobs
- WebSocket runtime
- Vix AttachedRuntime
- paginated runtime events API
- Rix CSV export
- Rix Auth with database-backed users and sessions
- Rix PDF generation
- browser-based runtime dashboard

## Quick start

```bash
cd pico
cp .env.example .env
vix dev
```

Open:

```txt
http://127.0.0.1:8080
http://127.0.0.1:8080/status.html
```

Check the API:

```bash
curl http://127.0.0.1:8080/api
curl http://127.0.0.1:8080/health
curl http://127.0.0.1:8080/api/health
curl http://127.0.0.1:8080/api/status
```

## Status dashboard

Pico includes a browser dashboard:

```txt
/status.html
```

The dashboard observes and tests:

- application health
- uptime
- boot count
- HTTP request count
- latest heartbeat
- WebSocket message count
- SQLite event count
- ThreadPool metrics
- KV write/read behavior
- background job execution
- Rix Auth register/login/session/logout/account deletion checks
- Rix PDF download checks
- WebSocket ping/pong behavior
- paginated runtime event loading
- CSV export of runtime events

The dashboard is served from `public/` through the Vix static file handler.

## Project layout

```txt
include/pico/
  app/
    AppBootstrap.hpp

  application/
    services/
      EventService.hpp
      JobService.hpp
      RuntimeStatusService.hpp
      AuthService.hpp
      PdfService.hpp

  domain/
    models/
      Event.hpp

  infrastructure/
    database/
      DatabaseProvider.hpp
      EventRepository.hpp

    kv/
      PicoKvStore.hpp

    threadpool/
      PicoThreadPool.hpp

    websocket/
      PicoWebSocketServer.hpp

  presentation/
    controllers/
      EventController.hpp
      HealthController.hpp
      HomeController.hpp
      JobController.hpp
      KvController.hpp
      StatusController.hpp
      WebSocketController.hpp
      AuthController.hpp
      PdfController.hpp

    middleware/
      MiddlewareRegistry.hpp

    routes/
      RouteRegistry.hpp

  support/
    HttpResponses.hpp
    JsonHelpers.hpp
    HttpQuery.hpp

src/
  main.cpp

  pico/
    app/
      AppBootstrap.cpp

    application/
      services/
        AuthService.cpp
        EventService.cpp
        JobService.cpp
        PdfService.cpp
        RuntimeStatusService.cpp

    domain/
      models/
        Event.cpp

    infrastructure/
      database/
        DatabaseProvider.cpp
        EventRepository.cpp

      kv/
        PicoKvStore.cpp

      threadpool/
        PicoThreadPool.cpp

      websocket/
        PicoWebSocketServer.cpp

    presentation/
      controllers/
        AuthController.cpp
        EventController.cpp
        HealthController.cpp
        HomeController.cpp
        JobController.cpp
        KvController.cpp
        PdfController.cpp
        StatusController.cpp
        WebSocketController.cpp

      middleware/
        MiddlewareRegistry.cpp

      routes/
        RouteRegistry.cpp

    support/
      HttpResponses.cpp
      JsonHelpers.cpp

migrations/
  2026_06_08_000000_create_events.up.sql
  2026_06_08_000000_create_events.down.sql

public/
  index.html
  status.html
  app.css
  app.js

storage/

.env.example
vix.app
vix.json
README.md
```

## Architecture

Pico uses a layered backend structure.

```txt
domain
  Pure application models.

application
  Services that coordinate runtime behavior.

infrastructure
  Vix-backed technical components such as SQLite, KV, ThreadPool, filesystem,
  time, and WebSocket runtime.

presentation
  HTTP controllers, middleware, and route registration.

app
  Application bootstrap and dependency wiring.

support
  Shared JSON and HTTP helpers.
```

This keeps the runtime wiring explicit while avoiding a monolithic `main.cpp`.

## Entry point

`main.cpp` stays intentionally small:

```cpp
#include <pico/app/AppBootstrap.hpp>

int main()
{
  pico::app::AppBootstrap bootstrap;
  return bootstrap.run();
}
```

Application setup lives in:

```txt
pico::app::AppBootstrap
```

## Application bootstrap

`AppBootstrap` owns the startup sequence.

It creates and connects:

- `vix::config::Config`
- `vix::App`
- `DatabaseProvider`
- `EventRepository`
- `PicoKvStore`
- `PicoThreadPool`
- `EventService`
- `RuntimeStatusService`
- `JobService`
- `PicoWebSocketServer`
- middleware
- routes
- static files
- templates
- `AuthService`
- `PdfService`

Startup flow:

```txt
main.cpp
  -> AppBootstrap
      -> load .env
      -> create vix::App
      -> open SQLite database
      -> open durable KV store
      -> create ThreadPool
      -> initialize event schema
      -> mark boot count
      -> configure static files
      -> configure optional compression
      -> register middleware
      -> register runtime metrics
      -> register routes
      -> run HTTP
      -> optionally run HTTP + WebSocket together
      -> create AuthService with database-backed Rix Auth
      -> create PdfService for Rix PDF generation
```

## Runtime metrics

Pico stores lightweight runtime state in Vix KV.

Current runtime counters include:

```txt
app/boot_count
app/last_heartbeat
http/requests
ws/messages
jobs/run_count
jobs/slow_run_count
```

`RuntimeStatusService` exposes the current runtime snapshot through:

```txt
GET /api/status
```

Example response:

```json
{
  "ok": true,
  "app": "pico",
  "runtime": "vix.cpp",
  "status": "running",
  "uptime_seconds": 79,
  "boot_count": 21,
  "last_heartbeat": "2026-06-08T09:36:55Z",
  "http_requests": 55,
  "ws_messages": 6,
  "job_runs": 4,
  "slow_job_runs": 0,
  "events": {
    "total": 53
  },
  "threadpool": {
    "running": true
  }
}
```

The heartbeat is updated through runtime middleware and formatted with `vix/time`.

## Heartbeat model

Pico separates heartbeat state from heartbeat events.

`heartbeat()` updates the durable KV value only.

`record_heartbeat_event()` records a SQLite event when explicit event history is needed.

This avoids polluting the SQLite event table with one heartbeat event per HTTP request while still keeping the latest heartbeat visible in `/api/status`.

## KV corruption recovery

Pico uses Vix KV for diagnostic counters. Since these counters are operational state and not business data, a corrupted local KV store should not permanently prevent Pico from starting.

When the KV store fails to open because of corruption, Pico can move the corrupted store to a backup path and create a fresh KV store.

Expected behavior:

```txt
storage/kv
  -> open fails because the KV log is corrupted
  -> move to storage/kv.corrupted.<timestamp>
  -> create fresh storage/kv
  -> continue startup
```

This protects the runtime validation app from being blocked by local diagnostic-state corruption.

The SQLite database is separate:

```txt
storage/pico.db
```

KV recovery should not delete or modify the SQLite event database.

## SQLite events

Pico stores runtime events in SQLite.

The event model contains:

```txt
id
source
type
payload
created_at
```

Common event types:

```txt
system / app.boot
kv     / kv.write
kv     / kv.delete
job    / job.completed
job    / job.slow.completed
ws     / ws.open
ws     / ws.ping
ws     / ws.message
ws     / ws.close
ws     / ws.error
```

The latest events are available through:

```txt
GET /api/events
```

Example:

```bash
curl http://127.0.0.1:8080/api/events
```

## Rix CSV export

Pico uses **Rix CSV** to export SQLite runtime events as CSV.

Rix is the optional userland library layer for Vix.cpp projects. It is separate from the Vix runtime and CLI, but belongs to the same ecosystem.

Vix.cpp provides the runtime, application workflow, and core modules.

Rix provides optional libraries that applications can add when needed.

In Pico:

```txt
Vix.cpp
  -> HTTP runtime
  -> middleware
  -> KV
  -> SQLite access
  -> ThreadPool
  -> WebSocket
  -> time
  -> filesystem

Rix
  -> CSV export

Pico
  -> real C++ backend using Vix.cpp + Rix CSV
```

CSV export endpoint:

```txt
GET /api/events.csv
```

Example:

```bash
curl -i http://127.0.0.1:8080/api/events.csv
```

Or save it to a file:

```bash
curl -L http://127.0.0.1:8080/api/events.csv -o pico-events.csv
```

The endpoint returns:

```txt
Content-Type: text/csv; charset=utf-8
Content-Disposition: attachment; filename="pico-events.csv"
```

## Durable KV

Pico uses Vix KV to persist small runtime values.

Write a value:

```bash
curl -X POST http://127.0.0.1:8080/api/kv \
  -H "Content-Type: application/json" \
  -d '{"key":"demo","value":"hello"}'
```

Read a value:

```bash
curl http://127.0.0.1:8080/api/kv/demo
```

Delete a value:

```bash
curl -X DELETE http://127.0.0.1:8080/api/kv/demo
```

The status dashboard also performs a KV write/read test from the browser.

## Rix Auth diagnostics

Pico uses **Rix Auth** to validate authentication workflows through a real Vix.cpp backend.

The auth diagnostics use database-backed Rix Auth stores, so users and sessions are persisted through Pico's SQLite database instead of memory-only storage.

Pico currently exposes diagnostic auth routes for:

```txt
register
login
session validation
logout
account deletion
```

### Auth endpoints

```txt
POST   /api/auth/register
POST   /api/auth/login
POST   /api/auth/check
POST   /api/auth/logout
DELETE /api/auth/account
```

### Register a user

```bash
curl -X POST http://127.0.0.1:8080/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{"email":"pico-demo@example.com","password":"correct-password"}'
```

### Login

```bash
curl -X POST http://127.0.0.1:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"email":"pico-demo@example.com","password":"correct-password"}'
```

### Check a session

```bash
curl -X POST http://127.0.0.1:8080/api/auth/check \
  -H "Content-Type: application/json" \
  -d '{"session_id":"session_xxx"}'
```

### Logout

```bash
curl -X POST http://127.0.0.1:8080/api/auth/logout \
  -H "Content-Type: application/json" \
  -d '{"session_id":"session_xxx"}'
```

### Delete the authenticated diagnostic account

```bash
curl -X DELETE http://127.0.0.1:8080/api/auth/account \
  -H "Content-Type: application/json" \
  -d '{"session_id":"session_xxx"}'
```

The account deletion route authenticates the session first, revokes the user's sessions, then removes the user through the Rix Auth user store API.

### Security note

Pico's auth routes are diagnostic routes. They are useful for validating `rix/auth` in a real production-style app, but they are not a complete product account system.

A real application should add stronger account flows, CSRF/session handling, email verification, password reset, and authorization rules.

## Rix PDF generation

Pico uses **Rix PDF** to validate PDF generation through the Vix HTTP runtime.

### PDF endpoints

```txt
GET /api/pdf/basic
GET /api/pdf/events
```

### Generate a basic diagnostic PDF

```bash
curl -L http://127.0.0.1:8080/api/pdf/basic -o pico-basic.pdf
```

### Generate a PDF containing recent runtime events

```bash
curl -L http://127.0.0.1:8080/api/pdf/events -o pico-events.pdf
```

The PDF routes generate documents through the unified Rix facade:

```cpp
rix.pdf.document()
rix.pdf.write(...)
```

The events PDF reads recent SQLite runtime events and renders them into a PDF table.

## Background jobs

Pico uses the Vix ThreadPool through `PicoThreadPool`.

Run a diagnostic job:

```bash
curl -X POST http://127.0.0.1:8080/api/jobs \
  -H "Content-Type: application/json" \
  -d '{"name":"demo-job","value":21}'
```

Run a slow timeout-observation job:

```bash
curl -X POST http://127.0.0.1:8080/api/jobs/slow
```

ThreadPool metrics are available through:

```txt
GET /api/threadpool
```

The metrics include values such as:

```txt
workers
pending
active
idle_workers
busy_workers
submitted
completed
failed
cancelled
timed_out
rejected
```

## WebSocket

Pico can run HTTP and WebSocket together through Vix AttachedRuntime.

When WebSocket is enabled, Pico exposes a WebSocket runtime on the configured WebSocket port.

The status dashboard can test WebSocket behavior by sending a typed `ping` message and expecting a `pong` response.

Diagnostic routes:

```txt
GET /api/ws/info
GET /api/ws/metrics
```

Supported WebSocket behavior includes:

- client open events
- raw echo messages
- typed `ping` / `pong`
- room join
- room broadcast
- close events
- error events

## Routes

Routes are registered from:

```txt
src/pico/presentation/routes/RouteRegistry.cpp
```

Current routes:

```txt
GET    /api
GET    /health
GET    /api/health

GET    /api/status
GET    /api/threadpool

POST   /api/kv
POST   /api/kv/read
GET    /api/kv/:key
DELETE /api/kv/:key

GET    /api/events
POST   /api/events
GET    /api/events.csv

POST   /api/auth/register
POST   /api/auth/login
POST   /api/auth/check
POST   /api/auth/logout
DELETE /api/auth/account

POST   /api/jobs
POST   /api/jobs/slow

GET    /api/pdf/basic
GET    /api/pdf/events

GET    /api/ws/info
GET    /api/ws/metrics
```

The `/` route is served from:

```txt
public/index.html
```

The status page is served from:

```txt
public/status.html
```

## Middleware

Middleware is registered from:

```txt
src/pico/presentation/middleware/MiddlewareRegistry.cpp
```

Current middleware includes:

- CORS for local development
- rate limiting for `/api`
- request logging
- security headers
- body size limits for write routes
- custom API headers
- runtime metrics middleware

Recommended middleware order:

```txt
CORS
  -> rate limit
  -> request logging
  -> security headers
  -> body limits
  -> runtime metrics
  -> routes
```

## Static files

Static files live in:

```txt
public/
```

The app mounts `public/` at `/`.

Current public files:

```txt
public/index.html
public/status.html
public/app.css
public/app.js
```

The frontend is intentionally framework-free. It uses plain HTML, CSS, and JavaScript to inspect the runtime through the HTTP and WebSocket APIs.

## Configuration

Start by copying the example environment file:

```bash
cp .env.example .env
```

Important local values:

```dotenv
APP_NAME=pico
APP_ENV=development

SERVER_PORT=8080

DATABASE_ENGINE=sqlite
DATABASE_SQLITE_PATH=storage/pico.db

VIX_ORM_DIR=migrations

WEBSOCKET_ENABLED=true
WEBSOCKET_PORT=9090
```

Runtime values and secrets belong in:

```txt
.env
```

Documented expected variables belong in:

```txt
.env.example
```

## Vix configuration files

Pico uses two important Vix files:

```txt
vix.app
vix.json
```

`vix.app` describes the application build manifest.

`vix.json` describes Vix orchestration and project metadata.

Rule:

```txt
vix.app   -> application build manifest
vix.json  -> Vix project/orchestration config
.env      -> local runtime values and secrets
```

Vix generates an internal build project under:

```txt
.vix/generated/app/
```

Do not edit generated files manually. Edit `vix.app` and source files instead.

## Migrations

Pico includes a `migrations/` directory.

Current migration:

```txt
2026_06_08_000000_create_events.up.sql
2026_06_08_000000_create_events.down.sql
```

ORM commands:

```bash
vix orm status
vix orm migrate
vix orm rollback --steps 1
```

Pico also ensures the events table exists at startup through `EventRepository::ensure_schema()`.

## Templates

Templates can be placed in:

```txt
views/
```

The app configures the template directory through:

```cpp
app.templates(views_path);
```

## Common commands

```bash
vix dev
vix build
vix run
vix tests
```

For stronger local validation:

```bash
vix check --tests --run
```

## Manual checks

Health:

```bash
curl http://127.0.0.1:8080/health
curl http://127.0.0.1:8080/api/health
```

Runtime status:

```bash
curl http://127.0.0.1:8080/api/status
```

ThreadPool metrics:

```bash
curl http://127.0.0.1:8080/api/threadpool
```

Events:

```bash
curl http://127.0.0.1:8080/api/events
```

CSV export:

```bash
curl -L http://127.0.0.1:8080/api/events.csv -o pico-events.csv
```

### Paginated events

```bash
curl "http://127.0.0.1:8080/api/events?limit=20&offset=0"
curl "http://127.0.0.1:8080/api/events?limit=20&offset=20"
```

### Auth diagnostics

```bash
curl -X POST http://127.0.0.1:8080/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{"email":"pico-demo@example.com","password":"correct-password"}'

curl -X POST http://127.0.0.1:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"email":"pico-demo@example.com","password":"correct-password"}'
```

### PDF generation

```bash
curl -L http://127.0.0.1:8080/api/pdf/basic -o pico-basic.pdf
curl -L http://127.0.0.1:8080/api/pdf/events -o pico-events.pdf
```

KV write/read:

```bash
curl -X POST http://127.0.0.1:8080/api/kv \
  -H "Content-Type: application/json" \
  -d '{"key":"demo","value":"hello"}'

curl http://127.0.0.1:8080/api/kv/demo
```

Job execution:

```bash
curl -X POST http://127.0.0.1:8080/api/jobs \
  -H "Content-Type: application/json" \
  -d '{"name":"demo-job","value":21}'
```

WebSocket diagnostics:

```bash
curl http://127.0.0.1:8080/api/ws/info
curl http://127.0.0.1:8080/api/ws/metrics
```

## Design constraints

Pico should remain small, but real.

It should not become a large framework example or a business application. Its purpose is to keep validating Vix.cpp runtime behavior under realistic conditions.

Keep these rules:

- keep `main.cpp` minimal
- keep startup wiring in `AppBootstrap`
- register routes through `RouteRegistry`
- register middleware through `MiddlewareRegistry`
- keep HTTP logic inside controllers
- keep runtime coordination inside services
- keep database, KV, filesystem, time, ThreadPool, and WebSocket wrappers under `infrastructure`
- keep reusable JSON and HTTP helpers under `support`
- use Vix modules where they exist
- use Rix for optional userland libraries
- do not mix Rix into the Vix core runtime identity

## How to read Pico

For reviewers and contributors, the most important files are:

```txt
src/main.cpp
src/pico/app/AppBootstrap.cpp
src/pico/presentation/routes/RouteRegistry.cpp
src/pico/presentation/middleware/MiddlewareRegistry.cpp
src/pico/application/services/RuntimeStatusService.cpp
src/pico/infrastructure/kv/PicoKvStore.cpp
src/pico/infrastructure/database/EventRepository.cpp
src/pico/infrastructure/websocket/PicoWebSocketServer.cpp
public/status.html
public/app.js
```

These files show the core runtime flow from process startup to browser-visible diagnostics.

## Summary

Pico is the Vix.cpp runtime validation app.

It proves that a native C++ backend can be structured, observed, and exercised through Vix.cpp while remaining explicit and understandable.

It validates the runtime in a real application shape:

```txt
C++ source
  -> Vix.cpp runtime
  -> HTTP app
  -> middleware
  -> services
  -> KV + SQLite
  -> ThreadPool
  -> WebSocket
  -> runtime dashboard
  -> Rix CSV export
  -> Rix Auth diagnostics
  -> Rix PDF generation
```

Pico should stay focused: small enough to understand, real enough to catch integration problems.
