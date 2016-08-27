#pragma once

#include <queue>

#include "graphics.hpp"
#include "protocol.hpp"


enum class State {
    // Non-threaded environment; initializing the program
    INITIALIZING,
    // Threaded environment; listening for connections
    WAITING_FOR_CONNECTION,
    // Threaded environment; a client connected, waiting for user confirmation
    CONNECTION_ATTEMPT,
    // Threaded environment; a client connected, waiting for authorization and InitialData message
    AUTHORIZATION,
    // Threaded environment; a client connected and authorized, exchaning the data
    CONNECTED,
    // Threaded environment; signalling threads to end
    CLOSING
};

// The settings changed at runtime
namespace rtStgs {
    State state = State::INITIALIZING;
    namespace msgQueue {
        std::queue<NetMessage *> in;
        std::queue<NetMessage *> out;
    }
    int port;
    std::string user;
    std::string password;
    ConnectionMode connectionMode;
    uint16_t pingInterval = 30;
    namespace render {
        uint8_t fg;
        uint8_t bg;
        Palette palette;
        Resolution resolution;
        bool screenState;
        bool preciseMode;
        Charmap chars(1, 1);
    }
}
