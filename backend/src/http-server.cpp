#include <http-server.hpp>
#include <runtime.hpp>
#include <log.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <endpoints.hpp>
#include <configuration.hpp>

std::map<std::pair<std::string, std::string>, MyRequestHandler::RouteHandler> MyRequestHandler::routes = {
    {{"GET",  "/api/configuration/get"}, std::bind(&Endpoints::getConfiguration, std::placeholders::_1, std::placeholders::_2)},
    {{"POST", "/api/configuration/update"}, std::bind(&Endpoints::updateConfiguration, std::placeholders::_1, std::placeholders::_2)},
    {{"GET",  "/api/passwords/get"}, std::bind(&Endpoints::getPasswords, std::placeholders::_1, std::placeholders::_2)},
    {{"POST", "/api/passwords/add"}, std::bind(&Endpoints::addPassword, std::placeholders::_1, std::placeholders::_2)},
    {{"POST", "/api/passwords/update"}, std::bind(&Endpoints::updatePassword, std::placeholders::_1, std::placeholders::_2)},
    {{"POST", "/api/passwords/delete"}, std::bind(&Endpoints::removePassword, std::placeholders::_1, std::placeholders::_2)},
    {{"POST", "/api/authentication/login"}, std::bind(&Endpoints::login, std::placeholders::_1, std::placeholders::_2)},
    {{"POST", "/api/authentication/register"}, std::bind(&Endpoints::registerUser, std::placeholders::_1, std::placeholders::_2)}
};

void MyRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    Poco::URI uri(request.getURI());
    std::string path = uri.getPath();
    std::string method = request.getMethod();

    setCorsHeaders(response);

    if (method == "OPTIONS") {
        handleOptions(request, response);
        return;
    }

    auto route = routes.find({method, path});
    if (route != routes.end()) {
        route->second(request, response);
        return;
    }
    handleNotFound(request, response);
}

void MyRequestHandler::handleNotFound(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
    response.setContentType("text/html");
    std::ostream& out = response.send();
    out << "<html><body><h1>404 Not Found</h1><p>This page was not found.</p></body></html>";
}

void MyRequestHandler::handleOptions(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.send();
}

void MyRequestHandler::setCorsHeaders(Poco::Net::HTTPServerResponse& response) {
    response.set("Access-Control-Allow-Origin", "*");
    response.set("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");
    response.set("Access-Control-Allow-Credentials", "true");
}

Poco::Net::HTTPRequestHandler* MyRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
    return new MyRequestHandler;
}