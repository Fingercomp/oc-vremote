#include <fstream>

#include <SFML/Graphics.hpp>

#include "graphics.hpp"
#include "protocol.hpp"
#include "util.hpp"


int main() {
    std::fstream fio("resources/font.hex");
    std::istream *fistr = &fio;
    sf::Texture texture;
    std::vector<unsigned long> indexes;
    loadBitmapFont(fistr, indexes, texture);
    fio.close();
    fistr = nullptr;

    Charmap charmap(10, 10);
    Palette palette;

    Tilemap tilemap(texture, charmap, indexes, palette);


    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                default:
                    break;
            }
        }
        window.clear();
        tilemap.update();
        window.draw(tilemap);
        window.display();
    }
}
