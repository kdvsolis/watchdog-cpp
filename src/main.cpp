// main.cpp
#include "WatchdogWorkflow.h"
#include "ConfigurationLoader.h"
#include "Logger.h"
#include "Factory.h"

int main() {
    // Create a logger instance
    auto logger = std::make_shared<Logger>("watchdog.log");

    // Create a configuration loader instance
    auto configLoader = std::make_shared<ConfigurationLoader>("config.json");

    // Create an API instance using the factory
    auto api = createAPI();

    // Create a WatchdogWorkflow instance with the logger
    WatchdogWorkflow watchdog(std::move(api), configLoader, logger);

    // Run the watchdog
    watchdog.run();

    return 0;
}
