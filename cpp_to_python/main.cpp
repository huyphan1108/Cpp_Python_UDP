#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

// Structure to hold message and its timestamp
struct MessageLog {
    string message;
    string timestamp;
};

// Initialize Winsock
void initialize_winsock() {
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cerr << "WSAStartup failed: " << iResult << endl;
        exit(1);
    }
}

// Cleanup Winsock
void cleanup_winsock() {
    WSACleanup();
}

// Send UDP message
void send_udp_message(const char* server_ip, int server_port, const char* message) {
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
        cerr << "Failed to create socket: " << WSAGetLastError() << endl;
        return;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    int message_len = strlen(message);

    if (sendto(sockfd, message, message_len, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Failed to send message: " << WSAGetLastError() << endl;
        closesocket(sockfd);
        return;
    }

    cout << "Message sent: " << message << endl;
    closesocket(sockfd);
}

// Get current timestamp
string get_current_timestamp() {
    auto now = chrono::system_clock::now();
    auto now_time = chrono::system_clock::to_time_t(now);
    auto now_us = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()) % 1000000;

    tm buf;
    localtime_s(&buf, &now_time);

    ostringstream oss;
    oss << put_time(&buf, "%S");
    oss << '.' << setw(6) << setfill('0') << now_us.count();

    return oss.str();
}

int main() {
    initialize_winsock();

    const char* server_ip = "192.168.56.1"; // IP address of the receiver
    const int server_port = 8888;       // Port of the receiver

    string message;
    vector<MessageLog> message_logs; 

    while (true) {
        cout << "Enter message to send (or 0 to exit): ";
        cin >> message;

        if (message == "0") {
            break;
        }

        string timestamp = get_current_timestamp();
        string message_with_timestamp = message + " at "  + timestamp ;

        //cout << "Timestamp: " << timestamp << endl;  // Debug print
        send_udp_message(server_ip, server_port, message.c_str());
        send_udp_message(server_ip, server_port, timestamp.c_str());
    }

    cleanup_winsock();

    return 0;
}
