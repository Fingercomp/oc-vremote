#pragma once

#include <chrono>
#include <sstream>
#include <stdexcept>
#include <thread>

#include <SFML/Network.hpp>

#include "main.hpp"
#include "protocol.hpp"


class closing: public std::exception {
    const char _data[21] = "Closing the program";
public:
    virtual const char* what() const noexcept;
};


inline void checkIsClosing();

void networkThread();
