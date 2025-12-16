#include "IconRenderer.h"
#include "Renderer.h"
#include <cmath>

IconRenderer::IconRenderer()
    : m_renderer(nullptr)
{
}

IconRenderer::~IconRenderer() {
    cleanup();
}

bool IconRenderer::initialize(Renderer* renderer) {
    m_renderer = renderer;
    return m_renderer != nullptr;
}

void IconRenderer::cleanup() {
    m_renderer = nullptr;
}

void IconRenderer::drawIcon(float x, float y, IconType type, float size, float r, float g, float b, float a) {
    if (!m_renderer) return;
    
    switch (type) {
        case IconType::PLAY:
            drawPlayIcon(x, y, size, r, g, b, a);
            break;
        case IconType::PAUSE:
            drawPauseIcon(x, y, size, r, g, b, a);
            break;
        case IconType::PLUS:
            drawPlusIcon(x, y, size, r, g, b, a);
            break;
        case IconType::MINUS:
            drawMinusIcon(x, y, size, r, g, b, a);
            break;
        case IconType::CHECK:
            drawCheckIcon(x, y, size, r, g, b, a);
            break;
        case IconType::ARROW_LEFT:
            drawArrowIcon(x, y, size, r, g, b, a, 0.0f);
            break;
        case IconType::ARROW_RIGHT:
            drawArrowIcon(x, y, size, r, g, b, a, 180.0f);
            break;
        case IconType::ARROW_UP:
            drawArrowIcon(x, y, size, r, g, b, a, 90.0f);
            break;
        case IconType::ARROW_DOWN:
            drawArrowIcon(x, y, size, r, g, b, a, 270.0f);
            break;
    }
}

void IconRenderer::drawPlayIcon(float x, float y, float size, float r, float g, float b, float a) {
    // Triangle pointing right
    float centerX = x + size / 2.0f;
    float centerY = y + size / 2.0f;
    float halfSize = size / 3.0f;
    
    // Draw triangle using three rectangles (simplified)
    float x1 = centerX - halfSize;
    float y1 = centerY - halfSize;
    float x2 = centerX + halfSize;
    float y2 = centerY;
    float x3 = centerX - halfSize;
    float y3 = centerY + halfSize;
    
    // Draw as filled triangle approximation
    m_renderer->drawRect(x1, y1, halfSize * 2, halfSize, r, g, b, a);
    m_renderer->drawRect(x1, centerY, halfSize * 2, halfSize, r, g, b, a);
}

void IconRenderer::drawPauseIcon(float x, float y, float size, float r, float g, float b, float a) {
    float barWidth = size / 6.0f;
    float barHeight = size * 0.6f;
    float centerY = y + (size - barHeight) / 2.0f;
    float gap = size / 3.0f;
    
    m_renderer->drawRect(x + gap, centerY, barWidth, barHeight, r, g, b, a);
    m_renderer->drawRect(x + size - gap - barWidth, centerY, barWidth, barHeight, r, g, b, a);
}

void IconRenderer::drawPlusIcon(float x, float y, float size, float r, float g, float b, float a) {
    float thickness = size / 4.0f;
    float centerX = x + size / 2.0f;
    float centerY = y + size / 2.0f;
    
    // Horizontal bar
    m_renderer->drawRect(x + size * 0.25f, centerY - thickness / 2.0f, size * 0.5f, thickness, r, g, b, a);
    // Vertical bar
    m_renderer->drawRect(centerX - thickness / 2.0f, y + size * 0.25f, thickness, size * 0.5f, r, g, b, a);
}

void IconRenderer::drawMinusIcon(float x, float y, float size, float r, float g, float b, float a) {
    float thickness = size / 4.0f;
    float centerY = y + size / 2.0f;
    
    m_renderer->drawRect(x + size * 0.25f, centerY - thickness / 2.0f, size * 0.5f, thickness, r, g, b, a);
}

void IconRenderer::drawCheckIcon(float x, float y, float size, float r, float g, float b, float a) {
    float thickness = size / 6.0f;
    float startX = x + size * 0.2f;
    float startY = y + size * 0.5f;
    float endX = x + size * 0.45f;
    float endY = y + size * 0.7f;
    float tipX = x + size * 0.8f;
    float tipY = y + size * 0.3f;
    
    // Draw checkmark as two connected rectangles
    float len1 = sqrtf((endX - startX) * (endX - startX) + (endY - startY) * (endY - startY));
    float len2 = sqrtf((tipX - endX) * (tipX - endX) + (tipY - endY) * (tipY - endY));
    
    // Simplified: draw as horizontal and diagonal segments
    m_renderer->drawRect(startX, startY, len1 * 0.7f, thickness, r, g, b, a);
    m_renderer->drawRect(endX, endY - thickness, len2 * 0.6f, thickness, r, g, b, a);
}

void IconRenderer::drawArrowIcon(float x, float y, float size, float r, float g, float b, float a, float rotation) {
    // Draw arrow as triangle + rectangle
    float centerX = x + size / 2.0f;
    float centerY = y + size / 2.0f;
    float halfSize = size / 3.0f;
    
    // Simplified arrow (always points right, rotation not implemented for simplicity)
    m_renderer->drawRect(centerX - halfSize, centerY - size / 6.0f, halfSize * 2, size / 3.0f, r, g, b, a);
    m_renderer->drawRect(centerX + halfSize, centerY - halfSize, halfSize, halfSize * 2, r, g, b, a);
}

