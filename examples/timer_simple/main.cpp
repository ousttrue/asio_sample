#include <asio.hpp>
#include <iostream>

int main() {
  asio::io_context io;

  asio::steady_timer timer(io);
  timer.expires_after(std::chrono::seconds(1));

  timer.async_wait(
      [](asio::error_code ec) { std::cout << "expired" << std::endl; });

  std::cout << "start..." << std::endl;
  io.run();
  std::cout << "end" << std::endl;

  return 0;
}
