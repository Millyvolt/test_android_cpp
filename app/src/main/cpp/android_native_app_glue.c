/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is a simplified version of android_native_app_glue.c
 * For full implementation, use the one from Android NDK
 */

#include "android_native_app_glue.h"
#include <jni.h>
#include <android/log.h>
#include <android/input.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native-activity", __VA_ARGS__))

static void process_cmd(struct android_app* app, struct android_poll_source* source) {
    int8_t cmd;
    (void)source;
    if (read(app->msgread, &cmd, sizeof(cmd)) == sizeof(cmd)) {
        if (app->onAppCmd != NULL) {
            app->onAppCmd(app, cmd);
        }
    }
    if (app->msgread != app->msgwrite) {
        ALooper_wake(app->looper);
    }
}

static void process_input(struct android_app* app, struct android_poll_source* source) {
    (void)source;
    AInputEvent* event = NULL;
    while (AInputQueue_getEvent(app->inputQueue, &event) >= 0) {
        if (AInputQueue_preDispatchEvent(app->inputQueue, event)) {
            continue;
        }
        int32_t handled = 0;
        if (app->onInputEvent != NULL) {
            handled = app->onInputEvent(app, event);
        }
        AInputQueue_finishEvent(app->inputQueue, event, handled);
    }
}

static int input_looper_callback(int fd, int events, void* data) {
    (void)fd; (void)events;

    struct android_poll_source* source = (struct android_poll_source*)data;
    if (source != NULL && source->app != NULL) {
        if (source->app->destroyRequested) {
            return -1;
        }
        source->process(source->app, source);
    }
    return 1;
}

static void* android_app_entry(void* param) {
    struct android_app* app = (struct android_app*)param;
    
    app->config = AConfiguration_new();
    AConfiguration_fromAssetManager(app->config, app->activity->assetManager);
    
    app->cmdPollSource.id = LOOPER_ID_MAIN;
    app->cmdPollSource.app = app;
    app->cmdPollSource.process = process_cmd;
    
    app->inputPollSource.id = LOOPER_ID_INPUT;
    app->inputPollSource.app = app;
    app->inputPollSource.process = process_input;
    
    ALooper* looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    ALooper_addFd(looper, app->msgread, LOOPER_ID_MAIN, ALOOPER_EVENT_INPUT, NULL, &app->cmdPollSource);
    app->looper = looper;
    
    // Set app pointer in poll sources for callback access
    app->cmdPollSource.app = app;
    app->inputPollSource.app = app;
    
    pthread_mutex_lock(&app->mutex);
    app->running = 1;
    pthread_cond_broadcast(&app->cond);
    pthread_mutex_unlock(&app->mutex);
    
    android_main(app);
    
    app->destroyRequested = 1;
    return NULL;
}

static void android_app_write_cmd(struct android_app* android_app, int8_t cmd) {
    if (write(android_app->msgwrite, &cmd, sizeof(cmd)) != sizeof(cmd)) {
        LOGE("Failure writing android_app cmd: %s\n", strerror(errno));
    }
    if (android_app->looper != NULL) {
        ALooper_wake(android_app->looper);
    }
}

static void android_app_set_input(struct android_app* android_app, AInputQueue* inputQueue) {
    pthread_mutex_lock(&android_app->mutex);
    android_app->pendingInputQueue = inputQueue;
    android_app_write_cmd(android_app, APP_CMD_INPUT_CHANGED);
    android_app->inputQueue = inputQueue;
    pthread_cond_broadcast(&android_app->cond);
    pthread_mutex_unlock(&android_app->mutex);
    
    if (inputQueue != NULL) {
        AInputQueue_attachLooper(inputQueue, android_app->looper, LOOPER_ID_INPUT, input_looper_callback, &android_app->inputPollSource);
    }
}

static void android_app_set_window(struct android_app* android_app, ANativeWindow* window) {
    pthread_mutex_lock(&android_app->mutex);
    if (android_app->pendingWindow != NULL) {
        android_app_write_cmd(android_app, APP_CMD_TERM_WINDOW);
    }
    android_app->pendingWindow = window;
    if (window != NULL) {
        android_app_write_cmd(android_app, APP_CMD_INIT_WINDOW);
    }
    android_app->window = window;
    pthread_cond_broadcast(&android_app->cond);
    pthread_mutex_unlock(&android_app->mutex);
}

static void android_app_set_activity_state(struct android_app* android_app, int8_t cmd) {
    pthread_mutex_lock(&android_app->mutex);
    android_app_write_cmd(android_app, cmd);
    android_app->activityState = cmd;
    pthread_cond_broadcast(&android_app->cond);
    pthread_mutex_unlock(&android_app->mutex);
}

static void onDestroy(ANativeActivity* activity) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_write_cmd(android_app, APP_CMD_DESTROY);
}

static void onStart(ANativeActivity* activity) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_set_activity_state(android_app, APP_CMD_START);
}

static void onResume(ANativeActivity* activity) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_set_activity_state(android_app, APP_CMD_RESUME);
}

static void* onSaveInstanceState(ANativeActivity* activity, size_t* outLen) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    void* savedState = NULL;
    
    pthread_mutex_lock(&android_app->mutex);
    android_app->stateSaved = 0;
    android_app_write_cmd(android_app, APP_CMD_SAVE_STATE);
    // Wait a bit for state to be saved, but don't block indefinitely
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 1;
    while (!android_app->stateSaved) {
        if (pthread_cond_timedwait(&android_app->cond, &android_app->mutex, &timeout) != 0) {
            break;
        }
    }
    
    if (android_app->savedState != NULL) {
        savedState = android_app->savedState;
        *outLen = android_app->savedStateSize;
        android_app->savedState = NULL;
        android_app->savedStateSize = 0;
    }
    
    android_app->stateSaved = 1;
    pthread_mutex_unlock(&android_app->mutex);
    return savedState;
}

static void onPause(ANativeActivity* activity) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_set_activity_state(android_app, APP_CMD_PAUSE);
}

static void onStop(ANativeActivity* activity) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_set_activity_state(android_app, APP_CMD_STOP);
}

static void onConfigurationChanged(ANativeActivity* activity) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_write_cmd(android_app, APP_CMD_CONFIG_CHANGED);
}

static void onLowMemory(ANativeActivity* activity) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_write_cmd(android_app, APP_CMD_LOW_MEMORY);
}

static void onWindowFocusChanged(ANativeActivity* activity, int focused) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_write_cmd(android_app, focused ? APP_CMD_GAINED_FOCUS : APP_CMD_LOST_FOCUS);
}

static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_set_window(android_app, window);
}

static void onNativeWindowResized(ANativeActivity* activity, ANativeWindow* window) {
    (void)window;
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_write_cmd(android_app, APP_CMD_WINDOW_RESIZED);
}

static void onNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window) {
    (void)window;
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_write_cmd(android_app, APP_CMD_WINDOW_REDRAW_NEEDED);
}

static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) {
    (void)window;
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_set_window(android_app, NULL);
}

static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_set_input(android_app, queue);
}

static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue) {
    (void)queue;
    struct android_app* android_app = (struct android_app*)activity->instance;
    android_app_set_input(android_app, NULL);
}

void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    struct android_app* android_app = (struct android_app*)malloc(sizeof(struct android_app));
    memset(android_app, 0, sizeof(struct android_app));
    activity->instance = android_app;
    
    android_app->activity = activity;
    android_app->savedState = savedState;
    android_app->savedStateSize = savedStateSize;
    
    pthread_mutex_init(&android_app->mutex, NULL);
    pthread_cond_init(&android_app->cond, NULL);
    
    int msgpipe[2];
    if (pipe(msgpipe)) {
        LOGE("could not create pipe: %s", strerror(errno));
        return;
    }
    android_app->msgread = msgpipe[0];
    android_app->msgwrite = msgpipe[1];
    
    activity->callbacks->onDestroy = onDestroy;
    activity->callbacks->onStart = onStart;
    activity->callbacks->onResume = onResume;
    activity->callbacks->onSaveInstanceState = onSaveInstanceState;
    activity->callbacks->onPause = onPause;
    activity->callbacks->onStop = onStop;
    activity->callbacks->onConfigurationChanged = onConfigurationChanged;
    activity->callbacks->onLowMemory = onLowMemory;
    activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    activity->callbacks->onNativeWindowResized = onNativeWindowResized;
    activity->callbacks->onNativeWindowRedrawNeeded = onNativeWindowRedrawNeeded;
    activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    activity->callbacks->onInputQueueCreated = onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;
    
    android_app->config = AConfiguration_new();
    AConfiguration_fromAssetManager(android_app->config, activity->assetManager);
    
    pthread_create(&android_app->thread, NULL, android_app_entry, android_app);
    
    pthread_mutex_lock(&android_app->mutex);
    while (!android_app->running) {
        pthread_cond_wait(&android_app->cond, &android_app->mutex);
    }
    pthread_mutex_unlock(&android_app->mutex);
}

void app_dummy() {
    // Prevent compiler from stripping out native_app_glue
}

