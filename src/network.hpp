#pragma once

#include <chrono>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "main.hpp"
#include "protocol.hpp"
#include "runtime.hpp"

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif


#ifndef _WIN32
using SOCKET = int;
#endif


class closing: public std::exception {
    const char _data[21] = "Closing the program";
public:
    virtual const char* what() const noexcept;
};


class Socket {
private:
    SOCKET sockd;
    bool closed = true;
    static bool initialized;
public:
    Socket(const std::string port);
    Socket(const SOCKET sockd);
    ~Socket();
    int disconnect();
    int send(const std::string &data);
    int recv(std::string &data, const int size, const int timeout);
    int accept(const int timeout, sockaddr_storage &remoteaddr);
    bool isClosed() const;
    int getHandle() const;
public:
    static int init();
    static int uninit();
};

////////////////////////////////////////////////////////////////////////////////

bool receiveMsg(std::stringstream &str, Socket &socket, int timeout);
bool sendMsg(std::stringstream &str, Socket &socket, NetMessageCode opcode);

inline void checkIsClosing();

void networkThread();
