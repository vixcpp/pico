# Pico

Pico is a small production-style backend application built with **Vix.cpp**.

**Website:** https://pico.vixcpp.com \
**Runtime:** Vix.cpp \
**Purpose:** production-style validation app for Vix.cpp modules \

Pico is not a framework and it is not a separate runtime. It is a real application used to validate how Vix.cpp modules work together in one backend project.

It is not a framework and it is not a separate runtime. Pico is a real application used to validate how Vix.cpp modules work together in one backend project.

Pico currently exercises:

- `vix::App`
- HTTP routes
- middleware
- public static files
- `.env` configuration
- SQLite events
- durable KV storage
- ThreadPool jobs
- WebSocket runtime
- runtime status metrics
- `vix/time`
- Rix CSV export

## Purpose

Pico exists to test Vix.cpp in a realistic backend application.

Instead of testing every module only in isolation, Pico connects multiple Vix modules together:

```txt
HTTP app
  -> middleware
  -> routes
  -> services
  -> KV
  -> SQLite
  -> ThreadPool
  -> WebSocket
  -> runtime status dashboard
```

This makes Pico useful as a production-oriented validation app for Vix.cpp.

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

Pico includes a small browser dashboard:

```txt
/status.html
```

The dashboard observes:

- application health
- uptime
- boot count
- HTTP request count
- WebSocket message count
- latest heartbeat
- SQLite event count
- ThreadPool metrics
- KV write/read behavior
- background job execution
- WebSocket ping/pong behavior

The page is served from the `public/` directory through Vix static files.

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

    middleware/
      MiddlewareRegistry.hpp

    routes/
      RouteRegistry.hpp

  support/
    HttpResponses.hpp
    JsonHelpers.hpp

src/
  main.cpp

  pico/
    app/
      AppBootstrap.cpp

    application/
      services/
        EventService.cpp
        JobService.cpp
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
        EventController.cpp
        HealthController.cpp
        HomeController.cpp
        JobController.cpp
        KvController.cpp
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

Pico uses a simple layered structure:

```txt
domain
  Pure application models.

application
  Services that coordinate business/runtime behavior.

infrastructure
  Vix-backed technical components such as SQLite, KV, ThreadPool, and WebSocket.

presentation
  HTTP controllers, middleware, and route registration.

app
  Application bootstrap and dependency wiring.

support
  Shared JSON and HTTP helpers.
```

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

`AppBootstrap` is responsible for wiring the application.

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
      -> register static files
      -> register middleware
      -> register routes
      -> run HTTP
      -> optionally run WebSocket with HTTP
```

## Runtime metrics

Pico stores lightweight runtime state in durable KV.

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
  }
}
```

The heartbeat is updated through runtime middleware and formatted with `vix/time`.

## Routes

Routes are registered from:

```txt
src/pico/presentation/routes/RouteRegistry.cpp
```

Current routes:

```txt
GET  /api
GET  /health
GET  /api/health

GET  /api/status
GET  /api/threadpool

POST /api/kv
GET  /api/kv/:key
DEL  /api/kv/:key

GET  /api/events
POST /api/events
GET  /api/events.csv

POST /api/jobs
POST /api/jobs/slow

GET  /api/ws/info
GET  /api/ws/metrics
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

CSV export is available through:

```txt
GET /api/events.csv
```

The CSV export uses Rix.

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

## Common commands

```bash
vix dev
vix build
vix run
vix tests
```

For a stronger local validation:

```bash
vix check --tests --run
```

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

## Templates

Templates can be placed in:

```txt
views/
```

The app configures the template directory through:

```cpp
app.templates(views_path);
```

## Notes for development

Keep the structure simple:

- keep `main.cpp` minimal
- keep startup wiring in `AppBootstrap`
- register routes through `RouteRegistry`
- register middleware through `MiddlewareRegistry`
- keep HTTP logic inside controllers
- keep runtime logic inside services
- keep database, KV, ThreadPool, and WebSocket wrappers under `infrastructure`
- keep reusable JSON and HTTP helpers under `support`

Pico should remain small, but real enough to validate Vix.cpp under realistic backend conditions.
