#include <stdexcept>

#include "protocol.hpp"


std::stringstream& pack(std::stringstream &result, const long data) {
    result.put(static_cast<char>((data >> 24) & 0xff));
    result.put(static_cast<char>((data >> 16) & 0xff));
    result.put(static_cast<char>((data >> 8) & 0xff));
    result.put(static_cast<char>(data & 0xff));
    return result;
}

std::stringstream& pack(std::stringstream &result, const uint24_t data) {
    result.put(static_cast<char>((data >> 16) & 0xff));
    result.put(static_cast<char>((data >> 8) & 0xff));
    result.put(static_cast<char>(data & 0xff));
    return result;
}

std::stringstream& pack(std::stringstream &result, const uint16_t data) {
    result.put(static_cast<char>((data >> 8) & 0xff));
    result.put(static_cast<char>(data & 0xff));
    return result;
}

std::stringstream& pack(std::stringstream &result, const uint8_t data) {
    result.put(static_cast<char>(data & 0xff));
    return result;
}

std::stringstream& pack(std::stringstream &result, const std::string &data) {
    pack(result, static_cast<uint24_t>(data.size()));
    for (int i = 0; i < static_cast<int>(data.size()); ++i) {
        result.put(data.at(i));
    }
    return result;
}

std::stringstream& pack(std::stringstream &result, const NetMessageCode data) {
    result.put(static_cast<char>(data & 0xff));
    return result;
}

std::stringstream& pack(std::stringstream &result, const ConnectionMode data) {
    pack(result, static_cast<uint8_t>(data));
    return result;
}

std::stringstream& pack(std::stringstream &result, const AuthResult data) {
    pack(result, static_cast<uint8_t>(data));
    return result;
}

std::stringstream& pack(std::stringstream &result, const Resolution &data) {
    pack(result, data.w);
    pack(result, data.h);
    return result;
}

std::stringstream& pack(std::stringstream &result, const bool data) {
    if (data) {
        result.put('\xff');
    } else {
        result.put('\x00');
    }
    return result;
}

std::stringstream& pack(std::stringstream &result, const Char &data) {
    pack(result, data.c);
    pack(result, data.fg);
    pack(result, data.bg);
    return result;
}

std::stringstream& pack(std::stringstream &result, const Color &color) {
    pack(result, color.rgb());
    return result;
}

std::stringstream& pack(std::stringstream &result, const Palette &data) {
    for (int i = 0; i < 16; ++i) {
        pack(result, data[i]);
    }
    return result;
}

std::stringstream& pack(std::stringstream &result, const uint64_t data) {
    result.put(static_cast<char>((data >> 56) & 0xff));
    result.put(static_cast<char>((data >> 48) & 0xff));
    result.put(static_cast<char>((data >> 40) & 0xff));
    result.put(static_cast<char>((data >> 32) & 0xff));
    result.put(static_cast<char>((data >> 24) & 0xff));
    result.put(static_cast<char>((data >> 16) & 0xff));
    result.put(static_cast<char>((data >> 8) & 0xff));
    result.put(static_cast<char>(data & 0xff));
    return result;
}


std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageError &data) {
    pack(result, data.description);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageAuthClient &data) {
    pack(result, data.user);
    pack(result, data.password);
    pack(result, data.connectionMode);
    pack(result, data.pingInterval);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageAuthServer &data) {
    pack(result, data.result);
    pack(result, data.displayString);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageInitialData &data) {
    pack(result, data.palette);
    pack(result, data.fg);
    pack(result, data.bg);
    pack(result, data.resolution);
    pack(result, data.screenState);
    pack(result, data.preciseMode);
    pack(result, data.chars);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetBG &data) {
    pack(result, data.index);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetFG &data) {
    pack(result, data.index);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetPalette &data) {
    pack(result, data.color);
    pack(result, data.index);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetResolution &data) {
    pack(result, data.w);
    pack(result, data.h);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetChars &data) {
    pack(result, data.x);
    pack(result, data.y);
    pack(result, data.chars);
    pack(result, data.vertical);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageCopy &data) {
    pack(result, data.x);
    pack(result, data.y);
    pack(result, data.w);
    pack(result, data.h);
    pack(result, data.tx);
    pack(result, data.ty);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageFill &data) {
    pack(result, data.x);
    pack(result, data.y);
    pack(result, data.w);
    pack(result, data.h);
    pack(result, data.c);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageTurnOnOff &data) {
    pack(result, data.on);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageSetPrecise &data) {
    pack(result, data.precise);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageFetch &) {
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventTouch &data) {
    pack(result, data.x);
    pack(result, data.y);
    pack(result, data.button);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventDrag &data) {
    pack(result, data.x);
    pack(result, data.y);
    pack(result, data.button);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventDrop &data) {
    pack(result, data.x);
    pack(result, data.y);
    pack(result, data.button);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventScroll &data) {
    pack(result, data.x);
    pack(result, data.y);
    pack(result, data.direction);
    pack(result, data.delta);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventKeyDown &data) {
    pack(result, data.chr);
    pack(result, data.cod);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventKeyUp &data) {
    pack(result, data.chr);
    pack(result, data.cod);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessageEventClipboard &data) {
    pack(result, data.data);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessagePing &data) {
    pack(result, data.ping);
    return result;
}

std::stringstream& pack(std::stringstream &result, const nmsg::NetMessagePong &data) {
    pack(result, data.pong);
    return result;
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

void unpack(std::stringstream &, nmsg::NetMessageFetch &) {}

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
    unpack(str, result.delta);
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
