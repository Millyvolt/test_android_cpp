#ifndef ANDROID_NATIVE_APP_GLUE_H
#define ANDROID_NATIVE_APP_GLUE_H

#include <android/looper.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/configuration.h>

#ifdef __cplusplus
extern "C" {
#endif

#define APP_CMD_INPUT_CHANGED 0
#define APP_CMD_INIT_WINDOW 1
#define APP_CMD_TERM_WINDOW 2
#define APP_CMD_WINDOW_RESIZED 3
#define APP_CMD_WINDOW_REDRAW_NEEDED 4
#define APP_CMD_CONTENT_RECT_CHANGED 5
#define APP_CMD_GAINED_FOCUS 6
#define APP_CMD_LOST_FOCUS 7
#define APP_CMD_CONFIG_CHANGED 8
#define APP_CMD_LOW_MEMORY 9
#define APP_CMD_START 10
#define APP_CMD_RESUME 11
#define APP_CMD_SAVE_STATE 12
#define APP_CMD_PAUSE 13
#define APP_CMD_STOP 14
#define APP_CMD_DESTROY 15

#define LOOPER_ID_MAIN 1
#define LOOPER_ID_INPUT 2

struct android_poll_source {
    int32_t id;
    struct android_app* app;
    void (*process)(struct android_app* app, struct android_poll_source* source);
};

struct android_app {
    void* userData;
    void (*onAppCmd)(struct android_app* app, int32_t cmd);
    int32_t (*onInputEvent)(struct android_app* app, AInputEvent* event);
    
    ANativeActivity* activity;
    AConfiguration* config;
    
    void* savedState;
    size_t savedStateSize;
    
    ALooper* looper;
    AInputQueue* inputQueue;
    ANativeWindow* window;
    ANativeWindow* pendingWindow;
    AInputQueue* pendingInputQueue;
    
    int activityState;
    int destroyRequested;
    
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    
    int msgread;
    int msgwrite;
    
    pthread_t thread;
    struct android_poll_source cmdPollSource;
    struct android_poll_source inputPollSource;
    int running;
    int stateSaved;
    int destroyed;
    int redrawNeeded;
};

void android_main(struct android_app* app);
void app_dummy();

#ifdef __cplusplus
}
#endif

#endif // ANDROID_NATIVE_APP_GLUE_H

