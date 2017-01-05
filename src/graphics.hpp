#pragma once

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "main.hpp"
#include "protocol.hpp"
#include "runtime.hpp"
#include "util.hpp"


void writeBitmapPixel(std::vector<sf::Uint8> &pixels, bool isWhite);
void loadBitmapFont(std::istream *str, std::vector<unsigned long> &result, sf::Texture &texture);


class Charmap {
private:
    int _w;
    int _h;
    std::vector<Char> _chars;
public:
    Charmap(const int w, const int h);
    int getWidth() const;
    int getHeight() const;
    Char& get(const int x, const int y);
    const Char& get(const int x, const int y) const;

    void resize(const int w, const int h);
    void set(const int x, const int y, const std::string &data, const bool vertical = false);
    void copy(const int x, const int y, const int w, const int h, const int tx, const int ty);
    void fill(const int x, const int y, const int w, const int h, const long chr);
};

class Tilemap: public sf::Drawable, public sf::Transformable {
private:
    Charmap &_charmap;
    sf::Texture &_tileset;
    sf::VertexArray _vertices;
    sf::VertexArray _bgVertices;
    std::vector<unsigned long> &_indexes;
    Palette &_palette;
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
public:
    Tilemap(sf::Texture &texture, Charmap &charmap, std::vector<unsigned long> &indexes, Palette &palette);
    void update();
};
