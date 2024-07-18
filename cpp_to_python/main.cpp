#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ctime>
#include <ctime>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;


void initialize_winsock() {
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        exit(1);
    }
}

void cleanup_winsock() {
    WSACleanup();
}

void send_udp_message(const char* server_ip, int server_port, const char* message) {
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        return;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    int message_len = std::strlen(message);

    if (sendto(sockfd, message, message_len, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to send message: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        return;
    }

    std::cout << "Message sent" << std::endl;
    closesocket(sockfd);
}


int main() {
    initialize_winsock();

    const char* server_ip = "192.168.56.1"; // IP address of the receiver
    const int server_port = 8888;       

    string message;

    while (true) {
        cout << "Enter message to send (or 0 to exit): ";
        cin >> message;

        send_udp_message(server_ip, server_port, message.c_str());

        if (message == "0") {
            break;
        }

        
    }

    cleanup_winsock();
    return 0;
}
