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
    return pack(data.code) + pack(data.description);
}

std::string pack(const nmsg::NetMessageAuthClient &data) {
    return pack(data.code) + pack(data.user) + pack(data.password) +
           pack(data.connectionMode) + pack(data.pingInterval);
}

std::string pack(const nmsg::NetMessageAuthServer &data) {
    return pack(data.code) + pack(data.result) + pack(data.displayString);
}

std::string pack(const nmsg::NetMessageInitialData &data) {
    return pack(data.code) + pack(data.palette) + pack(data.fg) +
           pack(data.bg) + pack(data.resolution) + pack(data.screenState) +
           pack(data.preciseMode) + pack(data.chars);
}

std::string pack(const nmsg::NetMessageSetBG &data) {
    return pack(data.code) + pack(data.index);
}

std::string pack(const nmsg::NetMessageSetFG &data) {
    return pack(data.code) + pack(data.index);
}

std::string pack(const nmsg::NetMessageSetPalette &data) {
    return pack(data.code) + pack(data.color) + pack(data.index);
}

std::string pack(const nmsg::NetMessageSetResolution &data) {
    return pack(data.code) + pack(data.w) + pack(data.h);
}

std::string pack(const nmsg::NetMessageSetChars &data) {
    return pack(data.code) + pack(data.x) + pack(data.y) + pack(data.chars) +
           pack(data.vertical);
}

std::string pack(const nmsg::NetMessageCopy &data) {
    return pack(data.code) + pack(data.x) + pack(data.y) + pack(data.w) +
           pack(data.h) + pack(data.tx) + pack(data.ty);
}

std::string pack(const nmsg::NetMessageFill &data) {
    return pack(data.code) + pack(data.x) + pack(data.y) + pack(data.w) +
           pack(data.h) + pack(data.c);
}

std::string pack(const nmsg::NetMessageTurnOnOff &data) {
    return pack(data.code) + pack(data.on);
}

std::string pack(const nmsg::NetMessageSetPrecise &data) {
    return pack(data.code) + pack(data.precise);
}

std::string pack(const nmsg::NetMessageFetch &data) {
    return pack(data.code);
}

std::string pack(const nmsg::NetMessageEventTouch &data) {
    return pack(data.code) + pack(data.x) + pack(data.y) + pack(data.button);
}

std::string pack(const nmsg::NetMessageEventDrag &data) {
    return pack(data.code) + pack(data.x) + pack(data.y) + pack(data.button);
}

std::string pack(const nmsg::NetMessageEventDrop &data) {
    return pack(data.code) + pack(data.x) + pack(data.y) + pack(data.button);
}

std::string pack(const nmsg::NetMessageEventScroll &data) {
    return pack(data.code) + pack(data.x) + pack(data.y) + pack(data.direction);
}

std::string pack(const nmsg::NetMessageEventKeyDown &data) {
    return pack(data.code) + pack(data.chr) + pack(data.cod);
}

std::string pack(const nmsg::NetMessageEventKeyUp &data) {
    return pack(data.code) + pack(data.chr) + pack(data.cod);
}

std::string pack(const nmsg::NetMessageEventClipboard &data) {
    return pack(data.code) + pack(data.data);
}

std::string pack(const nmsg::NetMessagePing &data) {
    return pack(data.code) + pack(data.ping);
}

std::string pack(const nmsg::NetMessagePong &data) {
    return pack(data.code) + pack(data.pong);
}
