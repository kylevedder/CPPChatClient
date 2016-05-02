#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions

#define BUFFER_SIZE (4096)
#define PORT ("4000")

using namespace std;

int main(int argc, char const *argv[]) {

        int status;
        struct addrinfo host_info; // The struct that getaddrinfo() fills up with data.
        struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.

        //NULL out host_info
        memset(&host_info, 0, sizeof host_info);

        host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
        host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.

        status = getaddrinfo("127.0.0.1", PORT, &host_info, &host_info_list);

        if (status != 0) {
                std::cout << "getaddrinfo error" << gai_strerror(status) << endl;
                return -1;
        }

        int socketfd; // The socket descripter
        socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                          host_info_list->ai_protocol);

        if (socketfd == -1) {
                std::cout << "socket error "<< std::endl;
                return -1;
        }

        std::cout << "Connect()ing..."  << std::endl;
        status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
        if (status < 0) {
                std::cout << "connect error"<< std::endl;
                return -1;
        }


        ssize_t bytes_recieved;
        char incomming_data_buffer[BUFFER_SIZE];
        while(true) {

                const char  *msg = "ping\n";
                std::cout << "send()ing message... \"" << msg << "\""  << std::endl;
                int len;
                ssize_t bytes_sent;
                len = strlen(msg);
                bytes_sent = send(socketfd, msg, len, 0);


                for(bytes_recieved = 0;; ) {
                        //init
                        bytes_recieved = 0;
                        memset( incomming_data_buffer, '\0', sizeof(char)*BUFFER_SIZE );

                        //read response
                        bytes_recieved = recv(socketfd, incomming_data_buffer,BUFFER_SIZE, 0);
                        // If no data arrives, the program will just wait here until some data arrives.
                        if (bytes_recieved == 0) {
                                std::cout << "host shut down." << std::endl;
                                break;
                        }
                        else if (bytes_recieved < 0) {
                                std::cout << "recieve error!" << std::endl;
                                break;
                        }

                        incomming_data_buffer[bytes_recieved] = '\0';
                        std::cout << "Read data: \"" << incomming_data_buffer << "\""<< std::endl;

                        //Newline read, send response
                        if(bytes_recieved > 0 && incomming_data_buffer[bytes_recieved - 1] == '\n') {
                                break;
                        }
                }
        }


        return 0;
}
