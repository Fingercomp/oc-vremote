#include <fstream>
#include <iostream>

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
                case sf::Event::Resized: {
                    sf::FloatRect visibleArea(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                    window.setView(sf::View(visibleArea));
                }
                default:
                    break;
            }
        }
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                uint8_t cr = rand() % 255;
                uint8_t cg = rand() % 255;
                uint8_t cb = rand() % 255;
                uint32_t chr = rand() % 0x8000;
                Char c;
                c.c = chr;
                c.fg = palette.color2index(Color(cr, cg, cb));
                charmap.get(i, j) = c;
            }
        }
        window.clear();
        tilemap.update();
        window.draw(tilemap);
        window.display();
    }
}
