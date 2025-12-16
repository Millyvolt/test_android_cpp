#include "App.h"
#include <android/log.h>
#include <android/native_window.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "WorkoutTracker", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "WorkoutTracker", __VA_ARGS__))

App::App()
    : m_app(nullptr)
    , m_renderer(nullptr)
    , m_inputHandler(nullptr)
    , m_workoutTracker(nullptr)
    , m_initialized(false)
    , m_windowReady(false)
    , m_width(0)
    , m_height(0)
{
}

App::~App() {
    cleanup();
}

bool App::initialize(android_app* app) {
    if (m_initialized) {
        return true;
    }
    
    m_app = app;
    
    // Create workout tracker
    m_workoutTracker = new WorkoutTracker();
    if (!m_workoutTracker) {
        LOGE("Failed to create WorkoutTracker");
        return false;
    }
    
    // Create input handler
    m_inputHandler = new InputHandler();
    if (!m_inputHandler) {
        LOGE("Failed to create InputHandler");
        return false;
    }
    
    // Renderer will be created when window is ready
    m_renderer = nullptr;
    
    m_initialized = true;
    LOGI("App initialized successfully");
    return true;
}

void App::cleanup() {
    if (m_renderer) {
        delete m_renderer;
        m_renderer = nullptr;
    }
    
    if (m_inputHandler) {
        delete m_inputHandler;
        m_inputHandler = nullptr;
    }
    
    if (m_workoutTracker) {
        delete m_workoutTracker;
        m_workoutTracker = nullptr;
    }
    
    m_initialized = false;
    m_windowReady = false;
    LOGI("App cleaned up");
}

void App::update() {
    if (!m_initialized || !m_windowReady) {
        return;
    }
    
    // Update workout tracker
    if (m_workoutTracker) {
        m_workoutTracker->update();
    }
}

void App::render() {
    if (!m_initialized || !m_windowReady || !m_renderer) {
        return;
    }
    
    m_renderer->beginFrame();
    
    // Render workout tracker UI
    if (m_workoutTracker) {
        m_workoutTracker->render(m_renderer);
    }
    
    m_renderer->endFrame();
}

void App::handleCommand(int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW");
            processWindowCommand(cmd);
            break;
            
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW");
            processWindowCommand(cmd);
            break;
            
        case APP_CMD_WINDOW_RESIZED:
            LOGI("APP_CMD_WINDOW_RESIZED");
            if (m_app && m_app->window) {
                m_width = ANativeWindow_getWidth(m_app->window);
                m_height = ANativeWindow_getHeight(m_app->window);
                if (m_renderer) {
                    m_renderer->onWindowResized(m_width, m_height);
                }
            }
            break;
            
        case APP_CMD_GAINED_FOCUS:
            LOGI("APP_CMD_GAINED_FOCUS");
            break;
            
        case APP_CMD_LOST_FOCUS:
            LOGI("APP_CMD_LOST_FOCUS");
            break;
            
        case APP_CMD_PAUSE:
            LOGI("APP_CMD_PAUSE");
            break;
            
        case APP_CMD_RESUME:
            LOGI("APP_CMD_RESUME");
            break;
            
        case APP_CMD_START:
            LOGI("APP_CMD_START");
            break;
            
        case APP_CMD_STOP:
            LOGI("APP_CMD_STOP");
            break;
            
        case APP_CMD_DESTROY:
            LOGI("APP_CMD_DESTROY");
            cleanup();
            break;
            
        case APP_CMD_CONFIG_CHANGED:
            LOGI("APP_CMD_CONFIG_CHANGED");
            break;
            
        default:
            break;
    }
}

int32_t App::handleInput(AInputEvent* event) {
    if (!m_inputHandler || !m_workoutTracker) {
        return 0;
    }
    
    return m_inputHandler->handleEvent(event, m_workoutTracker);
}

void App::processWindowCommand(int32_t cmd) {
    if (cmd == APP_CMD_INIT_WINDOW) {
        if (m_app && m_app->window && !m_windowReady) {
            m_width = ANativeWindow_getWidth(m_app->window);
            m_height = ANativeWindow_getHeight(m_app->window);
            
            LOGI("Window initialized: %d x %d", m_width, m_height);
            
            // Create renderer
            if (!m_renderer) {
                m_renderer = new Renderer();
                if (m_renderer && m_renderer->initialize(m_app->window, m_width, m_height)) {
                    m_windowReady = true;
                    LOGI("Renderer initialized successfully");
                } else {
                    LOGE("Failed to initialize renderer");
                    delete m_renderer;
                    m_renderer = nullptr;
                }
            }
        }
    } else if (cmd == APP_CMD_TERM_WINDOW) {
        if (m_renderer) {
            delete m_renderer;
            m_renderer = nullptr;
        }
        m_windowReady = false;
        LOGI("Window terminated");
    }
}

