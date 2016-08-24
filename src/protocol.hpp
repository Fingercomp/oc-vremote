#include <string>
#include <vector>

#include "util.hpp"


// We're using uint32_t as uint24_t:
// 32-bit integers has the long type
using uint24_t = uint32_t;

enum NetMessageCode {
    MSG_ERROR = 0,
    MSG_AUTH_CLIENT = 1,
    MSG_AUTH_SERVER = 2,
    MSG_INITIAL_DATA = 3,
    MSG_SET_BG = 4,
    MSG_SET_FG = 5,
    MSG_SET_PALETTE = 6,
    MSG_SET_RESOLUTION = 7,
    MSG_SET_CHARS = 8,
    MSG_COPY = 9,
    MSG_FILL = 10,
    MSG_TURN_ON_OFF = 11,
    MSG_SET_PRECISE = 12,
    MSG_FETCH = 13,
    MSG_EVENT_TOUCH = 14,
    MSG_EVENT_DRAG = 15,
    MSG_EVENT_DROP = 16,
    MSG_EVENT_SCROLL = 17,
    MSG_EVENT_KEY_DOWN = 18,
    MSG_EVENT_KEY_UP = 19,
    MSG_EVENT_CLIPBOARD = 20,
    MSG_PING = 21,
    MSG_PONG = 22
};

enum class ConnectionMode {
    GpuKbd = 0,
    Gpu = 1,
    Kbd = 2,
    Custom = 3
};

enum class AuthResult {
    Authenticated = 0,
    WrongCredentials = 1,
    UnsupportedMode = 2
};

struct Resolution {
    uint8_t w;
    uint8_t h;
};

struct WideChar {
    std::string c;
};

struct Char {
    WideChar c;
    uint8_t fg;
    uint8_t bg;
};


struct NetMessage {
    NetMessageCode code;
};


namespace nmsg {
    struct NetMessageError: public NetMessage {
        NetMessageCode code = MSG_ERROR;
        std::string description;
    };

    struct NetMessageAuthClient: public NetMessage {
        NetMessageCode code = MSG_AUTH_CLIENT;
        std::string user;
        std::string password;
        ConnectionMode connectionMode;
        uint16_t pingInterval;
    };

    struct NetMessageAuthServer: public NetMessage {
        NetMessageCode code = MSG_AUTH_SERVER;
        AuthResult result;
        std::string displayString;
    };

    struct NetMessageInitialData: public NetMessage {
        NetMessageCode code = MSG_INITIAL_DATA;
        Palette palette;
        uint8_t fg;
        uint8_t bg;
        Resolution resolution;
        bool screenState;
        bool preciseMode;
        std::vector<Char> chars;
    };

    struct NetMessageSetBG: public NetMessage {
        NetMessageCode code = MSG_SET_BG;
        uint8_t index;
    };

    struct NetMessageSetFG: public NetMessage {
        NetMessageCode code = MSG_SET_FG;
        uint8_t index;
    };

    struct NetMessageSetPalette: public NetMessage {
        NetMessageCode code = MSG_SET_PALETTE;
        Color color;
        uint8_t index;
    };

    struct NetMessageSetResolution: public NetMessage {
        NetMessageCode code = MSG_SET_RESOLUTION;
        uint8_t w;
        uint8_t h;
    };

    struct NetMessageSetChars: public NetMessage {
        NetMessageCode code = MSG_SET_CHARS;
        uint8_t x;
        uint8_t y;
        std::string chars;
        bool vertical;
    };

    struct NetMessageCopy: public NetMessage {
        NetMessageCode code = MSG_COPY;
        uint8_t x;
        uint8_t y;
        uint8_t w;
        uint8_t h;
        uint8_t tx;
        uint8_t ty;
    };

    struct NetMessageFill: public NetMessage {
        NetMessageCode code = MSG_FILL;
        uint8_t x;
        uint8_t y;
        uint8_t w;
        uint8_t h;
        WideChar c;
    };

    struct NetMessageTurnOnOff: public NetMessage {
        NetMessageCode code = MSG_TURN_ON_OFF;
        bool on;
    };

    struct NetMessageSetPrecise: public NetMessage {
        NetMessageCode code = MSG_SET_PRECISE;
        bool precise;
    };

    struct NetMessageFetch: public NetMessage {
        NetMessageCode code = MSG_FETCH;
    };

    struct NetMessageEventTouch: public NetMessage {
        NetMessageCode code = MSG_EVENT_TOUCH;
        uint8_t x;
        uint8_t y;
        uint8_t button;
    };

    struct NetMessageEventDrag: public NetMessage {
        NetMessageCode code = MSG_EVENT_DRAG;
        uint8_t x;
        uint8_t y;
        uint8_t button;
    };

    struct NetMessageEventDrop: public NetMessage {
        NetMessageCode code = MSG_EVENT_DROP;
        uint8_t x;
        uint8_t y;
        uint8_t button;
    };

    struct NetMessageEventScroll: public NetMessage {
        NetMessageCode code = MSG_EVENT_SCROLL;
        uint8_t x;
        uint8_t y;
        uint8_t direction;
    };

    struct NetMessageEventKeyDown: public NetMessage {
        NetMessageCode code = MSG_EVENT_KEY_DOWN;
        long chr;
        long cod;
    };

    struct NetMessageEventKeyUp: public NetMessage {
        NetMessageCode code = MSG_EVENT_KEY_UP;
        long chr;
        long cod;
    };

    struct NetMessageEventClipboard: public NetMessage {
        NetMessageCode code = MSG_EVENT_CLIPBOARD;
        std::string data;
    };

    struct NetMessagePing: public NetMessage {
        NetMessageCode code = MSG_PING;
        uint64_t ping;
    };

    struct NetMessagePong: public NetMessage {
        NetMessageCode code = MSG_PONG;
        uint64_t pong;
    };
}

std::string pack(const long data);
std::string pack(const uint24_t data);
std::string pack(const uint16_t data);
std::string pack(const uint8_t data);
std::string pack(const std::string &data);
std::string pack(const NetMessageCode data);
std::string pack(const ConnectionMode data);
std::string pack(const AuthResult data);
std::string pack(const Resolution &data);
std::string pack(const bool data);
std::string pack(const WideChar data);
std::string pack(const Char data);
std::string pack(const Color &color);
template <typename T>
std::string pack(const std::vector<T> &data) {
    std::string result;
    result.append(pack(static_cast<uint24_t>(data.size())));
    for (auto i: data) {
        result.append(pack(i));
    }
    return result;
}
std::string pack(const Palette &data);
std::string pack(const uint64_t data);

std::string pack(const nmsg::NetMessageError &data);
std::string pack(const nmsg::NetMessageAuthClient &data);
std::string pack(const nmsg::NetMessageAuthServer &data);
std::string pack(const nmsg::NetMessageInitialData &data);
std::string pack(const nmsg::NetMessageSetBG &data);
std::string pack(const nmsg::NetMessageSetFG &data);
std::string pack(const nmsg::NetMessageSetPalette &data);
std::string pack(const nmsg::NetMessageSetResolution &data);
std::string pack(const nmsg::NetMessageSetChars &data);
std::string pack(const nmsg::NetMessageCopy &data);
std::string pack(const nmsg::NetMessageFill &data);
std::string pack(const nmsg::NetMessageTurnOnOff &data);
std::string pack(const nmsg::NetMessageSetPrecise &data);
std::string pack(const nmsg::NetMessageFetch &data);
std::string pack(const nmsg::NetMessageEventTouch &data);
std::string pack(const nmsg::NetMessageEventDrag &data);
std::string pack(const nmsg::NetMessageEventDrop &data);
std::string pack(const nmsg::NetMessageEventScroll &data);
std::string pack(const nmsg::NetMessageEventKeyDown &data);
std::string pack(const nmsg::NetMessageEventKeyUp &data);
std::string pack(const nmsg::NetMessageEventClipboard &data);
std::string pack(const nmsg::NetMessagePing &data);
std::string pack(const nmsg::NetMessagePong &data);
