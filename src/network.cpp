#include "network.hpp"


closing::closing() {}

const char* closing::what() const noexcept {
    return _data;
}

error_occured::error_occured() {}

const char* error_occured::what() const noexcept {
    return _data;
}

bool Socket::initialized = false;

Socket::Socket(const std::string port) {
    try {
        Socket::init();
        addrinfo basicInfo, *servInfo, *res;
        memset(&basicInfo, 0, sizeof(basicInfo));
        basicInfo.ai_family = AF_UNSPEC;
        basicInfo.ai_socktype = SOCK_STREAM;
        basicInfo.ai_flags = AI_PASSIVE;

        int result;
        int yes = 1;
        if ((result = getaddrinfo(nullptr, port.c_str(), &basicInfo, &servInfo)) != 0) {
            std::cerr << "Could not get address info: " << gai_strerror(result) << "\n";
            freeaddrinfo(servInfo);
            throw error_occured();
        }

        for (res = servInfo; res != nullptr; res = res->ai_next) {
            if ((sockd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
                std::cerr << "Could not create a socket on address, trying next one...\n";
                continue;
            }

            if (setsockopt(sockd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
                std::cerr << "Could not set the socket options!\n";
                freeaddrinfo(servInfo);
                throw error_occured();
            }

            if (bind(sockd, res->ai_addr, res->ai_addrlen) == -1) {
                disconnect();
                std::cerr << "Could not bind the socket to the port\n";
                continue;
            }

            break;
        }
        freeaddrinfo(servInfo);

        if (res == nullptr) {
            std::cerr << "Could not find the address to bind to.\n";
            throw error_occured();
        }

        if (listen(sockd, 10) == -1) {
            std::cerr << "Could not set a listener on the port.\n";
            disconnect();
            throw error_occured();
        }
        closed = false;
    } catch (error_occured &e) {
        sockd = -1;
        closed = true;
    }
}

Socket::Socket(const SOCKET sockd): sockd(sockd) {
    Socket::init();
    closed = false;
    if (sockd < 0) {
        closed = true;
    }
}

Socket::~Socket() {
    if (sockd >= 0) {
        disconnect();
    }
}

int Socket::disconnect() {
    int status = 0;

#ifdef _WIN32
    status = shutdown(sockd, SF_BOTH);
    if (status == 0) {
        status = closesocket(sockd);
    }
#else
    status = shutdown(sockd, SHUT_RDWR);
    if (status == 0) {
        status = close(sockd);
    }
#endif
    closed = true;
    return status;
}

int Socket::send(const std::string &data) {
    std::size_t total = 0;
    int bytesleft = data.size();
    int n;

    while (total < data.size()) {
        n = ::send(sockd, data.c_str() + total, bytesleft, 0);
        if (n == -1) {
            return -1;
        }

        total += n;
        bytesleft -= n;
    }

    return total;
}

int Socket::recv(std::string &strData, const int size, const int timeout) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockd, &readfds);
    timeval to {timeout / 1000000, timeout % 1000000};
    if (select(FD_SETSIZE, &readfds, nullptr, nullptr, &to) == 1) {
        if (FD_ISSET(sockd, &readfds)) {
            std::unique_ptr<char> data(new char[size]);
            int got = 0;
            int bytesleft = size;
            int n;

            while (got < size) {
                n = ::recv(sockd, data.get() + got, bytesleft, 0);
                if (n == -1) {
                    return -1;
                } else if (n == 0) {
                    closed = true;
                    return 0;
                }

                got += n;
                bytesleft -= n;
            }

            strData = std::string(data.get(), size);
            return got;
        } else {
            return -2;
        }
    } else {
        return -3;
    }
}

int Socket::accept(const int timeout, sockaddr_storage &remoteaddr) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockd, &readfds);
    timeval to {timeout / 1000000, timeout % 1000000};
    if (select(FD_SETSIZE, &readfds, nullptr, nullptr, &to) == 1) {
        if (FD_ISSET(sockd, &readfds)) {
            socklen_t addrlen = sizeof(remoteaddr);
            SOCKET clientd = ::accept(sockd, reinterpret_cast<sockaddr *>(&remoteaddr), &addrlen);
            if (clientd == -1) {
                return -1;
            }
            return clientd;
        } else {
            return -2;
        }
    } else {
        return -3;
    }
}

bool Socket::isClosed() const {
    return closed;
}

int Socket::init() {
    if (!Socket::initialized) {
        Socket::initialized = true;
#ifdef _WIN32
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(1, 1), &wsaData);
#else
        return 0;
#endif
    }
    return 0;
}

int Socket::uninit() {
    if (Socket::initialized) {
        Socket::initialized = false;
#ifdef _WIN32
        return WSACleanup();
#else
        return 0;
#endif
    }

    return 0;
}

int Socket::getHandle() const {
    return sockd;
}


inline void checkIsClosing() {
    if (rtStgs::state == State::CLOSING) {
        throw closing();
    }
}

bool receiveMsg(std::stringstream &str, Socket &socket, int timeout) {
    std::string strReceived;
    std::size_t received = socket.recv(strReceived, 4, timeout);
    if (received != 4) {
        return false;
    }
    std::stringstream strCopy;
    str << strReceived;
    strCopy << strReceived;
    NetMessageCode opcode;
    uint24_t len;
    unpack(strCopy, opcode);
    unpack(strCopy, len);
    strReceived.clear();
    if (socket.recv(strReceived, len, timeout) == static_cast<int>(len)) {
        str << strReceived;
        return true;
    } else {
        return false;
    }
}

bool sendMsg(std::stringstream &str, Socket &socket, NetMessageCode opcode) {
    std::string sendString;
    while (!str.eof()) {
        char c = str.get();
        sendString.push_back(c);
    }
    std::stringstream sendStr;
    pack(sendStr, opcode);
    pack(sendStr, static_cast<uint24_t>(sendString.size()));
    sendString = sendStr.str() + sendString;
    if (socket.send(sendString) == static_cast<int>(sendString.size())) {
        return true;
    } else {
        return false;
    }
}


void networkThread() {
    try {
        while (1) {
            Socket listener(rtStgs::port);
            checkIsClosing();
            if (!listener.isClosed()) {
                rtStgs::state = State::WAITING_FOR_CONNECTION;

                sockaddr_storage remoteaddr;
                int clientSockd = 0;
                while (rtStgs::state == State::WAITING_FOR_CONNECTION || rtStgs::state == State::TIMEOUT) {
                    checkIsClosing();
                    if ((clientSockd = listener.accept(500000, remoteaddr)) > 0) {
                        rtStgs::state = State::AUTHORIZATION;
                        break;
                    }
                }

                Socket socket(clientSockd);

                std::stringstream strIn;
                std::stringstream strOut;
                bool authed = false;
                while (rtStgs::state == State::AUTHORIZATION && !socket.isClosed()) {
                    checkIsClosing();
                    strIn.str(std::string(""));
                    if (receiveMsg(strIn, socket, 250000)) {
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
                                        rtStgs::ping::interval = msg.pingInterval;
                                    }
                                    rtStgs::user = msg.user;
                                    rtStgs::password = msg.password;
                                    rtStgs::connectionMode = msg.connectionMode;
                                    resp.result = AuthResult::Authenticated;
                                    resp.displayString = "Hello :)";
                                }
                                pack(strOut, resp);
                                sendMsg(strOut, socket, MSG_AUTH_SERVER);
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
                                rtStgs::render::chars.resize(msg.resolution.w, msg.resolution.h);
                                for (std::size_t i = 0; i < msg.chars.size(); ++i) {
                                    int x = i % msg.resolution.w;
                                    int y = i / msg.resolution.w;
                                    rtStgs::render::chars.get(x, y) = msg.chars.at(i);
                                }
                                rtStgs::state = State::CONNECTION_ATTEMPT;
                            }
                        }
                    }
                }

                while (rtStgs::state == State::CONNECTION_ATTEMPT) {
                    checkIsClosing();
                }

                while (rtStgs::state == State::CONNECTED && !socket.isClosed()) {
                    checkIsClosing();
                    strIn.str(std::string(""));
                    if (receiveMsg(strIn, socket, 250000)) {
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
                                rtStgs::render::chars.set(msg.x, msg.y, msg.chars, msg.vertical);
                                break;
                            }
                            case MSG_COPY: {
                                nmsg::NetMessageCopy msg;
                                unpack(strIn, msg);
                                rtStgs::render::chars.copy(msg.x, msg.y, msg.w, msg.h, msg.tx, msg.ty);
                                break;
                            }
                            case MSG_FILL: {
                                nmsg::NetMessageFill msg;
                                unpack(strIn, msg);
                                rtStgs::render::chars.fill(msg.x, msg.y, msg.w, msg.h, msg.c);
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
                                sendMsg(strOut, socket, MSG_INITIAL_DATA);
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
                                if (msg.pong == rtStgs::ping::challenge) {
                                    rtStgs::ping::sent = false;
                                    rtStgs::ping::challenge = 0;
                                    rtStgs::ping::clock::clock.restart();
                                    rtStgs::ping::clock::timeout.restart();
                                }
                                break;
                            }
                        }
                    }
                    // Send messages in the queue
                    while (!rtStgs::msgQueue::out.empty()) {
                        strOut.str(std::string(""));
                        std::unique_ptr<NetMessage> &baseMsg = rtStgs::msgQueue::out.front();
                        switch (baseMsg->code()) {
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
                                std::unique_ptr<nmsg::NetMessageError> msg = dynamic_unique_ptr_cast<nmsg::NetMessageError>(baseMsg);
                                pack(strOut, *msg);
                                sendMsg(strOut, socket, msg->code());
                                break;
                            }
                            case MSG_EVENT_TOUCH: {
                                std::unique_ptr<nmsg::NetMessageEventTouch> msg = dynamic_unique_ptr_cast<nmsg::NetMessageEventTouch>(baseMsg);
                                pack(strOut, *msg);
                                sendMsg(strOut, socket, msg->code());
                                break;
                            }
                            case MSG_EVENT_DRAG: {
                                std::unique_ptr<nmsg::NetMessageEventDrag> msg = dynamic_unique_ptr_cast<nmsg::NetMessageEventDrag>(baseMsg);
                                pack(strOut, *msg);
                                sendMsg(strOut, socket, msg->code());
                                break;
                            }
                            case MSG_EVENT_DROP: {
                                std::unique_ptr<nmsg::NetMessageEventDrop> msg = dynamic_unique_ptr_cast<nmsg::NetMessageEventDrop>(baseMsg);
                                pack(strOut, *msg);
                                sendMsg(strOut, socket, msg->code());
                                break;
                            }
                            case MSG_EVENT_SCROLL: {
                                std::unique_ptr<nmsg::NetMessageEventScroll> msg = dynamic_unique_ptr_cast<nmsg::NetMessageEventScroll>(baseMsg);
                                pack(strOut, *msg);
                                sendMsg(strOut, socket, msg->code());
                                break;
                            }
                            case MSG_EVENT_KEY_DOWN: {
                                std::unique_ptr<nmsg::NetMessageEventKeyDown> msg = dynamic_unique_ptr_cast<nmsg::NetMessageEventKeyDown>(baseMsg);
                                pack(strOut, *msg);
                                sendMsg(strOut, socket, msg->code());
                                break;
                            }
                            case MSG_EVENT_KEY_UP: {
                                std::unique_ptr<nmsg::NetMessageEventKeyUp> msg = dynamic_unique_ptr_cast<nmsg::NetMessageEventKeyUp>(baseMsg);
                                pack(strOut, *msg);
                                sendMsg(strOut, socket, msg->code());
                                break;
                            }
                            case MSG_EVENT_CLIPBOARD: {
                                std::unique_ptr<nmsg::NetMessageEventClipboard> msg = dynamic_unique_ptr_cast<nmsg::NetMessageEventClipboard>(baseMsg);
                                pack(strOut, *msg);
                                sendMsg(strOut, socket, msg->code());
                                break;
                            }
                        }
                        rtStgs::msgQueue::out.pop();
                    }
                }
                checkIsClosing();
            } else {
                std::cerr << "Could not create a listener!\n";
                throw error_occured();
            }
        }
    } catch (closing &e) {
        // pass
    } catch (error_occured &e) {
        rtStgs::state = State::ERROR;
        std::cerr << "Network thread terminated.\n";
    }
}
