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
