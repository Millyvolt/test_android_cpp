#include "Renderer.h"
#include <android/log.h>
#include <cmath>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Renderer", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Renderer", __VA_ARGS__))

static const char* vertexShaderSource = R"(
attribute vec4 a_position;
attribute vec4 a_color;
uniform mat4 u_matrix;
varying vec4 v_color;

void main() {
    gl_Position = u_matrix * a_position;
    v_color = a_color;
}
)";

static const char* fragmentShaderSource = R"(
precision mediump float;
varying vec4 v_color;

void main() {
    gl_FragColor = v_color;
}
)";

Renderer::Renderer()
    : m_window(nullptr)
    , m_display(EGL_NO_DISPLAY)
    , m_surface(EGL_NO_SURFACE)
    , m_context(EGL_NO_CONTEXT)
    , m_config(nullptr)
    , m_width(0)
    , m_height(0)
    , m_shaderProgram(0)
    , m_positionHandle(0)
    , m_colorHandle(0)
    , m_matrixHandle(0)
{
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize(ANativeWindow* window, int width, int height) {
    m_window = window;
    m_width = width;
    m_height = height;
    
    if (!initializeEGL()) {
        LOGE("Failed to initialize EGL");
        return false;
    }
    
    if (!createShaderProgram()) {
        LOGE("Failed to create shader program");
        cleanupEGL();
        return false;
    }
    
    LOGI("Renderer initialized: %d x %d", m_width, m_height);
    return true;
}

void Renderer::cleanup() {
    if (m_shaderProgram != 0) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }
    
    cleanupEGL();
}

void Renderer::onWindowResized(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Renderer::beginFrame() {
    if (m_display == EGL_NO_DISPLAY || m_surface == EGL_NO_SURFACE) {
        return;
    }
    
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::endFrame() {
    if (m_display != EGL_NO_DISPLAY && m_surface != EGL_NO_SURFACE) {
        eglSwapBuffers(m_display, m_surface);
    }
}

void Renderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::drawRect(float x, float y, float width, float height, float r, float g, float b, float a) {
    if (m_shaderProgram == 0) {
        return;
    }
    
    glUseProgram(m_shaderProgram);
    
    // Setup orthographic matrix
    float matrix[16];
    setupOrthographicMatrix(matrix, 0.0f, (float)m_width, (float)m_height, 0.0f);
    glUniformMatrix4fv(m_matrixHandle, 1, GL_FALSE, matrix);
    
    // Setup vertices for rectangle
    float vertices[] = {
        x, y, 0.0f,
        x + width, y, 0.0f,
        x + width, y + height, 0.0f,
        x, y + height, 0.0f
    };
    
    float colors[] = {
        r, g, b, a,
        r, g, b, a,
        r, g, b, a,
        r, g, b, a
    };
    
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    
    glVertexAttribPointer(m_positionHandle, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(m_positionHandle);
    
    glVertexAttribPointer(m_colorHandle, 4, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(m_colorHandle);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    
    glDisableVertexAttribArray(m_positionHandle);
    glDisableVertexAttribArray(m_colorHandle);
}

void Renderer::drawText(float x, float y, const char* text, float r, float g, float b, float a) {
    // Legacy method - kept for compatibility
    // Text rendering is now handled by TextRenderer class
    drawRect(x, y, 100.0f, 20.0f, r, g, b, a * 0.5f);
}

void Renderer::drawRoundedRect(float x, float y, float width, float height, float radius, float r, float g, float b, float a) {
    if (m_shaderProgram == 0) {
        return;
    }
    
    // For simplicity, draw as regular rectangle for now
    // Full rounded rectangle would require more complex geometry
    drawRect(x, y, width, height, r, g, b, a);
}

bool Renderer::initializeEGL() {
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }
    
    if (eglInitialize(m_display, nullptr, nullptr) == EGL_FALSE) {
        LOGE("eglInitialize failed");
        return false;
    }
    
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };
    
    EGLint numConfigs;
    if (eglChooseConfig(m_display, attribs, &m_config, 1, &numConfigs) == EGL_FALSE) {
        LOGE("eglChooseConfig failed");
        cleanupEGL();
        return false;
    }
    
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    m_context = eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, contextAttribs);
    if (m_context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        cleanupEGL();
        return false;
    }
    
    const EGLint surfaceAttribs[] = {
        EGL_NONE
    };
    
    m_surface = eglCreateWindowSurface(m_display, m_config, m_window, surfaceAttribs);
    if (m_surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        cleanupEGL();
        return false;
    }
    
    if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE) {
        LOGE("eglMakeCurrent failed");
        cleanupEGL();
        return false;
    }
    
    return true;
}

void Renderer::cleanupEGL() {
    if (m_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        
        if (m_context != EGL_NO_CONTEXT) {
            eglDestroyContext(m_display, m_context);
            m_context = EGL_NO_CONTEXT;
        }
        
        if (m_surface != EGL_NO_SURFACE) {
            eglDestroySurface(m_display, m_surface);
            m_surface = EGL_NO_SURFACE;
        }
        
        eglTerminate(m_display);
        m_display = EGL_NO_DISPLAY;
    }
}

bool Renderer::createShaderProgram() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    GLint compiled;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        LOGE("Vertex shader compilation failed: %s", infoLog);
        glDeleteShader(vertexShader);
        return false;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLchar infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        LOGE("Fragment shader compilation failed: %s", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);
    
    GLint linked;
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
        LOGE("Shader program linking failed: %s", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    m_positionHandle = glGetAttribLocation(m_shaderProgram, "a_position");
    m_colorHandle = glGetAttribLocation(m_shaderProgram, "a_color");
    m_matrixHandle = glGetUniformLocation(m_shaderProgram, "u_matrix");
    
    return true;
}

void Renderer::setupOrthographicMatrix(float* matrix, float left, float right, float bottom, float top) {
    float near = -1.0f;
    float far = 1.0f;
    
    matrix[0] = 2.0f / (right - left);
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;
    
    matrix[4] = 0.0f;
    matrix[5] = 2.0f / (top - bottom);
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;
    
    matrix[8] = 0.0f;
    matrix[9] = 0.0f;
    matrix[10] = -2.0f / (far - near);
    matrix[11] = 0.0f;
    
    matrix[12] = -(right + left) / (right - left);
    matrix[13] = -(top + bottom) / (top - bottom);
    matrix[14] = -(far + near) / (far - near);
    matrix[15] = 1.0f;
}

