#include "Button.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include <algorithm>

Button::Button()
    : m_x(0.0f)
    , m_y(0.0f)
    , m_width(100.0f)
    , m_height(50.0f)
    , m_text("")
    , m_colorR(0.3f)
    , m_colorG(0.3f)
    , m_colorB(0.3f)
    , m_colorA(1.0f)
    , m_pressedR(0.5f)
    , m_pressedG(0.5f)
    , m_pressedB(0.5f)
    , m_pressedA(1.0f)
    , m_textR(1.0f)
    , m_textG(1.0f)
    , m_textB(1.0f)
    , m_textA(1.0f)
    , m_textScale(1.0f)
    , m_pressed(false)
    , m_cornerRadius(8.0f)
{
}

Button::~Button() {
}

void Button::setBounds(float x, float y, float width, float height) {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
}

void Button::setText(const std::string& text) {
    m_text = text;
}

void Button::setColor(float r, float g, float b, float a) {
    m_colorR = r;
    m_colorG = g;
    m_colorB = b;
    m_colorA = a;
}

void Button::setPressedColor(float r, float g, float b, float a) {
    m_pressedR = r;
    m_pressedG = g;
    m_pressedB = b;
    m_pressedA = a;
}

void Button::setTextColor(float r, float g, float b, float a) {
    m_textR = r;
    m_textG = g;
    m_textB = b;
    m_textA = a;
}

void Button::setTextScale(float scale) {
    m_textScale = scale;
}

void Button::render(Renderer* renderer, TextRenderer* textRenderer) {
    if (!renderer) return;
    
    // Draw button background
    float r, g, b, a;
    if (m_pressed) {
        r = m_pressedR;
        g = m_pressedG;
        b = m_pressedB;
        a = m_pressedA;
    } else {
        r = m_colorR;
        g = m_colorG;
        b = m_colorB;
        a = m_colorA;
    }
    
    renderer->drawRoundedRect(m_x, m_y, m_width, m_height, m_cornerRadius, r, g, b, a);
    
    // Draw text if available
    if (textRenderer && !m_text.empty()) {
        float textWidth = textRenderer->getTextWidth(m_text, m_textScale);
        float textHeight = textRenderer->getTextHeight(m_textScale);
        float textX = m_x + (m_width - textWidth) / 2.0f;
        float textY = m_y + (m_height - textHeight) / 2.0f;
        textRenderer->drawText(textX, textY, m_text, m_textR, m_textG, m_textB, m_textA, m_textScale);
    }
}

bool Button::containsPoint(float x, float y) const {
    return (x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height);
}

