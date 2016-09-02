#include "runtime.hpp"


namespace rtStgs {
    State state = State::INITIALIZING;
    namespace msgQueue {
        std::queue<std::unique_ptr<NetMessage>> in;
        std::queue<std::unique_ptr<NetMessage>> out;
    }
    // This port was choosen at random.
    // https://i.imgur.com/WiC1Rhi.png
    std::string port = "39139";
    std::string user;
    std::string password;
    ConnectionMode connectionMode;
    namespace ping {
        uint16_t interval = 30;
        bool sent = false;
        uint64_t challenge = 0;
        namespace clock {
            sf::Clock clock;
            sf::Clock timeout;
        }
    }
    namespace render {
        uint8_t fg = 255;
        uint8_t bg = 16;
        Palette palette;
        Resolution resolution {160, 50};
        bool screenState = true;
        bool preciseMode = false;
        Charmap chars(160, 50);
    }
}
