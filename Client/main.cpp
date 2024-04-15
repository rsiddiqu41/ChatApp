#include <Networking/Client/tcp_client.h>
#include <iostream>

int main(int argc, char* argv[]) {
    App::TCPClient tcpClient{"localhost", 1337};

    tcpClient.OnMessage = [](const std::string& message) {
        std::cout << message;
    };

    // server will run on main thread while client while run on separate thread
    std::thread t{[&tcpClient] () { tcpClient.Run(); }};

    while(true) {
        std::string message;
        getline(std::cin, message);

        // If the client enters the quit request we will stop
        if (message == "\\q") break;

        message += "\n";

        tcpClient.Post(message);
    }

    tcpClient.Stop();
    t.join();
    return 0;
}
