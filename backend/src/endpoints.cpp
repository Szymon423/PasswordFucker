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
#include <crypto.hpp>

namespace Endpoints {

    std::string extractJwt(Poco::Net::HTTPServerRequest& request) {
        // Read auth header
        std::string authHeader = request.get("Authorization", "");
        
        if (authHeader.empty()) {
            throw std::runtime_error("Missing Authorization header");
        }
        
        // Check if it starts with "Bearer "
        const std::string bearerPrefix = "Bearer ";
        if (authHeader.substr(0, bearerPrefix.length()) != bearerPrefix) {
            throw std::runtime_error("Invalid Authorization header format - missing Bearer prefix");
        }
        
        // Extract token        
        std::string token = authHeader.substr(bearerPrefix.length());
        if (token.empty()) {
            throw std::runtime_error("Empty JWT token");
        }
        
        return token;
    }
    
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
            auto passwordOptions = pass::Password::Options::fromJson(requestBody);

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

            // Validate request
            auto userId = auth::AuthenticationManager::validateJWTToken(extractJwt(request));
            
            // Read passwords
            pass::PasswordManager manager;
            auto passwords = manager.getAllPasswords();
            nlohmann::json resoult = nlohmann::json::array();
            for (const auto& password : passwords) {
                if (password.userId == userId) {
                    auto decryptedPassword = pass::PasswordCrypto::decrypt(password, userId);
                    resoult.push_back(decryptedPassword.toJson());
                }
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

            // Validate request
            auto userId = auth::AuthenticationManager::validateJWTToken(extractJwt(request));

            // Parse JSON from request body
            nlohmann::json requestBody = nlohmann::json::parse(request.stream());

            // Parse and update password
            pass::PasswordManager manager;
            auto password = pass::Password::fromJson(requestBody);
            auto encryptedPassword = pass::PasswordCrypto::encrypt(password, userId);
            manager.addPassword(encryptedPassword);
            
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

            // Validate request
            auto userId = auth::AuthenticationManager::validateJWTToken(extractJwt(request));

            // Parse JSON from request body
            nlohmann::json requestBody = nlohmann::json::parse(request.stream());

            // Parse and update password
            pass::PasswordManager manager;
            auto password = pass::Password::fromJson(requestBody);
            auto decryptedPassword = pass::PasswordCrypto::decrypt(password, userId);
            manager.updatePassword(decryptedPassword);
            
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

            // Validate request
            auto userId = auth::AuthenticationManager::validateJWTToken(extractJwt(request));

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
                
                // register Crypto instance under this user ID
                CryptoManager::registerCrypto(user.value().password, user.value().id);

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

            // Encrypt User
            auth::User encryptedUser;
            Crypto crypto(user.password);
            encryptedUser.login = crypto.encrypt(user.login);
            encryptedUser.password = crypto.encrypt(user.password);
            encryptedUser.name = crypto.encrypt(user.name);
            encryptedUser.surname = crypto.encrypt(user.surname);
            encryptedUser.id = user.id;

            manager.addUser(encryptedUser);
            
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