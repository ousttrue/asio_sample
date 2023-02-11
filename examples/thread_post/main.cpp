#include <asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

std::mutex mutex;
void print(std::thread::id id, const std::string &s) {
  std::lock_guard<std::mutex> lock(mutex);
  std::cout << id << s << std::endl;
}

void fB() { print(std::this_thread::get_id(), " call fB"); }

void fA(asio::io_service &io_service) {
  static int selector = 0;

  ++selector;
  if (selector % 2 == 0) {
    print(std::this_thread::get_id(), " post");
    io_service.post(fB);
  } else {
    print(std::this_thread::get_id(), " dispatch");
    io_service.dispatch(fB);
  }
}

bool startAFunction() {
  print(std::this_thread::get_id(),
        " Enter a non-empty string to run A function");

  std::string input;
  getline(std::cin, input);
  return input.length() == 0 ? false : true;
}

int main() {
  std::cout << std::this_thread::get_id() << std::endl;

  asio::io_service io;
  asio::io_service::work work(io);

  std::list<std::thread> group;
  const int count = 3;
  for (int i = 0; i < count; ++i) {
    group.emplace_back(std::thread{[&io]() { io.run(); }});
  }

  while (startAFunction()) {
    io.post(std::bind(fA, std::ref(io)));
  }

  std::cout << "stop io_service" << std::endl;
  io.stop();
  for (auto &t : group) {
    t.join();
  }

  std::cout << "end application" << std::endl;
}
