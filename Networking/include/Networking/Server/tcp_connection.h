//
// Created by DonRiz on 4/4/2024.
//

#include <boost/asio.hpp>
#include <memory>
#include <queue>

namespace App {
    using boost::asio::ip::tcp;
    namespace io = boost::asio;


    using MessageHandler = std::function<void(std::string)>;
    using ErrorHandler = std::function<void()>;

    class TCPConnection : public std::enable_shared_from_this<TCPConnection> {
    public:
        // use shared pointer to reference connection so that it only gets destroyed when there are no more references to the connection
        using pointer = std::shared_ptr<TCPConnection>;

        // pass in double reference (right hand value) for socket, so we can use std::move instead of creating a new socket each time
        static pointer Create(io::ip::tcp::socket&& socket) {
            // move our socket into our connection object and create a smart pointer (shared_ptr) to reference the connection
            return pointer(new TCPConnection(std::move(socket)));
        }

        inline const std::string& GetUsername() { return _username; }

        tcp::socket& Socket() {
            return _socket;
        }


        void Start(MessageHandler&& messageHandler, ErrorHandler&& errorHandler);

        // send message to client using a queue
        void Post(const std::string& message);

    private:
        explicit TCPConnection(io::ip::tcp::socket&& socket);

        // Waits for a new message from the client
        void ReceiveMessage();
        void ReadAsync(boost::system::error_code error, size_t bytesReceived);

        void SendMessage();
        void WriteAsync(boost::system::error_code error, size_t bytesTransferred);



    private:
        tcp::socket _socket;
        std::string _username;

        std::queue<std::string> _messageQueue;
        io::streambuf _streamBuf {65536};

        MessageHandler _messageHandler;
        ErrorHandler _errorHandler;
    };
}