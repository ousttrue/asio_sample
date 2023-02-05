#define ASIO_ERROR_CATEGORY_NOEXCEPT noexcept
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

asio::awaitable<void> run_async(asio::io_context &io) {
  try {
    auto work = asio::make_work_guard(io);

    // dns
    asio::ip::tcp::resolver resolver{io};

    auto endpoint = co_await resolver.async_resolve({"www.boost.org", "http"},
                                                    asio::use_awaitable);

    work.reset();

  } catch (std::exception const &e) {
    std::cout << e.what() << std::endl;
  }
}

int main() {
  asio::io_context io;

  asio::co_spawn(io, run_async(io), asio::detached);
  std::thread thread{[&io] {
    io.run();
    std::cout << "io end" << std::endl;
  }};
  thread.join();
}
