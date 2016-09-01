#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>

#include <SFML/Graphics.hpp>

#include "graphics.hpp"
#include "keyboard.hpp"
#include "main.hpp"
#include "network.hpp"
#include "protocol.hpp"
#include "runtime.hpp"
#include "util.hpp"


int main() {
    std::fstream fio("resources/font.hex");
    std::istream *fistr = &fio;
    sf::Texture texture;
    std::vector<unsigned long> indexes;
    loadBitmapFont(fistr, indexes, texture);
    fio.close();
    fistr = nullptr;

    Palette palette;

    Tilemap tilemap(texture, rtStgs::render::chars, indexes, palette);

    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");
    window.setKeyRepeatEnabled(false);

    std::thread thNet(networkThread);

    std::queue<sf::Keyboard::Key> keyQueuePressed;
    std::queue<sf::Keyboard::Key> keyQueueReleased;

    bool wasDrag = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::Resized: {
                    sf::FloatRect visibleArea(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                    window.setView(sf::View(visibleArea));
                    break;
                }
                case sf::Event::KeyPressed: {
                    if (rtStgs::state == State::CONNECTION_ATTEMPT) {
                        if (event.key.code == sf::Keyboard::Y) {
                            rtStgs::state = State::AUTHORIZATION;
                        } else if (event.key.code == sf::Keyboard::N) {
                            rtStgs::state = State::WAITING_FOR_CONNECTION;
                        }
                    } else if (rtStgs::state == State::CONNECTED) {
                        keyQueuePressed.push(event.key.code);
                        if (keyQueuePressed.size() > 1) {
                            sf::Keyboard::Key k = keyQueuePressed.front();
                            keyQueuePressed.pop();
                            std::shared_ptr<nmsg::NetMessageEventKeyDown> msg = std::make_shared<nmsg::NetMessageEventKeyDown>();
                            msg->cod = static_cast<long>(keymap.at(k));
                            msg->chr = 0;
                            rtStgs::msgQueue::out.push(msg);
                        }
                    }
                    break;
                }
                case sf::Event::KeyReleased: {
                    if (rtStgs::state == State::CONNECTED) {
                        if (keyQueueReleased.size() > 1) {
                            sf::Keyboard::Key k = keyQueueReleased.front();
                            keyQueueReleased.pop();
                            std::shared_ptr<nmsg::NetMessageEventKeyUp> msg = std::make_shared<nmsg::NetMessageEventKeyUp>();
                            msg->cod = static_cast<long>(keymap.at(k));
                            msg->chr = 0;
                            rtStgs::msgQueue::out.push(msg);
                        }
                    }
                    break;
                }
                case sf::Event::TextEntered: {
                    if (rtStgs::state == State::CONNECTED) {
                        sf::Keyboard::Key kdown = keyQueuePressed.front();
                        sf::Keyboard::Key kup = keyQueueReleased.front();
                        keyQueuePressed.pop();
                        keyQueueReleased.pop();
                        std::shared_ptr<nmsg::NetMessageEventKeyDown> msgdown = std::make_shared<nmsg::NetMessageEventKeyDown>();
                        msgdown->cod = static_cast<long>(keymap.at(kdown));
                        msgdown->chr = event.text.unicode;
                        std::shared_ptr<nmsg::NetMessageEventKeyUp> msgup = std::make_shared<nmsg::NetMessageEventKeyUp>();
                        msgup->cod = static_cast<long>(keymap.at(kup));
                        if (kdown == kup) {
                            msgup->chr = event.text.unicode;
                        } else {
                            msgup->chr = 0;
                        }
                        rtStgs::msgQueue::out.push(msgdown);
                        rtStgs::msgQueue::out.push(msgup);
                    }
                    break;
                }
                case sf::Event::MouseButtonPressed: {
                    if (rtStgs::state == State::CONNECTED) {
                        int button = -1;
                        switch (event.mouseButton.button) {
                            case sf::Mouse::Left:
                                button = 1;
                                break;
                            case sf::Mouse::Right:
                                button = -1;
                                break;
                            case sf::Mouse::Middle:
                                button = 0;
                                break;
                            default:
                                break;
                        }
                        if (button != -1) {
                            sf::Vector2i point(event.mouseButton.x, event.mouseButton.y);
                            sf::Vector2f pos = window.mapPixelToCoords(point);
                            uint8_t x = static_cast<uint8_t>(pos.x);
                            uint8_t y = static_cast<uint8_t>(pos.y);
                            x /= 8;
                            y /= 16;
                            if (x >= 0 && y >= 0 && x < rtStgs::render::resolution.w && y <= rtStgs::render::resolution.h) {
                                std::shared_ptr<nmsg::NetMessageEventTouch> msg = std::make_shared<nmsg::NetMessageEventTouch>();
                                msg->x = x;
                                msg->y = y;
                                msg->button = button;
                                rtStgs::msgQueue::out.push(msg);
                            }
                        }
                    }
                }
                case sf::Event::MouseMoved: {
                    if (rtStgs::state == State::CONNECTED) {
                        int button = -1;
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                            button = 1;
                        } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                            button = -1;
                        } else if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
                            button = 0;
                        }
                        if (button != -1) {
                            sf::Vector2i point(event.mouseMove.x, event.mouseMove.y);
                            sf::Vector2f pos = window.mapPixelToCoords(point);
                            uint8_t x = static_cast<uint8_t>(pos.x);
                            uint8_t y = static_cast<uint8_t>(pos.y);
                            x /= 8;
                            y /= 16;
                            if (x >= 0 && y >= 0 && x < rtStgs::render::resolution.w && y <= rtStgs::render::resolution.h) {
                                std::shared_ptr<nmsg::NetMessageEventDrag> msg = std::make_shared<nmsg::NetMessageEventDrag>();
                                msg->x = x;
                                msg->y = y;
                                msg->button = button;
                                rtStgs::msgQueue::out.push(msg);
                                wasDrag = true;
                            }
                        }
                    }
                }
                case sf::Event::MouseButtonReleased: {
                    if (rtStgs::state == State::CONNECTED) {
                        if (wasDrag) {
                            int button = -1;
                            switch (event.mouseButton.button) {
                                case sf::Mouse::Left:
                                    button = 1;
                                    break;
                                case sf::Mouse::Right:
                                    button = -1;
                                    break;
                                case sf::Mouse::Middle:
                                    button = 0;
                                    break;
                                default:
                                    break;
                            }
                            if (button != -1) {
                                sf::Vector2i point(event.mouseButton.x, event.mouseButton.y);
                                sf::Vector2f pos = window.mapPixelToCoords(point);
                                uint8_t x = static_cast<uint8_t>(pos.x);
                                uint8_t y = static_cast<uint8_t>(pos.y);
                                x /= 8;
                                y /= 16;
                                if (x >= 0 && y >= 0 && x < rtStgs::render::resolution.w && y <= rtStgs::render::resolution.h) {
                                    std::shared_ptr<nmsg::NetMessageEventDrop> msg = std::make_shared<nmsg::NetMessageEventDrop>();
                                    msg->x = x;
                                    msg->y = y;
                                    msg->button = button;
                                    rtStgs::msgQueue::out.push(msg);
                                }
                            }
                        }
                    }
                }
                case sf::Event::MouseWheelScrolled: {
                    if (rtStgs::state == State::CONNECTED) {
                        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                            sf::Vector2i point(event.mouseButton.x, event.mouseButton.y);
                            sf::Vector2f pos = window.mapPixelToCoords(point);
                            uint8_t x = static_cast<uint8_t>(pos.x);
                            uint8_t y = static_cast<uint8_t>(pos.y);
                            x /= 8;
                            y /= 16;
                            if (x >= 0 && y >= 0 && x < rtStgs::render::resolution.w && y <= rtStgs::render::resolution.h) {
                                std::shared_ptr<nmsg::NetMessageEventScroll> msg = std::make_shared<nmsg::NetMessageEventScroll>();
                                msg->x = x;
                                msg->y = y;
                                msg->direction = event.mouseWheelScroll.delta < 0 ? false : true;
                                msg->delta = static_cast<uint8_t>(std::abs(event.mouseWheelScroll.delta));
                                rtStgs::msgQueue::out.push(msg);
                            }
                        }
                    }
                }
                default:
                    break;
            }
        }
        switch (rtStgs::state) {
            case State::INITIALIZING:
                rtStgs::render::fg = 255;
                rtStgs::render::bg = 16;
                rtStgs::render::chars.fill(0, 0, rtStgs::render::resolution.w, rtStgs::render::resolution.h, 32);
                rtStgs::render::chars.set(0, 0, "Initializing...");
                break;
            case State::WAITING_FOR_CONNECTION:
                rtStgs::render::fg = 255;
                rtStgs::render::bg = 16;
                rtStgs::render::chars.fill(0, 0, rtStgs::render::resolution.w, rtStgs::render::resolution.h, 32);
                rtStgs::render::chars.set(0, 0, "Waiting for connection...");
                break;
            case State::CONNECTION_ATTEMPT:
                rtStgs::render::fg = 255;
                rtStgs::render::bg = 16;
                rtStgs::render::chars.fill(0, 0, rtStgs::render::resolution.w, rtStgs::render::resolution.h, 32);
                rtStgs::render::chars.set(0, 0, "A client has attempted to connect");
                rtStgs::render::chars.set(0, 1, "Accept? [y/n]");
                break;
            case State::AUTHORIZATION:
                rtStgs::render::fg = 255;
                rtStgs::render::bg = 16;
                rtStgs::render::chars.fill(0, 0, rtStgs::render::resolution.w, rtStgs::render::resolution.h, 32);
                rtStgs::render::chars.set(0, 0, "The client is currently authorizating.");
                rtStgs::render::chars.set(0, 1, "Please, wait.");
                rtStgs::ping::clock::clock.restart();
                break;
            case State::CONNECTED:
                if (!rtStgs::ping::sent && rtStgs::ping::clock::clock.getElapsedTime() >= sf::seconds(rtStgs::ping::interval)) {
                    std::shared_ptr<nmsg::NetMessagePing> msg = std::make_shared<nmsg::NetMessagePing>();
                    // XXX: it that random enough?
                    msg->ping = (0xffffffffffffffff ^ (rtStgs::ping::interval << 4) ^ rtStgs::ping::clock::clock.getElapsedTime().asMicroseconds() ^ (rand() % 0xffffffffffffffff)) & 0xffffffffffffffff;
                    rtStgs::ping::challenge = msg->ping;
                    rtStgs::msgQueue::out.push(msg);
                    rtStgs::ping::clock::clock.restart();
                    rtStgs::ping::clock::timeout.restart();
                    rtStgs::ping::sent = true;
                }
                if (rtStgs::ping::sent && rtStgs::ping::clock::timeout.getElapsedTime() >= sf::seconds(30)) {
                    rtStgs::state = State::TIMEOUT;
                    rtStgs::ping::sent = false;
                    rtStgs::ping::clock::clock.restart();
                    rtStgs::ping::clock::timeout.restart();
                }
                break;
            case State::CLOSING:
                window.close();
                break;
            case State::ERROR:
                rtStgs::render::fg = 255;
                rtStgs::render::bg = 16;
                rtStgs::render::chars.fill(0, 0, rtStgs::render::resolution.w, rtStgs::render::resolution.h, 32);
                rtStgs::render::chars.set(0, 0, "An error has occured.");
                rtStgs::render::chars.set(0, 1, "See console output for more details.");
                break;
            case State::TIMEOUT:
                rtStgs::render::fg = 255;
                rtStgs::render::bg = 16;
                rtStgs::render::chars.fill(0, 0, rtStgs::render::resolution.w, rtStgs::render::resolution.h, 32);
                rtStgs::render::chars.set(0, 0, "The client has timed out.");
                break;
        }
        window.clear();
        tilemap.update();
        window.draw(tilemap);
        window.display();
    }
    // Just to be on the safe side ;)
    rtStgs::state = State::CLOSING;
    thNet.join();
}
