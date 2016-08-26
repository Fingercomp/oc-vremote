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
    Char c1;
    c1.c = 97; // "a"
    c1.fg = 14;
    c1.bg = 14;
    Char c2;
    c2.c = 99; // "c"
    c2.fg = 10;
    c2.bg = 10;
    charmap.get(0, 0) = c1;
    charmap.get(1, 1) = c2;
    Palette palette;

    for (int i = 0; i < 256; ++i) {
        Color c = palette.index2color(i);
        std::cout << "i: " << static_cast<int>(i) << "; r: " << static_cast<int>(c.r()) << "; g: " << static_cast<int>(c.g()) << "; b: " << static_cast<int>(c.b()) << "\n";
    }

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
