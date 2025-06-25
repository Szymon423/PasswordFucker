#include <endpoints.hpp>
#include <log.hpp>
#include <nlohmann/json.hpp>
#include <exception>
#include <tuple>
#include <string>
#include <vector>
#include <configuration.hpp>
#include <passwords.hpp>
#include <auth.hpp>

namespace Endpoints {
    void getConfiguration(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept {
        try {
            Logger::trace("Reading configuration.");
            config::Configuration configuration = config::loadConfiguration();

            // Response
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            out << configuration.toJson().dump();
        }
        catch (const std::exception& e) {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = { {"status", "error"}, {"message", "Internal server error"} };
            out << errorJson.dump();
            Logger::error("Error reading configuration: {}", e.what());
        }
        catch (...) {
            // Catch any other unexpected exceptions
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", "An unexpected error occurred"}};
            out << errorJson.dump();
            Logger::error("Unexpected error occurred while reading configuration");
        }
    }

    void updateConfiguration(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept {
        try {
            Logger::trace("Updating configuration.");

            // Parse JSON from request body
            nlohmann::json requestBody = nlohmann::json::parse(request.stream());

            // Parse Configuration
            auto configuartion = config::Configuration::fromJson(requestBody);
            config::saveConfiguration(configuartion);
            
            // Response
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            nlohmann::json j = {{"message", "Configuration updated"}};
            std::ostream& out = response.send();
            out << j.dump();
        }
        catch (const std::exception& e) {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", e.what()}};
            out << errorJson.dump();
            Logger::error("Error updating configuration: {}", e.what());
        }
        catch (...) {
            // Catch any other unexpected exceptions
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", "An unexpected error occurred"}};
            out << errorJson.dump();
            Logger::error("Unexpected error occurred while updating configuration");
        }
    }

    void generatePassword(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept {
        try {
            Logger::trace("Generating password.");
            
            // Parse password options
            nlohmann::json requestBody = nlohmann::json::parse(request.stream());
            auto passwordOptions = pass::PasswordGenerator::Options::fromJson(requestBody);

            // Generate password
            auto password = pass::PasswordGenerator::generate(passwordOptions);
            Logger::trace("Generated password: {}", password);

            // Prepare response
            nlohmann::json resoult;
            resoult["password"] = password;

            // Response
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            out << resoult.dump();
        }
        catch (const std::exception& e) {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = { {"status", "error"}, {"message", "Internal server error"} };
            out << errorJson.dump();
            Logger::error("Error generating password: {}", e.what());
        }
        catch (...) {
            // Catch any other unexpected exceptions
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", "An unexpected error occurred"}};
            out << errorJson.dump();
            Logger::error("Unexpected error occurred while generating password");
        }
    }

    void getPasswords(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept {
        try {
            Logger::trace("Reading passwords.");
            
            // Read passwords
            pass::PasswordManager manager;
            auto passwords = manager.getAllPasswords();

            nlohmann::json resoult = nlohmann::json::array();
            for (const auto& password : passwords) {
                resoult.push_back(password.toJson());
            }

            // Response
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            out << resoult.dump();
        }
        catch (const std::exception& e) {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = { {"status", "error"}, {"message", "Internal server error"} };
            out << errorJson.dump();
            Logger::error("Error reading passwords: {}", e.what());
        }
        catch (...) {
            // Catch any other unexpected exceptions
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", "An unexpected error occurred"}};
            out << errorJson.dump();
            Logger::error("Unexpected error occurred while reading passwords");
        }
    }

    void addPassword(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept {
        try {
            Logger::trace("Adding password.");

            // Parse JSON from request body
            nlohmann::json requestBody = nlohmann::json::parse(request.stream());

            // Parse and update password
            pass::PasswordManager manager;
            auto password = pass::Password::fromJson(requestBody);
            manager.addPassword(password);
            
            // Response
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            nlohmann::json j = {{"message", "Configuration updated"}};
            std::ostream& out = response.send();
            out << j.dump();
        }
        catch (const std::exception& e) {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", e.what()}};
            out << errorJson.dump();
            Logger::error("Error adding password: {}", e.what());
        }
        catch (...) {
            // Catch any other unexpected exceptions
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", "An unexpected error occurred"}};
            out << errorJson.dump();
            Logger::error("Unexpected error occurred while adding password");
        }
    }

    void updatePassword(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept {
        try {
            Logger::trace("Updating password.");

            // Parse JSON from request body
            nlohmann::json requestBody = nlohmann::json::parse(request.stream());

            // Parse and update password
            pass::PasswordManager manager;
            auto password = pass::Password::fromJson(requestBody);
            manager.updatePassword(password);
            
            // Response
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            nlohmann::json j = {{"message", "Configuration updated"}};
            std::ostream& out = response.send();
            out << j.dump();
        }
        catch (const std::exception& e) {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", e.what()}};
            out << errorJson.dump();
            Logger::error("Error updating password: {}", e.what());
        }
        catch (...) {
            // Catch any other unexpected exceptions
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", "An unexpected error occurred"}};
            out << errorJson.dump();
            Logger::error("Unexpected error occurred while updating password");
        }
    }

    void removePassword(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept {
        try {
            Logger::trace("Removing password.");

            // Parse JSON from request body
            nlohmann::json requestBody = nlohmann::json::parse(request.stream());

            // Parse and update password
            pass::PasswordManager manager;
            auto password = pass::Password::fromJson(requestBody);
            manager.removePassword(password.id);
            
            // Response
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            nlohmann::json j = {{"message", "Configuration updated"}};
            std::ostream& out = response.send();
            out << j.dump();
        }
        catch (const std::exception& e) {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", e.what()}};
            out << errorJson.dump();
            Logger::error("Error removing password: {}", e.what());
        }
        catch (...) {
            // Catch any other unexpected exceptions
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", "An unexpected error occurred"}};
            out << errorJson.dump();
            Logger::error("Unexpected error occurred while removing password");
        }
    }

    void login(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept {
        try {
            Logger::trace("Login authenication.");

            // Parse JSON from request body
            nlohmann::json requestBody = nlohmann::json::parse(request.stream());

            // read login and password
            std::string login = requestBody.at("login").get<std::string>();
            std::string password = requestBody.at("password").get<std::string>();

            auto user = auth::AuthenticationManager::checkCredentials(login, password);

            if (user.has_value()) {
                std::string token = auth::AuthenticationManager::generateJWTToken(user.value());
                
                // Successful response
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setContentType("application/json");
                nlohmann::json successJson = {
                    {"status", "success"},
                    {"message", "Login successful"},
                    {"token", token},
                    {"user", {
                        {"login", login}
                    }}
                };
                
                std::ostream& out = response.send();
                out << successJson.dump();
                Logger::info("User {} successfully authenticated", login);
            } 
            else {
                // Failed authentication
                response.setStatus(Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED); // 401
                response.setContentType("application/json");
                nlohmann::json errorJson = {
                    {"status", "error"},
                    {"message", "Invalid credentials"}
                };
                
                std::ostream& out = response.send();
                out << errorJson.dump();
                Logger::warn("Failed login attempt for user: {}", login);
            }
        }
        catch (const std::invalid_argument& e) {
            // Błąd w formacie żądania
            response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST); // 400
            response.setContentType("application/json");
            nlohmann::json errorJson = {
                {"status", "error"},
                {"message", "Invalid request format"},
                {"details", e.what()}
            };
            
            std::ostream& out = response.send();
            out << errorJson.dump();
            Logger::error("Bad request format: {}", e.what());
        }
        catch (const std::exception& e) {
            // Ogólny błąd serwera
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR); // 500
            response.setContentType("application/json");
            nlohmann::json errorJson = {
                {"status", "error"},
                {"message", "Internal server error"},
                {"details", e.what()}
            };
            
            std::ostream& out = response.send();
            out << errorJson.dump();
            Logger::error("Error during authentication: {}", e.what());
        }
        catch (...) {
            // Nieoczekiwany błąd
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            nlohmann::json errorJson = {
                {"status", "error"},
                {"message", "An unexpected error occurred"}
            };
            
            std::ostream& out = response.send();
            out << errorJson.dump();
            Logger::error("Unexpected error during authentication");
        }
    }

    void registerUser(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept {
        try {
            Logger::trace("Registering new user.");

            // Parse JSON from request body
            nlohmann::json requestBody = nlohmann::json::parse(request.stream());

            // Parse and update password
            auth::AuthenticationManager manager;
            auto user = auth::User::fromJson(requestBody);
            manager.addUser(user);
            
            // Response
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            nlohmann::json j = {{"message", "User registered"}};
            std::ostream& out = response.send();
            out << j.dump();
        }
        catch (const std::exception& e) {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", e.what()}};
            out << errorJson.dump();
            Logger::error("Error registering user: {}", e.what());
        }
        catch (...) {
            // Catch any other unexpected exceptions
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            nlohmann::json errorJson = {{"status", "error"}, {"message", "An unexpected error occurred"}};
            out << errorJson.dump();
            Logger::error("Unexpected error occurred while registering user");
        }
    }
}