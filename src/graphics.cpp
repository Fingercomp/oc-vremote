#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "graphics.hpp"
#include "util.hpp"


void writeBitmapPixel(std::vector<sf::Uint8> &pixels, bool isWhite) {
    sf::Uint8 color = isWhite ? 0xff : 0x00;
    // RGB
    for (int i = 0; i < 3; ++i) {
        pixels.push_back(color);
    }
    // Alpha
    pixels.push_back(sf::Uint8(0xff));
}

void loadBitmapFont(std::istream *str, std::vector<unsigned long> &result, sf::Texture &texture) {
    static std::vector<sf::Uint8> pixels;
    std::array<std::vector<sf::Uint8>, 16> pixArray;
    unsigned long index = 0;
    while (*str && !str->eof()) {
        std::string line;
        std::getline(*str, line);
        std::string::size_type colPos = line.find(':');
        if (colPos == std::string::npos) {
            break;
        }
        std::string codePnt = line.substr(0, colPos);
        unsigned long iCodePnt = stoul(codePnt, nullptr, 16);
        std::string bitmap = line.substr(colPos + 1, line.size() - colPos - 1);
        std::string chr;
        if (static_cast<int>(bitmap.size()) == 64) {
            // Double-width character
            for (int i = 63; i >= 0; --i) {
                chr.erase(0, chr.size());
                chr.push_back(bitmap.at(63 - i));
                int num = stoi(chr, nullptr, 16);
                for (int j = 0; j < 4; ++j) {
                    writeBitmapPixel(pixArray.at(15 - i / 4), ((num >> j) & 0x1) ? true : false);
                }
            }
            result.push_back(iCodePnt);
            result.push_back(index);
            result.push_back(1);
            index += 2;
        } else if (static_cast<int>(bitmap.size()) == 32) {
            // Single-width character
            for (int i = 31; i >= 0; --i) {
                chr.erase(0, chr.size());
                chr.push_back(bitmap.at(31 - i));
                int num = stoi(chr, nullptr, 16);
                for (int j = 0; j < 4; ++j) {
                    writeBitmapPixel(pixArray.at(15 - i / 2), ((num >> j) & 0x1) ? true : false);
                }
            }
            result.push_back(iCodePnt);
            result.push_back(index++);
            result.push_back(0);
        }
    }
    // "Glue" array of vectors
    for (int i = 0; i < 16; ++i) {
        for (auto item: pixArray.at(i)) {
            pixels.push_back(item);
        }
    }
    texture.create(8 * index, 16);
    texture.update(&pixels[0]);
}


Charmap::Charmap(const int w, const int h): _w(w), _h(h) {
    fill(_chars, w * h);
    for (int i = 0; i < w * h; ++i) {
        Char c;
        c.fg = 16;
        c.bg = 16;
        c.c = 32;  // Whitespace
        _chars.push_back(c);
    }
}

Char& Charmap::get(const int x, const int y) {
    return _chars.at(y * _w + x);
}

const Char& Charmap::get(const int x, const int y) const {
    return _chars.at(y * _w + x);
}

void Charmap::resize(const int w, const int h) {
    std::vector<Char> newChars;
    fill(newChars, w * h);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            Char chr;
            if (x < _w && y < _h) {
                chr = get(x, y);
            }
            newChars.at(y * w + x) = chr;
        }
    }
    _chars = newChars;
    _w = w;
    _h = h;
}

int Charmap::getWidth() const {
    return _w;
}

int Charmap::getHeight() const {
    return _h;
}


Tilemap::Tilemap(sf::Texture &texture, Charmap &charmap, std::vector<unsigned long> &indexes, Palette &palette):
        _charmap(charmap), _tileset(texture), _indexes(indexes), _palette(palette) {}

void Tilemap::update() {
    int w = _charmap.getWidth();
    int h = _charmap.getHeight();

    _vertices.setPrimitiveType(sf::Quads);
    _vertices.resize(w * h * 4);

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            Char chr = _charmap.get(i, j);
            Color fg = _palette.index2color(chr.fg);
            unsigned long codepoint = chr.c;
            int tx1 = 0;
            int tx2 = 0;
            bool isWide = false;
            for (std::string::size_type n = 0; n < _indexes.size(); n += 3) {
                if (_indexes.at(n) == codepoint) {
                    tx1 = _indexes.at(n + 1);
                    isWide = _indexes.at(n + 2) == 1 ? true : false;
                }
            }

            tx2 = isWide ? 8 : 0;

            sf::Vertex *quad = &_vertices[(i + j * w) * 4];
            quad[0].position = sf::Vector2f(i * 8, j * 16);
            quad[1].position = sf::Vector2f(i * 8 + tx2, j * 16);
            quad[2].position = sf::Vector2f(i * 8 + tx2, (j + 1) * 16);
            quad[3].position = sf::Vector2f(i * 8, (j + 1) * 16);

            quad[0].texCoords = sf::Vector2f(tx1 * 8, 0);
            quad[1].texCoords = sf::Vector2f(tx1 * 8 + tx2, 0);
            quad[2].texCoords = sf::Vector2f(tx1 * 8 + tx2, 16);
            quad[3].texCoords = sf::Vector2f(tx1 * 8, 16);

            quad[0].color = fg;
            quad[1].color = fg;
            quad[2].color = fg;
            quad[3].color = fg;
        }
    }
}

void Tilemap::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &_tileset;
    target.draw(_vertices, states);
}
