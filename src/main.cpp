#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>
#include <cmath>

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
    window.setFramerateLimit(30);

    std::thread thNet(networkThread);

    long lastKey = -1;

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
                            rtStgs::state = State::CONNECTED;
                        } else if (event.key.code == sf::Keyboard::N) {
                            rtStgs::state = State::WAITING_FOR_CONNECTION;
                        }
                    } else if (rtStgs::state == State::CONNECTED) {
                        lastKey = static_cast<long>(event.key.code);
                    }
                    break;
                }
                case sf::Event::TextEntered: {
                    if (rtStgs::state == State::CONNECTED) {
                        if (lastKey != -1) {
                            nmsg::NetMessageEventKeyDown *msgdown = new nmsg::NetMessageEventKeyDown;
                            msgdown->cod = lastKey;
                            msgdown->chr = event.text.unicode;
                            nmsg::NetMessageEventKeyUp *msgup = new nmsg::NetMessageEventKeyUp;
                            msgup->cod = lastKey;
                            msgup->chr = event.text.unicode;
                            rtStgs::msgQueue::out.push(std::unique_ptr<NetMessage>(msgdown));
                            rtStgs::msgQueue::out.push(std::unique_ptr<NetMessage>(msgup));
                            lastKey = -1;
                        }
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
                            if (x < rtStgs::render::resolution.w && y <= rtStgs::render::resolution.h) {
                                nmsg::NetMessageEventTouch *msg = new nmsg::NetMessageEventTouch;
                                msg->x = x;
                                msg->y = y;
                                msg->button = button;
                                rtStgs::msgQueue::out.push(std::unique_ptr<NetMessage>(msg));
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
                            if (x < rtStgs::render::resolution.w && y <= rtStgs::render::resolution.h) {
                                nmsg::NetMessageEventDrag *msg = new nmsg::NetMessageEventDrag;
                                msg->x = x;
                                msg->y = y;
                                msg->button = button;
                                rtStgs::msgQueue::out.push(std::unique_ptr<NetMessage>(msg));
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
                                if (x < rtStgs::render::resolution.w && y <= rtStgs::render::resolution.h) {
                                    nmsg::NetMessageEventDrop *msg = new nmsg::NetMessageEventDrop;
                                    msg->x = x;
                                    msg->y = y;
                                    msg->button = button;
                                    rtStgs::msgQueue::out.push(std::unique_ptr<NetMessage>(msg));
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
                            if (x < rtStgs::render::resolution.w && y <= rtStgs::render::resolution.h) {
                                nmsg::NetMessageEventScroll *msg = new nmsg::NetMessageEventScroll;
                                msg->x = x;
                                msg->y = y;
                                msg->direction = event.mouseWheelScroll.delta < 0 ? false : true;
                                msg->delta = static_cast<uint8_t>(std::abs(event.mouseWheelScroll.delta));
                                rtStgs::msgQueue::out.push(std::unique_ptr<NetMessage>(msg));
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
                    nmsg::NetMessagePing *msg = new nmsg::NetMessagePing;
                    msg->ping = (0xffffffffffffffff ^ (rtStgs::ping::interval << 4) ^ rtStgs::ping::clock::clock.getElapsedTime().asMicroseconds() ^ (rand() % 0xffffffffffffffff)) & 0xffffffffffffffff;
                    rtStgs::ping::challenge = msg->ping;
                    rtStgs::msgQueue::out.push(std::unique_ptr<NetMessage>(msg));
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
