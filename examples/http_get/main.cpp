#define ASIO_ERROR_CATEGORY_NOEXCEPT noexcept
#include <asio.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

int main() {
  try {
    asio::io_context io;
    auto work = asio::make_work_guard(io);
    std::thread thread{[&io] {
      io.run();
      std::cout << "io end" << std::endl;
    }};

    // dns
    asio::ip::tcp::resolver resolver{io};

    auto resolve_future =
        resolver.async_resolve({"www.boost.org", "http"}, asio::use_future);

    // resolve_future.wait();
    auto endpoint = resolve_future.get();
    std::cout << "resolved" << std::endl;

    // connect
    asio::ip::tcp::socket socket{io};
    auto connect_future =
        asio::async_connect(socket, endpoint, asio::use_future);
    connect_future.get();
    std::cout << "connected" << std::endl;

    // request
    std::ostringstream os;
    os << "GET "
       << "/"
       << " HTTP/1.0\r\n";
    os << "Host: "
       << "www.boost.org"
       << "\r\n";
    os << "Accept: */*\r\n";
    os << "Connection: close\r\n\r\n";
    auto write_future = asio::async_write(
        socket, asio::buffer(os.str().data(), os.str().size()),
        asio::use_future);
    write_future.get();
    std::cout << "writen" << std::endl;

    // response
    asio::streambuf response;
    auto read_future = asio::async_read(socket, response, asio::use_future);
    std::cout << "read" << std::endl;

    work.reset();
    thread.join();

    // print response
    std::istream iss(&response);
    std::string buf;
    while (iss && std::getline(iss, buf)) {
      std::cout << buf << std::endl;
    }
  } catch (std::exception const &e) {
    std::cout << e.what() << std::endl;
  }
}
