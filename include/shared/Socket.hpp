#pragma once
#include <sys/socket.h>

namespace scs::shared {
class Socket {
 private:
  int socket_fd_;

 public:
  Socket();
  ~Socket();
};

Socket::Socket() {
}

Socket::~Socket() {}
    close()
}  // namespace scs::shared
