# Vix backend application manifest
# This file describes one executable backend target.
# Vix converts it to an internal CMake project under .vix/generated/app/.

name = "pico"
type = "executable"
standard = "c++20"
output_dir = "bin"

sources = [
  "src/main.cpp",

  "src/pico/app/AppBootstrap.cpp",

  "src/pico/support/HttpResponses.cpp",
  "src/pico/support/HttpQuery.cpp",
  "src/pico/support/JsonHelpers.cpp",

  "src/pico/domain/models/Event.cpp",

  "src/pico/infrastructure/database/DatabaseProvider.cpp",
  "src/pico/infrastructure/database/EventRepository.cpp",
  "src/pico/infrastructure/kv/PicoKvStore.cpp",
  "src/pico/infrastructure/threadpool/PicoThreadPool.cpp",
  "src/pico/infrastructure/websocket/PicoWebSocketServer.cpp",

  "src/pico/application/services/EventService.cpp",
  "src/pico/application/services/AuthService.cpp",
  "src/pico/application/services/PdfService.cpp",
  "src/pico/application/services/JobService.cpp",
  "src/pico/application/services/RuntimeStatusService.cpp",

  "src/pico/presentation/routes/RouteRegistry.cpp",
  "src/pico/presentation/middleware/MiddlewareRegistry.cpp",

  "src/pico/presentation/controllers/HomeController.cpp",
  "src/pico/presentation/controllers/HealthController.cpp",
  "src/pico/presentation/controllers/StatusController.cpp",
  "src/pico/presentation/controllers/KvController.cpp",
  "src/pico/presentation/controllers/EventController.cpp",
  "src/pico/presentation/controllers/AuthController.cpp",
  "src/pico/presentation/controllers/JobController.cpp",
  "src/pico/presentation/controllers/PdfController.cpp",
  "src/pico/presentation/controllers/WebSocketController.cpp",
]

include_dirs = [
  "include",
  "src",
]

deps = [
  "rix/rix",
  "rix/csv",
  "rix/debug"
]

defines = [
  "VIX_BACKEND_APP=1",
  "VIX_APP_NAME=pico",
]

compile_options = [
  "$<$<CXX_COMPILER_ID:MSVC>:/W4>",
  "$<$<CXX_COMPILER_ID:MSVC>:/permissive->",
  "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>",
  "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wextra>",
  "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wpedantic>",
]

link_options = [
]

compile_features = [
  "cxx_std_20",
]

packages = [
  "vix",
]

links = [
  "vix::vix",
]

resources = [
  ".env=.env",
  "public=public",
  "views=views",
  "storage=storage",
]
