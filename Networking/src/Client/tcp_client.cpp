
#include <iostream>
#include "Networking/Client/tcp_client.h"

namespace App{
    TCPClient::TCPClient(const std::string& address, int port) : _socket(_ioContext)
    {
        io::ip::tcp::resolver resolver(_ioContext);
        _endpoints = resolver.resolve(address, std::to_string(port));
    }

    void TCPClient::Post(const std::string &message)
    {
        bool queueIdle = _outgoingMessages.empty();
        _outgoingMessages.push(message);

        if(queueIdle)
        {
            SendMessage();
        }
    }

    void TCPClient::Run()
    {
        io::async_connect(_socket, _endpoints,
                      [this](boost::system::error_code error, io::ip::tcp::endpoint endPoint)
        {
            if(!error)
            {
                ReceiveMessage();
            }
        });

        _ioContext.run();
    }

    void TCPClient::Stop()
    {
        boost::system::error_code error;
        _socket.close(error);

        if(error)
        {
            std::cerr << "Client error: " << error.message() << std::endl;
        }

    }

    void TCPClient::ReadAsync(boost::system::error_code error, size_t bytesReceived)
    {
        if(error)
        {
            Stop();
            return;
        }
        std::stringstream message;
        message << std::istream{&_streamBuf}.rdbuf();
        OnMessage(message.str());
        ReceiveMessage();
    }

    void TCPClient::ReceiveMessage()
    {
        io::async_read_until(_socket, _streamBuf, '\n',
                 [this](boost::system::error_code error, size_t bytesReceived)
        {
            ReadAsync(error, bytesReceived);
        });
    }

    void TCPClient::SendMessage()
    {
        io::async_write(_socket, io::buffer(_outgoingMessages.front()),
                        [this](boost::system::error_code error, size_t bytesTransferred) {
            WriteAsync(error, bytesTransferred);
        });
    }

    void TCPClient::WriteAsync(boost::system::error_code error, size_t bytesTransferred)
    {
        if(error)
        {
            Stop();
            return;
        }

        _outgoingMessages.pop();

        if (!_outgoingMessages.empty()) {
            SendMessage();
        }
    }
}
