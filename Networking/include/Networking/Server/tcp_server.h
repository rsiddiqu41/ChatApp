//
// Created by DonRiz on 4/4/2024.
//
#include "tcp_connection.h"

#include <string>
#include <boost/asio.hpp>
#include <functional>
#include <optional>
#include <unordered_set>


namespace App{
    namespace io = boost::asio;
    enum class IPVersion
    {
        v4,
        v6
    };

    class TCPServer {
        // handlers for keeping track of when user joins/leaves/sends message
        using OnJoinHandler = std::function<void(TCPConnection::pointer)>;
        using OnLeaveHandler = std::function<void(TCPConnection::pointer)>;
        using OnClientMessageHandler = std::function<void(std::string)>;

    public:
        TCPServer(IPVersion ipVersion, int port);
        int Run();
        void Broadcast(const std::string& message);
        void startAccept();

        OnJoinHandler OnJoin;
        OnLeaveHandler OnLeave;
        OnClientMessageHandler OnClientMessage;

    private:
        IPVersion _ipVersion;
        int _port;

        io::io_context _ioContext;
        io::ip::tcp::acceptor  _acceptor;
        std::optional<io::ip::tcp::socket> _socket;
        std::unordered_set<TCPConnection::pointer> _connections {};
    };

}
