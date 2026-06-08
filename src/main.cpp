/**
 * @file main.cpp
 * @brief Entry point for the pico backend application.
 */

#include <pico/app/AppBootstrap.hpp>

/**
 * @brief Start the backend application.
 *
 * The main function stays intentionally small. Application setup,
 * middleware registration, route registration, and server startup are
 * delegated to pico::app::AppBootstrap.
 *
 * @return Process exit code.
 */
int main()
{
  pico::app::AppBootstrap bootstrap;
  return bootstrap.run();
}
