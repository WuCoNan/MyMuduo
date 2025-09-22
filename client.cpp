#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

using namespace std;

class SimpleTCPClient {
private:
    int sockfd;
    string server_ip;
    int port;

public:
    SimpleTCPClient(const string& ip = SERVER_IP, int port_num = PORT) 
        : sockfd(-1), server_ip(ip), port(port_num) {}

    ~SimpleTCPClient() {
        disconnect();
    }

    bool connectToServer() {
        // 创建socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Socket creation failed");
            return false;
        }

        // 设置服务器地址
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        
        // 转换IP地址
        if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
            perror("Invalid address");
            close(sockfd);
            sockfd = -1;
            return false;
        }

        // 连接服务器
        if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Connection failed");
            close(sockfd);
            sockfd = -1;
            return false;
        }

        cout << "Connected to server " << server_ip << ":" << port << endl;
        return true;
    }

    bool sendMessage(const string& message) {
        if (sockfd < 0) {
            cerr << "Not connected to server" << endl;
            return false;
        }

        ssize_t bytes_sent = send(sockfd, message.c_str(), message.length(), 0);
        if (bytes_sent < 0) {
            perror("Send failed");
            return false;
        }

        cout << "Sent " << bytes_sent << " bytes: " << message << endl;
        return true;
    }

    string receiveMessage() {
        if (sockfd < 0) {
            return "Not connected";
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received < 0) {
            perror("Receive failed");
            return "Receive error";
        } else if (bytes_received == 0) {
            return "Server closed connection";
        }

        buffer[bytes_received] = '\0';
        return string(buffer);
    }

    void disconnect() {
        if (sockfd >= 0) {
            close(sockfd);
            sockfd = -1;
            cout << "Disconnected from server" << endl;
        }
    }

    bool isConnected() const {
        return sockfd >= 0;
    }
};

int main() {
    SimpleTCPClient client;

    if (!client.connectToServer()) {
        return 1;
    }

    cout << "Simple TCP Client started. Type 'quit' to exit." << endl;
    cout << "Enter messages to send to the server:" << endl;

    string input;
    while (true) {
        cout << "> ";
        getline(cin, input);

        if (input == "quit") {
            break;
        }

        if (!client.sendMessage(input)) {
            cerr << "Failed to send message" << endl;
            break;
        }

        cout<<"client received message:"<<client.receiveMessage()<<endl;
       
    }

    client.disconnect();
    return 0;
}