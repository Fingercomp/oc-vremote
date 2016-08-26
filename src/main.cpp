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
    Char c;
    c.c = 97; // "a"
    c.fg = 6;
    c.bg = 6;
    charmap.get(0, 0) = c;
    Palette palette;

    Tilemap tilemap(texture, charmap, indexes, palette);


    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");

    // sf::Sprite sprite(texture);

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
                }
                default:
                    break;
            }
        }
        window.clear();
        tilemap.update();
        window.draw(tilemap);
        // window.draw(sprite);
        window.display();
    }
}
