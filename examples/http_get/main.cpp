#define ASIO_ERROR_CATEGORY_NOEXCEPT noexcept
#include <asio.hpp>
#include <iostream>
#include <memory>
#include <sstream>

int main() {
  try {
    asio::io_service io;
    asio::ip::tcp::resolver resolver{io};
    asio::ip::tcp::resolver::query query{"www.boost.org", "http"};
    asio::ip::tcp::socket socket{io};
    resolver.async_resolve(
        query, [&](auto const &, asio::ip::tcp::resolver::iterator endpoint) {
          std::cout << "resolve" << std::endl;
          asio::async_connect(socket, endpoint, [&](auto const &, auto) {
            std::cout << "connect" << std::endl;
            std::ostringstream os;
            os << "GET "
               << "/"
               << " HTTP/1.0\r\n";
            os << "Host: "
               << "www.boost.org"
               << "\r\n";
            os << "Accept: */*\r\n";
            os << "Connection: close\r\n\r\n";
            asio::async_write(
                socket, asio::buffer(os.str().data(), os.str().size()),
                [&](auto error_code, auto) {
                  std::cout << "write" << std::endl;
                  auto response = std::make_shared<asio::streambuf>();
                  asio::async_read(socket, *response,
                                   [&, response](auto, auto) {
                                     std::cout << "read" << std::endl;
                                     std::istream iss(response.get());
                                     std::string buf;
                                     while (iss && std::getline(iss, buf)) {
                                       std::cout << buf << std::endl;
                                     }
                                   });
                });
          });
        });
    io.run();
  } catch (std::exception const &e) {
    std::cout << e.what() << std::endl;
  }
}