#include "android_native_app_glue.h"
#include "App.h"
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "WorkoutTracker", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "WorkoutTracker", __VA_ARGS__))

static void handle_cmd(struct android_app* app, int32_t cmd) {
    App* workoutApp = static_cast<App*>(app->userData);
    if (workoutApp != nullptr) {
        workoutApp->handleCommand(cmd);
    }
}

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    App* workoutApp = static_cast<App*>(app->userData);
    if (workoutApp != nullptr) {
        return workoutApp->handleInput(event);
    }
    return 0;
}

void android_main(struct android_app* app) {
    app_dummy(); // Prevent compiler from stripping out native_app_glue
    
    LOGI("WorkoutTracker: Starting application");
    
    // Create application instance
    App workoutApp;
    app->userData = &workoutApp;
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;
    
    // Initialize application
    if (!workoutApp.initialize(app)) {
        LOGE("WorkoutTracker: Failed to initialize application");
        return;
    }
    
    // Main loop
    while (true) {
        int events;
        struct android_poll_source* source;
        
        // Process all pending events
        while (ALooper_pollAll(0, nullptr, &events, (void**)&source) >= 0) {
            if (source != nullptr) {
                source->process(app, source);
            }
            
            // Check if we are exiting
            if (app->destroyRequested != 0) {
                LOGI("WorkoutTracker: Destroy requested, cleaning up");
                workoutApp.cleanup();
                return;
            }
        }
        
        // Update and render
        workoutApp.update();
        workoutApp.render();
    }
}

