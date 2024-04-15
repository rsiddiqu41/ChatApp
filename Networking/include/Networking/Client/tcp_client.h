#pragma once
#include <boost/asio.hpp>
#include <queue>


namespace App{
    namespace io = boost::asio;
    using MessageHandler = std::function<void(std::string)>;

    class TCPClient {
    public:
        TCPClient(const std::string& address, int port);

        void Run();
        void Stop();
        void Post(const std::string& message);

    private:
        void ReceiveMessage();
        void ReadAsync(boost::system::error_code error, size_t bytesReceived);

        void SendMessage();
        void WriteAsync(boost::system::error_code error, size_t bytesTransferred);

    public:
        MessageHandler OnMessage;

    private:
        io::io_context _ioContext {};
        io::ip::tcp::socket _socket;

        // resolves endpoints based on hostname passed in. basically maps hostname -> endpoint
        io::ip::tcp::resolver::results_type _endpoints;

        io::streambuf _streamBuf{65536};
        std::queue<std::string> _outgoingMessages {};
    };
};