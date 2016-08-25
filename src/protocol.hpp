#include <sstream>
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

std::stringstream& pack(std::stringstream &result, const long data);
std::stringstream& pack(std::stringstream &result, const uint24_t data);
std::stringstream& pack(std::stringstream &result, const uint16_t data);
std::stringstream& pack(std::stringstream &result, const uint8_t data);
std::stringstream& pack(std::stringstream &result, const std::string &data);
std::stringstream& pack(std::stringstream &result, const NetMessageCode data);
std::stringstream& pack(std::stringstream &result, const ConnectionMode data);
std::stringstream& pack(std::stringstream &result, const AuthResult data);
std::stringstream& pack(std::stringstream &result, const Resolution &data);
std::stringstream& pack(std::stringstream &result, const bool data);
std::stringstream& pack(std::stringstream &result, const WideChar data);
std::stringstream& pack(std::stringstream &result, const Char data);
std::stringstream& pack(std::stringstream &result, const Color &color);

template <typename T>
std::stringstream& pack(std::stringstream &result, const std::vector<T> &data) {
    pack(result, static_cast<uint24_t>(data.size()));
    for (auto i: data) {
        pack(result, i);
    }
    return result;
}

std::stringstream& pack(std::stringstream &result, const Palette &data);
std::stringstream& pack(std::stringstream &result, const uint64_t data);

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageError &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageAuthClient &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageAuthServer &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageInitialData &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetBG &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetFG &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetPalette &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetResolution &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetChars &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageCopy &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageFill &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageTurnOnOff &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetPrecise &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageFetch &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventTouch &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventDrag &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventDrop &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventScroll &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventKeyDown &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventKeyUp &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventClipboard &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessagePing &data);
std::stringstream& pack(std::stringstream &result, const nmsg::NetMessagePong &data);


void unpack(std::stringstream &str, long &result);
void unpack(std::stringstream &str, uint24_t &result);
void unpack(std::stringstream &str, uint16_t &result);
void unpack(std::stringstream &str, uint8_t &result);
void unpack(std::stringstream &str, std::string &result);
void unpack(std::stringstream &str, NetMessageCode &result);
void unpack(std::stringstream &str, ConnectionMode &result);
void unpack(std::stringstream &str, AuthResult &result);
void unpack(std::stringstream &str, Resolution &result);
void unpack(std::stringstream &str, bool &result);
void unpack(std::stringstream &str, WideChar &result);
void unpack(std::stringstream &str, Char &result);
void unpack(std::stringstream &str, Color &result);
void unpack(std::stringstream &str, Palette &result);
void unpack(std::stringstream &str, uint64_t &result);

template <typename T>
void unpack(std::stringstream &str, std::vector<T> &result) {
    uint24_t len;
    unpack(str, len);
    for (uint24_t i = 0; i < len; ++i) {
        T element;
        unpack(str, element);
        result.push_back(element);
    }
}


void unpack(std::stringstream &str, nmsg::NetMessageError &result);
void unpack(std::stringstream &str, nmsg::NetMessageAuthClient &result);
void unpack(std::stringstream &str, nmsg::NetMessageAuthServer &result);
void unpack(std::stringstream &str, nmsg::NetMessageInitialData &result);
