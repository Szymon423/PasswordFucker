#include <fix.hpp>
#include <log.hpp>
#include <runtime.hpp>
#include <configuration.hpp>
#include <http-server.hpp>
#include <utilities.hpp>
#include <passwords.hpp>

int main() {
    // Initialize logger
    Logger::init("./Password-Fucker.log", "Password-Fucker");
    Logger::info("Starting backend");

    // Register signal handlers
    try {
        Runtime::RegisterSignalHandles();
    }
    catch (const std::exception& e) {
        Logger::critical("Could not register termination signals.");
        return 1;
    }

    // Load configuration
    config::Configuration configuration;
    try {
        configuration = config::loadConfiguration();
    }
    catch (const std::exception& e) {
        Logger::info("Could not load configuration becouse of: {} Setting default values.", e.what());
        configuration.setDefault();
        config::saveConfiguration(configuration);
    }

    // set propper path to password manager database
    pass::PasswordManager::initialize(configuration.databasePath);

    // Initialize backend server
    Poco::Net::HTTPServer s(new MyRequestHandlerFactory, configuration.backendServerPort);
    s.start();
    while (Runtime::Run()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    s.stop();

    // Exit program
    Logger::info("Backend stopped");
    return 0;
}