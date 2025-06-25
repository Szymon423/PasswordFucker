#pragma once

#include <fix.hpp>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

/// @brief Namespace for endpoints handling
namespace Endpoints {
    /// @brief Gets configuration 
    /// @param request HTTP request
    /// @param response HTTP response
    void getConfiguration(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept;

    /// @brief Updates configuration
    /// @param request HTTP request
    /// @param response HTTP response
    void updateConfiguration(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept;

    /// @brief Function to generate password
    /// @param request HTTP request
    /// @param response HTTP response
    void generatePassword(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept;

    /// @brief Reads all passwords
    /// @param request HTTP request
    /// @param response HTTP response
    void getPasswords(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept;

    /// @brief Adds new password
    /// @param request HTTP request
    /// @param response HTTP response
    void addPassword(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept;
    
    /// @brief Updates password
    /// @param request HTTP request
    /// @param response HTTP response
    void updatePassword(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept;

    /// @brief Remove password
    /// @param request HTTP request
    /// @param response HTTP response
    void removePassword(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept;

    /// @brief Login to app
    /// @param request HTTP request
    /// @param response HTTP response
    void login(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept;

    /// @brief Login to app
    /// @param request HTTP request
    /// @param response HTTP response
    void registerUser(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) noexcept;
}