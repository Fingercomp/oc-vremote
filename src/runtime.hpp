#pragma once

#include <string>
#include <queue>

#include "main.hpp"
#include "graphics.hpp"
#include "protocol.hpp"


class Charmap;

// The settings changed at runtime
namespace rtStgs {
    extern State state;
    namespace msgQueue {
        extern std::queue<NetMessage *> in;
        extern std::queue<NetMessage *> out;
    }
    extern int port;
    extern std::string user;
    extern std::string password;
    extern ConnectionMode connectionMode;
    extern uint16_t pingInterval;
    namespace render {
        extern uint8_t fg;
        extern uint8_t bg;
        extern Palette palette;
        extern Resolution resolution;
        extern bool screenState;
        extern bool preciseMode;
        extern Charmap chars;
    }
}