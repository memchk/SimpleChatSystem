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
    unsigned char buf[3];
    ser::span span_buf = buf;

    packet::HELOPacket packet("memchk", "#imepic");
    auto [ok, len] = packet.Serialize(span_buf);

    if(ok) {
        for (auto &i : span_buf.first(len)) {
            std::cout << std::hex << (int)i << " ";
        }
    } else {
        std::cout << "failure!";
    }
    std::cout << std::endl;
}   