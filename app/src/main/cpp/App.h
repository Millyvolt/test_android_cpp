#ifndef APP_H
#define APP_H

#include "android_native_app_glue.h"
#include "Renderer.h"
#include "InputHandler.h"
#include "WorkoutTracker.h"

class App {
public:
    App();
    ~App();
    
    bool initialize(android_app* app);
    void cleanup();
    void update();
    void render();
    void handleCommand(int32_t cmd);
    int32_t handleInput(AInputEvent* event);
    
private:
    android_app* m_app;
    Renderer* m_renderer;
    InputHandler* m_inputHandler;
    WorkoutTracker* m_workoutTracker;
    
    bool m_initialized;
    bool m_windowReady;
    int m_width;
    int m_height;
    
    void processWindowCommand(int32_t cmd);
};

#endif // APP_H

