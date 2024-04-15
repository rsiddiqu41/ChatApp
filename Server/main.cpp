#include <boost/asio.hpp>
#include <iostream>
#include <Networking/Server/tcp_server.h>

int main() {

    App::TCPServer tcpServer {App::IPVersion::v4, 1337};

    // When the user joins, print that the user has joined the server
    tcpServer.OnJoin = [](App::TCPConnection::pointer server) {
        std::cout << "User has joined the server: " << server->GetUsername() << std::endl;
    };

    tcpServer.OnLeave = [](App::TCPConnection::pointer server) {
        std::cout << "User has left the server: " << server->GetUsername() << std::endl;
    };

    tcpServer.OnClientMessage = [&tcpServer](const std::string& message) {
        // Parse the message
        // Do game server things

        // Send message to client
        tcpServer.Broadcast(message);
    };

    tcpServer.Run();
    return 0;
}
