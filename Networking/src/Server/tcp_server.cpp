//
// Created by DonRiz on 4/4/2024.
//

#include "Networking/Server/tcp_server.h"

#include <iostream>

namespace App{

    using boost::asio::ip::tcp;

    TCPServer::TCPServer(IPVersion ipv, int port) : _ipVersion(ipv), _port(port),
        _acceptor(_ioContext, tcp::endpoint(_ipVersion == IPVersion::v4 ? tcp::v4() : tcp::v6(), _port)){}

    int TCPServer::Run() {
        try
        {
            startAccept();
            _ioContext.run();
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return -1;
        }
        return 0;
    }

    /**
     *
     * @param message
     *
     * For all of the concurrent connections, we will add all the messages to the queue
     */
    void TCPServer::Broadcast(const std::string &message) {
        for (auto& connection : _connections)
        {
            connection->Post(message);
        }
    }

    void TCPServer::startAccept() {

        // construct our socket using our ioContext. optional.emplace will destroy any perviously stored values using emplace destructor
        _socket.emplace(_ioContext);

        // asynchronously accept the connection
        _acceptor.async_accept(*_socket, [this](const boost::system::error_code& error){
            // pass in socket to TCP connection object using std::move. This is less expensive than passing in the copy of the socket object, and it resets the optional value to null
            auto connection = TCPConnection::Create(std::move(*_socket));

            if(OnJoin)
            {
                OnJoin(connection);
            }

            _connections.insert(connection);

            if (!error)
            {
                connection->Start
                (
                    [this](const std::string& message) { if (OnClientMessage) OnClientMessage(message); },
                    [&, weak =std::weak_ptr(connection)]
                    {
                        // Check to see if the same connection exists in our set. This shouldn't happen so if it does we will erase the duplicate connection
                        if (auto shared = weak.lock(); shared && _connections.erase(shared))
                        {
                            if (OnLeave) OnLeave(shared);
                        }
                    }
                );
            }
            else{
                std::cout << error.message() << std::endl;
            }
            startAccept();
        });
    }
}