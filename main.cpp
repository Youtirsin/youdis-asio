#include <iostream>
#include <thread>
#include <vector>

#include "youdis/server.hpp"

int main(int argc, char* argv[]) {
  constexpr int PORT = 6379;
  constexpr int NUM_THREADS = 8;

  try {
    boost::asio::io_context io_context;
    std::cout << "server running at 0.0.0.0:" << PORT << "." << std::endl;
    youdis::Server s(io_context, PORT);

    std::vector<std::thread> threads;
    for (int i = NUM_THREADS - 1; i > 0; --i) {
      threads.push_back(std::thread([&]() { io_context.run(); }));
    }

    io_context.run();

  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
