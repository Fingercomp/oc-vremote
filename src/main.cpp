#include <fstream>
#include <iostream>
#include <thread>

#include <SFML/Graphics.hpp>

#include "graphics.hpp"
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

    std::thread thNet(networkThread);

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
                rtStgs::render::chars.set(0, 1, "Accept?");
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
