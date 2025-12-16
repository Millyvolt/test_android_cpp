#ifndef BUTTON_H
#define BUTTON_H

#include <string>

class Renderer;
class TextRenderer;

class Button {
public:
    Button();
    ~Button();
    
    void setBounds(float x, float y, float width, float height);
    void setText(const std::string& text);
    void setColor(float r, float g, float b, float a);
    void setPressedColor(float r, float g, float b, float a);
    void setTextColor(float r, float g, float b, float a);
    void setTextScale(float scale);
    
    void setPressed(bool pressed) { m_pressed = pressed; }
    bool isPressed() const { return m_pressed; }
    
    void render(Renderer* renderer, TextRenderer* textRenderer);
    bool containsPoint(float x, float y) const;
    
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }
    
private:
    float m_x, m_y;
    float m_width, m_height;
    std::string m_text;
    float m_colorR, m_colorG, m_colorB, m_colorA;
    float m_pressedR, m_pressedG, m_pressedB, m_pressedA;
    float m_textR, m_textG, m_textB, m_textA;
    float m_textScale;
    bool m_pressed;
    float m_cornerRadius;
};

#endif // BUTTON_H

