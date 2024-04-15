//
// Created by DonRiz on 4/4/2024.
//

#include "Networking/Server/tcp_connection.h"
#include <iostream>

namespace App{
    TCPConnection::TCPConnection(io::ip::tcp::socket&& socket) : _socket(std::move(socket)) {
        std::stringstream name;
        name << _socket.remote_endpoint();
        _username = name.str();
    }


    void TCPConnection::Start(MessageHandler&& messageHandler, ErrorHandler&& errorHandler) {
        _messageHandler = std::move(messageHandler);
        _errorHandler = std::move(errorHandler);

        ReceiveMessage();
    }

    /**
     *
     * @param message contains the content of the client message
     *
     * Queues the message into our message queue and whenever we pop the message from the queue and we start the response
     */
    void TCPConnection::Post(const std::string &message) {
        bool queueIdle = _messageQueue.empty();
        _messageQueue.push(message);

        if (queueIdle)
        {
            SendMessage();
        }
    }

    void TCPConnection::ReceiveMessage() {
        io::async_read_until(_socket, _streamBuf, '\n', [self = shared_from_this()]
        (boost::system::error_code error, size_t bytesReceived)
        {
            self -> ReadAsync(error, bytesReceived);
        });
    }

    void TCPConnection::ReadAsync(boost::system::error_code error, size_t bytesReceived){
        if(error)
        {
            _socket.close();
            _errorHandler();
            return;
        }

        std::stringstream message;
        message << _username << ": " << std::istream(&_streamBuf).rdbuf();
        _streamBuf.consume(bytesReceived);
        std::cout << message.str();

        _messageHandler(message.str());
        ReceiveMessage();
    }

    void TCPConnection::SendMessage(){
        io::async_write(_socket, io::buffer(_messageQueue.front()), [self = shared_from_this()]
                (boost::system::error_code error, size_t bytesTransferred)
        {
            self -> WriteAsync(error, bytesTransferred);
        });
    }

    /**
     *
     * @param error
     * @param bytesTransferred
     */
    void TCPConnection::WriteAsync(boost::system::error_code error, size_t bytesTransferred){
        if(error)
        {
            _socket.close();
            _errorHandler();
            return;
        }

        _messageQueue.pop();

        if(!_messageQueue.empty())
        {
            SendMessage();
        }
    }
}