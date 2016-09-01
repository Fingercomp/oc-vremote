#include "runtime.hpp"


namespace rtStgs {
    State state = State::INITIALIZING;
    namespace msgQueue {
        std::queue<std::shared_ptr<NetMessage>> in;
        std::queue<std::shared_ptr<NetMessage>> out;
    }
    std::string port = "44444";
    std::string user;
    std::string password;
    ConnectionMode connectionMode;
    uint16_t pingInterval = 30;
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
