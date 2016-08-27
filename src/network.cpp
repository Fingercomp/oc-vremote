#include "network.hpp"


const char* closing::what() const noexcept {
    return _data;
}

inline void checkIsClosing() {
    if (rtStgs::state == State::CLOSING) {
        throw closing();
    }
}

bool receive(std::stringstream &str, sf::TcpSocket &socket) {
    char *data = new char[5];
    std::size_t received;
    sf::Socket::Status s = socket.receive(data, 4, received);
    if (received != 4 || s != sf::Socket::Done) {
        delete[] data;
        return false;
    }
    data[4] = '\0';
    std::stringstream strCopy;
    str << data;
    strCopy << data;
    delete[] data;
    NetMessageCode opcode;
    uint24_t len;
    unpack(strCopy, opcode);
    unpack(strCopy, len);
    std::size_t receivedTotal = 0;
    while (receivedTotal < len) {
        std::size_t recLen = len - receivedTotal;
        data = new char[recLen + 1];
        sf::Socket::Status s = socket.receive(data, recLen, received);
        if (s != sf::Socket::Partial || s != sf::Socket::Done) {
            delete[] data;
            return false;
        }
        receivedTotal += received;
        data[recLen] = '\0';
        str << data;
        delete[] data;
    }
    return true;
}

bool send(std::stringstream &str, sf::TcpSocket &socket, NetMessageCode opcode) {
    std::string sendString;
    while (!str.eof()) {
        char c = str.get();
        sendString.push_back(c);
    }
    std::stringstream sendStr;
    pack(sendStr, opcode);
    pack(sendStr, static_cast<uint24_t>(sendString.size()));
    sendString = sendStr.str() + sendString;
    const char *data = sendString.c_str();
    std::size_t sentTotal = 0;
    while (sentTotal != sendString.size()) {
        std::size_t sent;
        sf::Socket::Status s = socket.send(data + sentTotal, sendString.size() - sentTotal, sent);
        switch (s) {
            case sf::Socket::Done:
            return true;
            case sf::Socket::Partial:
                sentTotal += sent;
                break;
            default:
                return false;
        }
    }
    return false;
}


void networkThread() {
    try {
        while (1) {
            sf::TcpListener listener;
            listener.setBlocking(false);

            while (listener.listen(rtStgs::port) != sf::Socket::Done) {
                checkIsClosing();
            }

            rtStgs::state = State::WAITING_FOR_CONNECTION;

            sf::TcpSocket socket;
            while (rtStgs::state == State::WAITING_FOR_CONNECTION) {
                checkIsClosing();
                if (listener.accept(socket) == sf::Socket::Done) {
                    rtStgs::state = State::CONNECTION_ATTEMPT;
                    break;
                }
            }

            while (rtStgs::state != State::AUTHORIZATION) {
                checkIsClosing();
            }

            socket.setBlocking(false);

            std::stringstream strIn;
            std::stringstream strOut;
            bool authed = false;
            while (rtStgs::state == State::AUTHORIZATION) {
                checkIsClosing();
                strIn.str(std::string(""));
                if (receive(strIn, socket)) {
                    NetMessageCode opcode;
                    uint24_t len;
                    unpack(strIn, opcode);
                    unpack(strIn, len);
                    if (!authed) {
                        if (opcode == MSG_AUTH_CLIENT) {
                            nmsg::NetMessageAuthClient msg;
                            unpack(strIn, msg);
                            nmsg::NetMessageAuthServer resp;
                            if (msg.connectionMode != ConnectionMode::GpuKbd) {
                                resp.result = AuthResult::UnsupportedMode;
                                resp.displayString = "Unsupported connection mode";
                            } else {
                                if (msg.pingInterval >= 30 && msg.pingInterval < 120) {
                                    rtStgs::pingInterval = msg.pingInterval;
                                }
                                rtStgs::user = msg.user;
                                rtStgs::password = msg.password;
                                rtStgs::connectionMode = msg.connectionMode;
                                resp.result = AuthResult::Authenticated;
                                resp.displayString = "Hello :)";
                            }
                            pack(strOut, resp);
                            send(strOut, socket, MSG_AUTH_SERVER);
                            authed = true;
                        }
                    } else {
                        if (opcode == MSG_INITIAL_DATA) {
                            nmsg::NetMessageInitialData msg;
                            unpack(strIn, msg);
                            rtStgs::render::palette = msg.palette;
                            rtStgs::render::fg = msg.fg;
                            rtStgs::render::bg = msg.bg;
                            rtStgs::render::resolution = msg.resolution;
                            rtStgs::render::screenState = msg.screenState;
                            rtStgs::render::preciseMode = msg.preciseMode;
                            rtStgs::render::chars = Charmap(msg.resolution.w, msg.resolution.h);
                            for (std::size_t i = 0; i < msg.chars.size(); ++i) {
                                int x = i % msg.resolution.w;
                                int y = i / msg.resolution.h;
                                rtStgs::render::chars.get(x, y) = msg.chars.at(i);
                            }
                            rtStgs::state = State::CONNECTED;
                        }
                    }
                }
            }


            while (1) {
                checkIsClosing();
                strIn.str(std::string(""));
                if (receive(strIn, socket)) {
                    NetMessageCode opcode;
                    uint24_t len;
                    unpack(strIn, opcode);
                    unpack(strIn, len);
                    switch (opcode) {
                        case MSG_ERROR: {
                            nmsg::NetMessageError msg;
                            unpack(strIn, msg);
                            rtStgs::state = State::WAITING_FOR_CONNECTION;
                            break;
                        }
                        case MSG_AUTH_CLIENT: {
                            nmsg::NetMessageAuthClient msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_AUTH_SERVER: {
                            nmsg::NetMessageAuthServer msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_INITIAL_DATA: {
                            nmsg::NetMessageInitialData msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_SET_BG: {
                            nmsg::NetMessageSetBG msg;
                            unpack(strIn, msg);
                            rtStgs::render::bg = msg.index;
                            break;
                        }
                        case MSG_SET_FG: {
                            nmsg::NetMessageSetFG msg;
                            unpack(strIn, msg);
                            rtStgs::render::fg = msg.index;
                            break;
                        }
                        case MSG_SET_PALETTE: {
                            nmsg::NetMessageSetPalette msg;
                            unpack(strIn, msg);
                            rtStgs::render::palette[msg.index] = Color(msg.color);
                            break;
                        }
                        case MSG_SET_RESOLUTION: {
                            nmsg::NetMessageSetResolution msg;
                            unpack(strIn, msg);
                            rtStgs::render::resolution.w = msg.w;
                            rtStgs::render::resolution.h = msg.h;
                            break;
                        }
                        case MSG_SET_CHARS: {
                            nmsg::NetMessageSetChars msg;
                            unpack(strIn, msg);
                            rtStgs::msgQueue::in.push(&msg);
                            break;
                        }
                        case MSG_COPY: {
                            nmsg::NetMessageCopy msg;
                            unpack(strIn, msg);
                            rtStgs::msgQueue::in.push(&msg);
                            break;
                        }
                        case MSG_FILL: {
                            nmsg::NetMessageFill msg;
                            unpack(strIn, msg);
                            rtStgs::msgQueue::in.push(&msg);
                            break;
                        }
                        case MSG_TURN_ON_OFF: {
                            nmsg::NetMessageTurnOnOff msg;
                            unpack(strIn, msg);
                            rtStgs::render::screenState = msg.on;
                            break;
                        }
                        case MSG_SET_PRECISE: {
                            nmsg::NetMessageSetPrecise msg;
                            unpack(strIn, msg);
                            rtStgs::render::preciseMode = msg.precise;
                            break;
                        }
                        case MSG_FETCH: {
                            nmsg::NetMessageFetch msg;
                            unpack(strIn, msg);
                            nmsg::NetMessageInitialData resp;
                            resp.palette = rtStgs::render::palette;
                            resp.fg = rtStgs::render::fg;
                            resp.bg = rtStgs::render::bg;
                            resp.resolution = rtStgs::render::resolution;
                            resp.screenState = rtStgs::render::screenState;
                            resp.preciseMode = rtStgs::render::preciseMode;
                            for (int y = 0; y < resp.resolution.h; ++y) {
                                for (int x = 0; x < resp.resolution.w; ++x) {
                                    Char &c = rtStgs::render::chars.get(x, y);
                                    resp.chars.push_back(c);
                                }
                            }
                            std::stringstream strOut;
                            pack(strOut, resp);
                            send(strOut, socket, MSG_INITIAL_DATA);
                            break;
                        }
                        case MSG_EVENT_TOUCH: {
                            nmsg::NetMessageEventTouch msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_EVENT_DRAG: {
                            nmsg::NetMessageEventDrag msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_EVENT_DROP: {
                            nmsg::NetMessageEventDrop msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_EVENT_SCROLL: {
                            nmsg::NetMessageEventScroll msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_EVENT_KEY_DOWN: {
                            nmsg::NetMessageEventKeyDown msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_EVENT_KEY_UP: {
                            nmsg::NetMessageEventKeyUp msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_EVENT_CLIPBOARD: {
                            nmsg::NetMessageEventClipboard msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_PING: {
                            nmsg::NetMessagePing msg;
                            unpack(strIn, msg);
                            break;
                        }
                        case MSG_PONG: {
                            nmsg::NetMessagePong msg;
                            unpack(strIn, msg);
                            // TODO: ping the client
                            break;
                        }
                    }
                }
                // Send messages in the queue
                while (!rtStgs::msgQueue::out.empty()) {
                    strOut.str(std::string(""));
                    NetMessage *baseMsg = rtStgs::msgQueue::out.front();
                    switch (baseMsg->code) {
                        case MSG_AUTH_CLIENT:
                        case MSG_AUTH_SERVER:
                        case MSG_INITIAL_DATA:
                        case MSG_SET_BG:
                        case MSG_SET_FG:
                        case MSG_SET_PALETTE:
                        case MSG_SET_RESOLUTION:
                        case MSG_SET_CHARS:
                        case MSG_COPY:
                        case MSG_FILL:
                        case MSG_TURN_ON_OFF:
                        case MSG_SET_PRECISE:
                        case MSG_FETCH:
                        case MSG_PING:
                        case MSG_PONG:
                            break;
                        case MSG_ERROR: {
                            nmsg::NetMessageError *msg = dynamic_cast<nmsg::NetMessageError *>(baseMsg);
                            pack(strOut, *msg);
                            send(strOut, socket, baseMsg->code);
                            break;
                        }
                        case MSG_EVENT_TOUCH: {
                            nmsg::NetMessageEventTouch *msg = dynamic_cast<nmsg::NetMessageEventTouch *>(baseMsg);
                            pack(strOut, msg);
                            send(strOut, socket, baseMsg->code);
                            break;
                        }
                        case MSG_EVENT_DRAG: {
                            nmsg::NetMessageEventDrag *msg = dynamic_cast<nmsg::NetMessageEventDrag *>(baseMsg);
                            pack(strOut, msg);
                            send(strOut, socket, baseMsg->code);
                            break;
                        }
                        case MSG_EVENT_DROP: {
                            nmsg::NetMessageEventDrop *msg = dynamic_cast<nmsg::NetMessageEventDrop *>(baseMsg);
                            pack(strOut, msg);
                            send(strOut, socket, baseMsg->code);
                            break;
                        }
                        case MSG_EVENT_SCROLL: {
                            nmsg::NetMessageEventScroll *msg = dynamic_cast<nmsg::NetMessageEventScroll *>(baseMsg);
                            pack(strOut, msg);
                            send(strOut, socket, baseMsg->code);
                            break;
                        }
                        case MSG_EVENT_KEY_DOWN: {
                            nmsg::NetMessageEventKeyDown *msg = dynamic_cast<nmsg::NetMessageEventKeyDown *>(baseMsg);
                            pack(strOut, msg);
                            send(strOut, socket, baseMsg->code);
                            break;
                        }
                        case MSG_EVENT_KEY_UP: {
                            nmsg::NetMessageEventKeyUp *msg = dynamic_cast<nmsg::NetMessageEventKeyUp *>(baseMsg);
                            pack(strOut, msg);
                            send(strOut, socket, baseMsg->code);
                            break;
                        }
                        case MSG_EVENT_CLIPBOARD: {
                            nmsg::NetMessageEventClipboard *msg = dynamic_cast<nmsg::NetMessageEventClipboard *>(baseMsg);
                            pack(strOut, msg);
                            send(strOut, socket, baseMsg->code);
                            break;
                        }
                    }
                    rtStgs::msgQueue::out.pop();
                }
            }
        }
    } catch (closing &e) {
        // pass
    }
}
