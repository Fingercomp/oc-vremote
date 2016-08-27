#include "util.hpp"


Color::Color() {}

Color::Color(uint32_t rgb) {
    _r = (rgb >> 16) & 0xff;
    _g = (rgb >> 8) & 0xff;
    _b = rgb & 0xff;
}

Color::Color(uint8_t r, uint8_t g, uint8_t b): _r(r), _g(g), _b(b) {}

uint8_t Color::r() const {
    return _r;
}

uint8_t Color::g() const {
    return _g;
}

uint8_t Color::b() const {
    return _b;
}

uint32_t Color::rgb() const {
    return (_r << 16) | (_g << 8) | _b;
}

void Color::r(uint8_t v) {
    _r = v;
}

void Color::g(uint8_t v) {
    _g = v;
}

void Color::b(uint8_t v) {
    _b = v;
}

void Color::rgb(uint32_t v) {
    _r = (v >> 16) & 0xff;
    _g = (v >> 8) & 0xff;
    _b = v & 0xff;
}

void Color::rgb(uint8_t rv, uint8_t gv, uint8_t bv) {
    r(rv);
    g(gv);
    b(bv);
}

float Color::delta(const Color &c1, const Color &c2) {
    int dr = c1.r() - c2.r();
    int dg = c1.g() - c2.g();
    int db = c1.b() - c2.b();
    return .2126 * dr * dr + .7152 * dg * dg + .0722 * db * db;
}

bool Color::operator==(const Color &c) const {
    return _r == c.r() && _g == c.g() && _b == c.b();
}

Color::operator sf::Color() const {
    return sf::Color(_r, _g, _b, 0xff);
}



Palette::Palette() {
    initializeToGrayscale();
    generatePalette();
}

Palette::Palette(const std::array<Color, 16> &palette) {
    for (int i = 0; i < 16; ++i) {
        _colors.at(i) = palette.at(i);
    }
    generatePalette();
}

void Palette::initializeToGrayscale() {
    for (int i = 0; i < 16; ++i) {
        uint8_t shade = 0xff * (i + 1) / 17;
        _colors.at(i) = Color(shade, shade, shade);
    }
}

void Palette::generatePalette() {
    for (int i = 0; i < 240; ++i) {
        int r = i % 6;
        int g = (i / 6) % 8;
        int b = (i / (6 * 8));
        r = (r * 255 + 2) / 5;
        g = (g * 255 + 3) / 7;
        b = (b * 255 + 2) / 4;
        _colors.at(i + 16) = Color(r, g, b);
    }
}

Color Palette::index2color(const int idx) const {
    return _colors.at(idx);
}

int Palette::color2index(const Color color) const {
    for (int i = 0; i < 256; ++i) {
        if (_colors.at(i) == color) {
            return i;
        }
    }
    int idxR = color.r() * 5 / 255 + .5;
    int idxG = color.g() * 7 / 255 + .5;
    int idxB = color.b() * 4 / 255 + .5;
    int idx = 16 + idxR * 8 * 5 + idxG * 5 + idxB;
    bool setDelta = false;
    float minDelta;
    float minDIdx;
    for (int i = 0; i < 16; ++i) {
        float d = Color::delta(_colors.at(i), color);
        if (!setDelta || d < minDelta) {
            minDelta = d;
            minDIdx = i;
            setDelta = true;
        }
    }
    if (Color::delta(index2color(idx), color) < Color::delta(index2color(minDIdx), color)) {
        return idx;
    } else {
        return minDIdx;
    }
}

Color& Palette::operator[](const int idx) {
    return _colors.at(idx);
}

const Color& Palette::operator[](const int idx) const {
    return _colors.at(idx);
}
