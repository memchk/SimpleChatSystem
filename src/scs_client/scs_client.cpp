#include<arpa/inet.h>
#include<sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <unistd.h>

#include "shared/Packet.hpp"
#include "shared/Serialization.hpp"

using namespace scs::shared;

int main(int argc, char const *argv[])
{
    packet::HELOPacket packet("memchk");
    std::cout << std::endl;
}   