#ifndef RENDERER_H
#define RENDERER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool initialize(ANativeWindow* window, int width, int height);
    void cleanup();
    void onWindowResized(int width, int height);
    
    void beginFrame();
    void endFrame();
    
    void clear(float r, float g, float b, float a);
    void drawRect(float x, float y, float width, float height, float r, float g, float b, float a);
    void drawRoundedRect(float x, float y, float width, float height, float radius, float r, float g, float b, float a);
    void drawText(float x, float y, const char* text, float r, float g, float b, float a);
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
private:
    ANativeWindow* m_window;
    EGLDisplay m_display;
    EGLSurface m_surface;
    EGLContext m_context;
    EGLConfig m_config;
    
    int m_width;
    int m_height;
    
    GLuint m_shaderProgram;
    GLuint m_positionHandle;
    GLuint m_colorHandle;
    GLuint m_matrixHandle;
    
    bool initializeEGL();
    void cleanupEGL();
    bool createShaderProgram();
    void setupOrthographicMatrix(float* matrix, float left, float right, float bottom, float top);
};

#endif // RENDERER_H

