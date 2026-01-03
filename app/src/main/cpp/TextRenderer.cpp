#include "TextRenderer.h"
#include "Renderer.h"
#include <cmath>
#include <cstring>

// Simple 5x7 bitmap font for digits and basic characters
// Each character is 5 pixels wide, 7 pixels tall
static const unsigned char font_bitmap[][7] = {
    // 0-9
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}, // 1
    {0x0E, 0x11, 0x01, 0x0E, 0x10, 0x10, 0x1F}, // 2
    {0x0E, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0E}, // 3
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}, // 4
    {0x1F, 0x10, 0x10, 0x1E, 0x01, 0x01, 0x1E}, // 5
    {0x0E, 0x11, 0x10, 0x1E, 0x11, 0x11, 0x0E}, // 6
    {0x1F, 0x01, 0x02, 0x04, 0x04, 0x04, 0x04}, // 7
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}, // 8
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x11, 0x0E}, // 9
    // A-Z
    {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}, // A
    {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}, // B
    {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}, // C
    {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E}, // D
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}, // E
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}, // F
    {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0E}, // G
    {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}, // H
    {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E}, // I
    {0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C}, // J
    {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}, // K
    {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}, // L
    {0x11, 0x1B, 0x15, 0x11, 0x11, 0x11, 0x11}, // M
    {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11}, // N
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, // O
    {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10}, // P
    {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D}, // Q
    {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}, // R
    {0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E}, // S
    {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}, // T
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, // U
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04}, // V
    {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11}, // W
    {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11}, // X
    {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04}, // Y
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F}, // Z
    // Space
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    // Colon
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00}, // :
    // Plus
    {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00}, // +
    // Minus
    {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}, // -
    // Slash
    {0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10}, // /
};

TextRenderer::TextRenderer()
    : m_renderer(nullptr)
    , m_charWidth(5.0f)
    , m_charHeight(7.0f)
{
}

TextRenderer::~TextRenderer() {
    cleanup();
}

bool TextRenderer::initialize(Renderer* renderer) {
    m_renderer = renderer;
    return m_renderer != nullptr;
}

void TextRenderer::cleanup() {
    m_renderer = nullptr;
}

void TextRenderer::drawText(float x, float y, const std::string& text, float r, float g, float b, float a, float scale) {
    if (!m_renderer) return;
    
    float currentX = x;
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        if (c == ' ') {
            currentX += m_charWidth * scale * 1.2f;
            continue;
        }
        drawChar(c, currentX, y, r, g, b, a, scale);
        currentX += m_charWidth * scale * 1.2f;
    }
}

void TextRenderer::drawNumber(float x, float y, int number, float r, float g, float b, float a, float scale) {
    if (number < 0) {
        drawChar('-', x, y, r, g, b, a, scale);
        x += m_charWidth * scale * 1.2f;
        number = -number;
    }
    
    std::string numStr = std::to_string(number);
    drawText(x, y, numStr, r, g, b, a, scale);
}

void TextRenderer::drawTime(float x, float y, int seconds, float r, float g, float b, float a, float scale) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    
    drawNumber(x, y, minutes, r, g, b, a, scale);
    x += m_charWidth * scale * 2.0f;
    drawChar(':', x, y, r, g, b, a, scale);
    x += m_charWidth * scale * 1.5f;
    if (secs < 10) {
        drawNumber(x, y, 0, r, g, b, a, scale);
        x += m_charWidth * scale * 1.2f;
    }
    drawNumber(x, y, secs, r, g, b, a, scale);
}

float TextRenderer::getTextWidth(const std::string& text, float scale) const {
    float width = 0.0f;
    for (size_t i = 0; i < text.length(); ++i) {
        if (text[i] == ' ') {
            width += m_charWidth * scale * 1.2f;
        } else {
            width += m_charWidth * scale * 1.2f;
        }
    }
    return width;
}

float TextRenderer::getTextHeight(float scale) const {
    return m_charHeight * scale;
}

void TextRenderer::drawChar(char c, float x, float y, float r, float g, float b, float a, float scale) {
    int charIndex = -1;
    
    if (c >= '0' && c <= '9') {
        charIndex = c - '0';
    } else if (c >= 'A' && c <= 'Z') {
        charIndex = 10 + (c - 'A');
    } else if (c >= 'a' && c <= 'z') {
        charIndex = 10 + (c - 'a');
    } else if (c == ' ') {
        charIndex = 36;
    } else if (c == ':') {
        charIndex = 37;
    } else if (c == '+') {
        charIndex = 38;
    } else if (c == '-') {
        charIndex = 39;
    } else if (c == '/') {
        charIndex = 40;
    }
    
    if (charIndex < 0 || charIndex >= 41) {
        return;
    }
    
    const unsigned char* bitmap = font_bitmap[charIndex];
    float pixelSize = scale;
    
    for (int row = 0; row < 7; ++row) {
        unsigned char rowData = bitmap[row];
        for (int col = 0; col < 5; ++col) {
            if (rowData & (1 << (4 - col))) {
                float px = x + col * pixelSize;
                float py = y + row * pixelSize;
                m_renderer->drawRect(px, py, pixelSize, pixelSize, r, g, b, a);
            }
        }
    }
}

