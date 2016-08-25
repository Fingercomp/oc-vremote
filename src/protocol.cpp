#include <stdexcept>

#include "protocol.hpp"


std::string pack(const long data) {
    std::string result;
    result.push_back(static_cast<char>((data >> 24) & 0xff));
    result.push_back(static_cast<char>((data >> 16) & 0xff));
    result.push_back(static_cast<char>((data >> 8) & 0xff));
    result.push_back(static_cast<char>(data & 0xff));
    return result;
}

std::string pack(const uint24_t data) {
    std::string result;
    result.push_back(static_cast<char>((data >> 16) & 0xff));
    result.push_back(static_cast<char>((data >> 8) & 0xff));
    result.push_back(static_cast<char>(data & 0xff));
    return result;
}

std::string pack(const uint16_t data) {
    std::string result;
    result.push_back(static_cast<char>((data >> 8) & 0xff));
    result.push_back(static_cast<char>(data & 0xff));
    return result;
}

std::string pack(const uint8_t data) {
    std::string result;
    result.push_back(static_cast<char>(data & 0xff));
    return result;
}

std::string pack(const std::string &data) {
    return pack(static_cast<uint24_t>(data.size())) + data;
}

std::string pack(const NetMessageCode data) {
    std::string result;
    result.push_back(static_cast<char>(data & 0xff));
    return result;
}

std::string pack(const ConnectionMode data) {
    return pack(static_cast<uint8_t>(data));
}

std::string pack(const AuthResult data) {
    return pack(static_cast<uint8_t>(data));
}

std::string pack(const Resolution &data) {
    return pack(data.w) + pack(data.h);
}

std::string pack(const bool data) {
    return data ? std::string("\xff") : std::string("\x00");
}

std::string pack(const WideChar data) {
    return pack(static_cast<uint8_t>(data.c.size())) + data.c;
}

std::string pack(const Char data) {
    return pack(data.c) + pack(data.fg) + pack(data.bg);
}

std::string pack(const Color &color) {
    return pack(color.rgb());
}

std::string pack(const Palette &data) {
    std::string result;
    for (int i = 0; i < 16; ++i) {
        result.append(pack(data[i]));
    }
    return result;
}

std::string pack(const uint64_t data) {
    std::string result;
    result.push_back(static_cast<char>((data >> 56) & 0xff));
    result.push_back(static_cast<char>((data >> 48) & 0xff));
    result.push_back(static_cast<char>((data >> 40) & 0xff));
    result.push_back(static_cast<char>((data >> 32) & 0xff));
    result.push_back(static_cast<char>((data >> 24) & 0xff));
    result.push_back(static_cast<char>((data >> 16) & 0xff));
    result.push_back(static_cast<char>((data >> 8) & 0xff));
    result.push_back(static_cast<char>(data & 0xff));
    return result;
}


std::string pack(const nmsg::NetMessageError &data) {
    return pack(data.description);
}

std::string pack(const nmsg::NetMessageAuthClient &data) {
    return pack(data.user) + pack(data.password) +
           pack(data.connectionMode) + pack(data.pingInterval);
}

std::string pack(const nmsg::NetMessageAuthServer &data) {
    return pack(data.result) + pack(data.displayString);
}

std::string pack(const nmsg::NetMessageInitialData &data) {
    return pack(data.palette) + pack(data.fg) +
           pack(data.bg) + pack(data.resolution) + pack(data.screenState) +
           pack(data.preciseMode) + pack(data.chars);
}

std::string pack(const nmsg::NetMessageSetBG &data) {
    return pack(data.index);
}

std::string pack(const nmsg::NetMessageSetFG &data) {
    return pack(data.index);
}

std::string pack(const nmsg::NetMessageSetPalette &data) {
    return pack(data.color) + pack(data.index);
}

std::string pack(const nmsg::NetMessageSetResolution &data) {
    return pack(data.w) + pack(data.h);
}

std::string pack(const nmsg::NetMessageSetChars &data) {
    return pack(data.x) + pack(data.y) + pack(data.chars) +
           pack(data.vertical);
}

std::string pack(const nmsg::NetMessageCopy &data) {
    return pack(data.x) + pack(data.y) + pack(data.w) +
           pack(data.h) + pack(data.tx) + pack(data.ty);
}

std::string pack(const nmsg::NetMessageFill &data) {
    return pack(data.x) + pack(data.y) + pack(data.w) +
           pack(data.h) + pack(data.c);
}

std::string pack(const nmsg::NetMessageTurnOnOff &data) {
    return pack(data.on);
}

std::string pack(const nmsg::NetMessageSetPrecise &data) {
    return pack(data.precise);
}

std::string pack(const nmsg::NetMessageFetch &data) {
    return pack(data.code);
}

std::string pack(const nmsg::NetMessageEventTouch &data) {
    return pack(data.x) + pack(data.y) + pack(data.button);
}

std::string pack(const nmsg::NetMessageEventDrag &data) {
    return pack(data.x) + pack(data.y) + pack(data.button);
}

std::string pack(const nmsg::NetMessageEventDrop &data) {
    return pack(data.x) + pack(data.y) + pack(data.button);
}

std::string pack(const nmsg::NetMessageEventScroll &data) {
    return pack(data.x) + pack(data.y) + pack(data.direction);
}

std::string pack(const nmsg::NetMessageEventKeyDown &data) {
    return pack(data.chr) + pack(data.cod);
}

std::string pack(const nmsg::NetMessageEventKeyUp &data) {
    return pack(data.chr) + pack(data.cod);
}

std::string pack(const nmsg::NetMessageEventClipboard &data) {
    return pack(data.data);
}

std::string pack(const nmsg::NetMessagePing &data) {
    return pack(data.ping);
}

std::string pack(const nmsg::NetMessagePong &data) {
    return pack(data.pong);
}


void unpack(std::stringstream &str, long &result) {
    result = 0;
    for (int i = 0; i < 4; ++i) {
        result <<= 8;
        result |= static_cast<uint8_t>(str.get());
    }
}

void unpack(std::stringstream &str, uint24_t &result) {
    result = 0;
    for (int i = 0; i < 3; ++i) {
        result <<= 8;
        result |= static_cast<uint8_t>(str.get());
    }
}

void unpack(std::stringstream &str, uint16_t &result) {
    result = 0;
    for (int i = 0; i < 2; ++i) {
        result <<= 8;
        result |= static_cast<uint8_t>(str.get());
    }
}

void unpack(std::stringstream &str, uint8_t &result) {
    result = static_cast<uint8_t>(str.get());
}

void unpack(std::stringstream &str, std::string &result) {
    result = "";
    uint24_t len;
    unpack(str, len);
    for (uint24_t i = 0; i < len; ++i) {
        result.push_back(str.get());
    }
}

void unpack(std::stringstream &str, NetMessageCode &result) {
    result = static_cast<NetMessageCode>(str.get());
}

void unpack(std::stringstream &str, ConnectionMode &result) {
    result = static_cast<ConnectionMode>(str.get());
}

void unpack(std::stringstream &str, AuthResult &result) {
    result = static_cast<AuthResult>(str.get());
}

void unpack(std::stringstream &str, Resolution &result) {
    unpack(str, result.w);
    unpack(str, result.h);
}

void unpack(std::stringstream &str, bool &result) {
    switch (static_cast<uint8_t>(str.get())) {
        case 0x00:
            result = false;
            break;
        case 0xff:
            result = true;
            break;
        default:
            throw std::runtime_error("Bad value for boolean");
    }
}

void unpack(std::stringstream &str, WideChar &result) {
    uint8_t len;
    unpack(str, len);
    for (int i = 0; i < len; ++i) {
        result.c.push_back(str.get());
    }
}

void unpack(std::stringstream &str, Char &result) {
    unpack(str, result.c);
    unpack(str, result.fg);
    unpack(str, result.bg);
}

void unpack(std::stringstream &str, Color &result) {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    unpack(str, r);
    unpack(str, g);
    unpack(str, b);
    result.r(r);
    result.g(g);
    result.b(b);
}

void unpack(std::stringstream &str, Palette &result) {
    for (int i = 0; i < 16; ++i) {
        Color c;
        unpack(str, c);
        result[i] = c;
    }
}

void unpack(std::stringstream &str, uint64_t &result) {
    result = 0;
    for (int i = 0; i < 8; ++i) {
        result <<= 8;
        result |= static_cast<uint8_t>(str.get());
    }
}


void unpack(std::stringstream &str, nmsg::NetMessageError &result) {
    unpack(str, result.description);
}

void unpack(std::stringstream &str, nmsg::NetMessageAuthClient &result) {
    unpack(str, result.user);
    unpack(str, result.password);
    unpack(str, result.connectionMode);
    unpack(str, result.pingInterval);
}

void unpack(std::stringstream &str, nmsg::NetMessageAuthServer &result) {
    unpack(str, result.result);
    unpack(str, result.displayString);
}

void unpack(std::stringstream &str, nmsg::NetMessageInitialData &result) {
    unpack(str, result.palette);
    unpack(str, result.fg);
    unpack(str, result.bg);
    unpack(str, result.resolution);
    unpack(str, result.screenState);
    unpack(str, result.preciseMode);
    unpack(str, result.chars);
}

void unpack(std::stringstream &str, nmsg::NetMessageSetBG &result) {
    unpack(str, result.index);
}

void unpack(std::stringstream &str, nmsg::NetMessageSetFG &result) {
    unpack(str, result.index);
}

void unpack(std::stringstream &str, nmsg::NetMessageSetPalette &result) {
    unpack(str, result.color);
    unpack(str, result.index);
}

void unpack(std::stringstream &str, nmsg::NetMessageSetResolution &result) {
    unpack(str, result.w);
    unpack(str, result.h);
}

void unpack(std::stringstream &str, nmsg::NetMessageSetChars &result) {
    unpack(str, result.x);
    unpack(str, result.y);
    unpack(str, result.chars);
    unpack(str, result.vertical);
}

void unpack(std::stringstream &str, nmsg::NetMessageCopy &result) {
    unpack(str, result.x);
    unpack(str, result.y);
    unpack(str, result.w);
    unpack(str, result.h);
    unpack(str, result.tx);
    unpack(str, result.ty);
}

void unpack(std::stringstream &str, nmsg::NetMessageFill &result) {
    unpack(str, result.x);
    unpack(str, result.y);
    unpack(str, result.w);
    unpack(str, result.h);
    unpack(str, result.c);
}

void unpack(std::stringstream &str, nmsg::NetMessageTurnOnOff &result) {
    unpack(str, result.on);
}

void unpack(std::stringstream &str, nmsg::NetMessageSetPrecise &result) {
    unpack(str, result.precise);
}

void unpack(std::stringstream, nmsg::NetMessageFetch) {}

void unpack(std::stringstream &str, nmsg::NetMessageEventTouch &result) {
    unpack(str, result.x);
    unpack(str, result.y);
    unpack(str, result.button);
}

void unpack(std::stringstream &str, nmsg::NetMessageEventDrag &result) {
    unpack(str, result.x);
    unpack(str, result.y);
    unpack(str, result.button);
}

void unpack(std::stringstream &str, nmsg::NetMessageEventDrop &result) {
    unpack(str, result.x);
    unpack(str, result.y);
    unpack(str, result.button);
}

void unpack(std::stringstream &str, nmsg::NetMessageEventScroll &result) {
    unpack(str, result.x);
    unpack(str, result.y);
    unpack(str, result.direction);
}

void unpack(std::stringstream &str, nmsg::NetMessageEventKeyDown &result) {
    unpack(str, result.chr);
    unpack(str, result.cod);
}

void unpack(std::stringstream &str, nmsg::NetMessageEventKeyUp &result) {
    unpack(str, result.chr);
    unpack(str, result.cod);
}

void unpack(std::stringstream &str, nmsg::NetMessageEventClipboard &result) {
    unpack(str, result.data);
}

void unpack(std::stringstream &str, nmsg::NetMessagePing &result) {
    unpack(str, result.ping);
}

void unpack(std::stringstream &str, nmsg::NetMessagePong &result) {
    unpack(str, result.pong);
}
