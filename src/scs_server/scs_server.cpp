#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "shared/Packet.hpp"

using namespace scs;

int main(int argc, char const *argv[])
{
    int socket_desc, client_socket;
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    sockaddr_in server_addr;
    sockaddr_in client_addr;

    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(24343);

    if (bind(socket_desc,(struct sockaddr *)&server_addr , sizeof(server_addr)) < 0)
    {
        return -1;
    }

    if( listen(socket_desc, 3) < 0)
    {
        return -1;
    }

    while((client_socket = accept(socket_desc, nullptr, nullptr)) >= 0)
    {
        std::cout << "Client connected" << std::endl;
        unsigned char buf[1024];
        auto len = recv(client_socket, buf, sizeof(buf), 0);
        gsl::span<const unsigned char> buf_span (buf, len);

        shared::HELOPacket rhelo;
        
        if(rhelo.ReadFromSpan(buf_span)){
            std::cout << "HELO: " << rhelo.user_name() << std::endl;
        }

        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
    }
}   