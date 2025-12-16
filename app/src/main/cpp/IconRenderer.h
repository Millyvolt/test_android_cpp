#ifndef ICON_RENDERER_H
#define ICON_RENDERER_H

class Renderer;

enum class IconType {
    PLAY,
    PAUSE,
    PLUS,
    MINUS,
    CHECK,
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN
};

class IconRenderer {
public:
    IconRenderer();
    ~IconRenderer();
    
    bool initialize(Renderer* renderer);
    void cleanup();
    
    void drawIcon(float x, float y, IconType type, float size, float r, float g, float b, float a);
    
private:
    Renderer* m_renderer;
    
    void drawPlayIcon(float x, float y, float size, float r, float g, float b, float a);
    void drawPauseIcon(float x, float y, float size, float r, float g, float b, float a);
    void drawPlusIcon(float x, float y, float size, float r, float g, float b, float a);
    void drawMinusIcon(float x, float y, float size, float r, float g, float b, float a);
    void drawCheckIcon(float x, float y, float size, float r, float g, float b, float a);
    void drawArrowIcon(float x, float y, float size, float r, float g, float b, float a, float rotation);
};

#endif // ICON_RENDERER_H

