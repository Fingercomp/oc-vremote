#include <fstream>
#include <iostream>
#include <thread>
#include <queue>

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
                    rtStgs::state = State::CLOSING;
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
                            nmsg::NetMessageEventKeyDown msg;
                            msg.cod = static_cast<long>(keymap.at(k));
                            msg.chr = 0;
                            rtStgs::msgQueue::out.push(&msg);
                        }
                    }
                    break;
                }
                case sf::Event::KeyReleased: {
                    if (rtStgs::state == State::CONNECTED) {
                        if (keyQueueReleased.size() > 1) {
                            sf::Keyboard::Key k = keyQueueReleased.front();
                            keyQueueReleased.pop();
                            nmsg::NetMessageEventKeyDown msg;
                            msg.cod = static_cast<long>(keymap.at(k));
                            msg.chr = 0;
                            rtStgs::msgQueue::out.push(&msg);
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
                        nmsg::NetMessageEventKeyDown msgdown;
                        msgdown.cod = static_cast<long>(keymap.at(kdown));
                        msgdown.chr = event.text.unicode;
                        nmsg::NetMessageEventKeyUp msgup;
                        msgup.cod = static_cast<long>(keymap.at(kup));
                        if (kdown == kup) {
                            msgup.chr = event.text.unicode;
                        } else {
                            msgup.chr = 0;
                        }
                        rtStgs::msgQueue::out.push(&msgdown);
                        rtStgs::msgQueue::out.push(&msgup);
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
                                nmsg::NetMessageEventTouch msg;
                                msg.x = x;
                                msg.y = y;
                                msg.button = button;
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
                                nmsg::NetMessageEventDrag msg;
                                msg.x = x;
                                msg.y = y;
                                msg.button = button;
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
                                    nmsg::NetMessageEventDrop msg;
                                    msg.x = x;
                                    msg.y = y;
                                    msg.button = button;
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
                                nmsg::NetMessageEventScroll msg;
                                msg.x = x;
                                msg.y = y;
                                msg.direction = event.mouseWheelScroll.delta < 0 ? false : true;
                                msg.delta = static_cast<uint8_t>(std::abs(event.mouseWheelScroll.delta));
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
                rtStgs::render::chars.set(0, 0, "Initializing...");
                break;
            case State::WAITING_FOR_CONNECTION:
                rtStgs::render::chars.set(0, 0, "Waiting for connection...");
                break;
            case State::CONNECTION_ATTEMPT:
                rtStgs::render::chars.set(0, 0, "A client has attempted to connect");
                rtStgs::render::chars.set(0, 1, "Accept? [y/n]");
                break;
            case State::AUTHORIZATION:
                rtStgs::render::chars.set(0, 0, "The client is currently authorizating.");
                rtStgs::render::chars.set(0, 1, "Please, wait.");
                break;
            case State::CONNECTED:
                break;
            case State::CLOSING:
                break;
        }
        window.clear();
        tilemap.update();
        window.draw(tilemap);
        window.display();
    }
    thNet.join();
}
