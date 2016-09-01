#pragma once

#include <memory>
#include <queue>
#include <string>

#include <SFML/System.hpp>

#include "main.hpp"
#include "graphics.hpp"
#include "protocol.hpp"


class Charmap;

// The settings changed at runtime
namespace rtStgs {
    extern State state;
    namespace msgQueue {
        extern std::queue<std::shared_ptr<NetMessage>> in;
        extern std::queue<std::shared_ptr<NetMessage>> out;
    }
    extern std::string port;
    extern std::string user;
    extern std::string password;
    extern ConnectionMode connectionMode;
    namespace ping {
        extern uint16_t interval;
        extern bool sent;
        extern uint64_t challenge;
        namespace clock {
            extern sf::Clock clock;
            extern sf::Clock timeout;
        }
    }
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
