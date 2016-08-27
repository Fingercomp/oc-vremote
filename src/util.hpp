#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "SFML/Graphics.hpp"


class Color {
    uint8_t _r = 0x00;
    uint8_t _g = 0x00;
    uint8_t _b = 0x00;
public:
    Color();
    Color(uint32_t rgb);
    Color(uint8_t r, uint8_t g, uint8_t b);
    uint8_t r() const;
    uint8_t g() const;
    uint8_t b() const;
    uint32_t rgb() const;
    void r(uint8_t v);
    void g(uint8_t v);
    void b(uint8_t v);
    void rgb(uint32_t v);
    void rgb(uint8_t r, uint8_t g, uint8_t b);
    bool operator==(const Color &c) const;
    operator sf::Color() const;
public:
    static float delta(const Color &c1, const Color &c2);
};

class Palette {
    std::array<Color, 256> _colors;
public:
    Palette();
    Palette(const std::array<Color, 16> &palette);
    void initializeToGrayscale();
    void generatePalette();
    int color2index(const Color color) const;
    Color index2color(const int idx) const;
    Color& operator[](const int idx);
    const Color& operator[](const int idx) const;
};


template <typename T>
inline void fill(std::vector<T> &vector, int size) {
    vector.reserve(size);
    vector.resize(size);
}
