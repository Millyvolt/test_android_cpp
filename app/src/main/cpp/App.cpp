#include "App.h"
#include <android/log.h>
#include <android/native_window.h>
#include <jni.h>

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
    , m_bottomInset(0)
    , m_javaVM(nullptr)
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
    
    // JavaVM will be retrieved lazily when needed (not during initialization)
    // This avoids crashes if activity->env is null at this point
    
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
            processWindowCommand(cmd);
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

JNIEnv* App::getJNIEnv() {
    // If we don't have JavaVM, we can't get JNIEnv
    // We don't try to get JavaVM from activity->env here because
    // activity->env is only valid in specific callback contexts and
    // accessing it at the wrong time can cause crashes.
    // Instead, we'll just use the fallback value (60 pixels).
    if (!m_javaVM) {
        return nullptr;
    }
    
    // Get JNIEnv from JavaVM
    JNIEnv* env = nullptr;
    int status = m_javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    
    if (status == JNI_EDETACHED) {
        // Thread not attached, attach it
        JavaVMAttachArgs args;
        args.version = JNI_VERSION_1_6;
        args.name = "WorkoutTrackerThread";
        args.group = nullptr;
        
        if (m_javaVM->AttachCurrentThread(&env, &args) != JNI_OK) {
            LOGE("Failed to attach thread to JavaVM");
            return nullptr;
        }
    } else if (status != JNI_OK) {
        LOGE("Failed to get JNIEnv, status: %d", status);
        return nullptr;
    }
    
    return env;
}

void App::updateBottomInset() {
    // Default fallback value (typically 48-56dp, ~60-80px on most devices)
    m_bottomInset = 60;
    
    if (!m_app || !m_app->activity) {
        LOGI("Using fallback bottom inset: %d (no app/activity)", m_bottomInset);
        return;
    }
    
    // Get safe JNIEnv by attaching thread if needed
    JNIEnv* env = getJNIEnv();
    if (!env) {
        LOGI("Using fallback bottom inset: %d (no JNIEnv)", m_bottomInset);
        return;
    }
    
    // Check for exceptions before proceeding
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE("JNI exception detected, using fallback");
        return;
    }
    
    // Double-check activity is still valid before accessing clazz
    if (!m_app->activity) {
        LOGI("Using fallback bottom inset: %d (activity became null)", m_bottomInset);
        return;
    }
    
    jobject activityObj = m_app->activity->clazz;
    if (!activityObj) {
        LOGI("Using fallback bottom inset: %d (no activity object)", m_bottomInset);
        return;
    }
    
    // Get the activity class
    jclass activityClass = env->GetObjectClass(activityObj);
    if (!activityClass || env->ExceptionCheck()) {
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        if (activityClass) {
            env->DeleteLocalRef(activityClass);
        }
        LOGI("Using fallback bottom inset: %d (failed to get activity class)", m_bottomInset);
        return;
    }
    
    // Get the getWindow() method
    jmethodID getWindowMethod = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");
    if (!getWindowMethod || env->ExceptionCheck()) {
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        env->DeleteLocalRef(activityClass);
        LOGI("Using fallback bottom inset: %d (failed to get getWindow method)", m_bottomInset);
        return;
    }
    
    // Call getWindow()
    jobject window = env->CallObjectMethod(activityObj, getWindowMethod);
    if (!window || env->ExceptionCheck()) {
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        env->DeleteLocalRef(activityClass);
        LOGI("Using fallback bottom inset: %d (failed to get window)", m_bottomInset);
        return;
    }
    
    // Get Window class
    jclass windowClass = env->GetObjectClass(window);
    if (!windowClass || env->ExceptionCheck()) {
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        env->DeleteLocalRef(window);
        env->DeleteLocalRef(activityClass);
        LOGI("Using fallback bottom inset: %d (failed to get window class)", m_bottomInset);
        return;
    }
    
    // Get getDecorView() method
    jmethodID getDecorViewMethod = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");
    if (!getDecorViewMethod || env->ExceptionCheck()) {
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        env->DeleteLocalRef(windowClass);
        env->DeleteLocalRef(window);
        env->DeleteLocalRef(activityClass);
        LOGI("Using fallback bottom inset: %d (failed to get getDecorView method)", m_bottomInset);
        return;
    }
    
    // Call getDecorView()
    jobject decorView = env->CallObjectMethod(window, getDecorViewMethod);
    if (!decorView || env->ExceptionCheck()) {
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        env->DeleteLocalRef(windowClass);
        env->DeleteLocalRef(window);
        env->DeleteLocalRef(activityClass);
        LOGI("Using fallback bottom inset: %d (failed to get decor view)", m_bottomInset);
        return;
    }
    
    // Get View class
    jclass viewClass = env->GetObjectClass(decorView);
    if (!viewClass || env->ExceptionCheck()) {
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        env->DeleteLocalRef(decorView);
        env->DeleteLocalRef(windowClass);
        env->DeleteLocalRef(window);
        env->DeleteLocalRef(activityClass);
        LOGI("Using fallback bottom inset: %d (failed to get view class)", m_bottomInset);
        return;
    }
    
    // Try to get root window insets (API 23+)
    jmethodID getRootWindowInsetsMethod = env->GetMethodID(viewClass, "getRootWindowInsets", "()Landroid/view/WindowInsets;");
    if (getRootWindowInsetsMethod && !env->ExceptionCheck()) {
        jobject windowInsets = env->CallObjectMethod(decorView, getRootWindowInsetsMethod);
        if (windowInsets && !env->ExceptionCheck()) {
            jclass windowInsetsClass = env->GetObjectClass(windowInsets);
            if (windowInsetsClass && !env->ExceptionCheck()) {
                // Get getSystemWindowInsetsBottom() method (API 21+)
                jmethodID getSystemWindowInsetsBottomMethod = env->GetMethodID(
                    windowInsetsClass, "getSystemWindowInsetsBottom", "()I");
                if (getSystemWindowInsetsBottomMethod && !env->ExceptionCheck()) {
                    jint bottomInset = env->CallIntMethod(windowInsets, getSystemWindowInsetsBottomMethod);
                    if (!env->ExceptionCheck() && bottomInset > 0) {
                        m_bottomInset = (int)bottomInset;
                        env->DeleteLocalRef(windowInsetsClass);
                        env->DeleteLocalRef(windowInsets);
                        env->DeleteLocalRef(viewClass);
                        env->DeleteLocalRef(decorView);
                        env->DeleteLocalRef(windowClass);
                        env->DeleteLocalRef(window);
                        env->DeleteLocalRef(activityClass);
                        LOGI("Bottom inset retrieved via JNI: %d", m_bottomInset);
                        return;
                    } else if (env->ExceptionCheck()) {
                        env->ExceptionClear();
                    }
                } else if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                }
                env->DeleteLocalRef(windowInsetsClass);
            } else if (env->ExceptionCheck()) {
                env->ExceptionClear();
            }
            env->DeleteLocalRef(windowInsets);
        } else if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
    } else if (env->ExceptionCheck()) {
        env->ExceptionClear();
    }
    
    // Cleanup and fallback
    env->DeleteLocalRef(viewClass);
    env->DeleteLocalRef(decorView);
    env->DeleteLocalRef(windowClass);
    env->DeleteLocalRef(window);
    env->DeleteLocalRef(activityClass);
    
    LOGI("Using fallback bottom inset: %d", m_bottomInset);
}

void App::processWindowCommand(int32_t cmd) {
    if (cmd == APP_CMD_INIT_WINDOW) {
        if (m_app && m_app->window && !m_windowReady) {
            m_width = ANativeWindow_getWidth(m_app->window);
            m_height = ANativeWindow_getHeight(m_app->window);
            
            LOGI("Window initialized: %d x %d", m_width, m_height);
            
            // Update bottom inset when window is ready
            updateBottomInset();
            
            // Create renderer
            if (!m_renderer) {
                m_renderer = new Renderer();
                if (m_renderer && m_renderer->initialize(m_app->window, m_width, m_height)) {
                    m_windowReady = true;
                    LOGI("Renderer initialized successfully");
                    
                    // Pass bottom inset to workout tracker
                    if (m_workoutTracker) {
                        m_workoutTracker->setBottomInset(m_bottomInset);
                    }
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
    } else if (cmd == APP_CMD_WINDOW_RESIZED) {
        // Update bottom inset when window is resized (e.g., navigation bar visibility changes)
        updateBottomInset();
        if (m_workoutTracker) {
            m_workoutTracker->setBottomInset(m_bottomInset);
        }
    }
}

