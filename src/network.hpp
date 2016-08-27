#pragma once

#include <chrono>
#include <sstream>
#include <stdexcept>
#include <thread>

#include <SFML/Network.hpp>

#include "main.hpp"
#include "protocol.hpp"
#include "runtime.hpp"


class closing: public std::exception {
    const char _data[21] = "Closing the program";
public:
    virtual const char* what() const noexcept;
};


bool receive(std::stringstream &str, sf::TcpSocket &socket);
bool send(std::stringstream &str, sf::TcpSocket &socket, NetMessageCode opcode);

inline void checkIsClosing();

void networkThread();
