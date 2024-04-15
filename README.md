# ChatApp
 A simple chat application using C++20 and Boost

# Notes
 This application was built using C++20 and utilizes the [Boost](https://www.boost.org/) libraries for networking. 

 The server will continuously run and have a network socket open so that multiple clients can asynchronously connect to the server at once. Each time a connection is made, the server will echo the username (address) of the new connection to the entire chat. Everyime a user sends a message in the chat, everyone will be able to see the username followed by the contents of the string message in the chat.

 Few quick notes on implementation:
  - Smart pointers `std::shared_ptr` are utilized to keep track of each connection that is made. `std::unordered_set` is used to keep track of all the different connections so that each connection is unique
  - The server sets up a network socket using `io_context` from the `boost::asio` library to allow the user to interact with the server. I set up my server to run on port 1337
  - A message queue `std::queue<std::string>` is setup for the server to asynchronously recieve messages and broadcast them to the entire chat room
  - Multithreading is utilized so that when a new client is added, that client is running on a separate thread