#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <string>
#include <vector>

class Renderer;

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    
    bool initialize(Renderer* renderer);
    void cleanup();
    
    void drawText(float x, float y, const std::string& text, float r, float g, float b, float a, float scale = 1.0f);
    void drawNumber(float x, float y, int number, float r, float g, float b, float a, float scale = 1.0f);
    void drawTime(float x, float y, int seconds, float r, float g, float b, float a, float scale = 1.0f);
    
    float getTextWidth(const std::string& text, float scale = 1.0f) const;
    float getTextHeight(float scale = 1.0f) const;
    
private:
    Renderer* m_renderer;
    float m_charWidth;
    float m_charHeight;
    
    void drawChar(char c, float x, float y, float r, float g, float b, float a, float scale);
    void getCharVertices(char c, float x, float y, float scale, float* vertices);
};

#endif // TEXT_RENDERER_H

