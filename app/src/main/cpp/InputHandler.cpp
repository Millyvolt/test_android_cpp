#include "InputHandler.h"
#include "WorkoutTracker.h"
#include <android/log.h>
#include <cmath>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "InputHandler", __VA_ARGS__))

InputHandler::InputHandler()
    : m_lastTouchX(0.0f)
    , m_lastTouchY(0.0f)
    , m_touching(false)
{
}

InputHandler::~InputHandler() {
}

int32_t InputHandler::handleEvent(AInputEvent* event, WorkoutTracker* tracker) {
    if (!event || !tracker) {
        return 0;
    }
    
    int32_t eventType = AInputEvent_getType(event);
    
    switch (eventType) {
        case AINPUT_EVENT_TYPE_MOTION:
            handleTouchEvent(event, tracker);
            return 1;
            
        case AINPUT_EVENT_TYPE_KEY:
            handleKeyEvent(event, tracker);
            return 1;
            
        default:
            return 0;
    }
}

void InputHandler::handleTouchEvent(AInputEvent* event, WorkoutTracker* tracker) {
    int32_t action = AMotionEvent_getAction(event);
    float x = AMotionEvent_getX(event, 0);
    float y = AMotionEvent_getY(event, 0);
    
    switch (action & AMOTION_EVENT_ACTION_MASK) {
        case AMOTION_EVENT_ACTION_DOWN:
            m_touching = true;
            m_lastTouchX = x;
            m_lastTouchY = y;
            tracker->onTouchDown(x, y);
            break;
            
        case AMOTION_EVENT_ACTION_UP:
            m_touching = false;
            tracker->onTouchUp(x, y);
            break;
            
        case AMOTION_EVENT_ACTION_MOVE:
            if (m_touching) {
                float dx = x - m_lastTouchX;
                float dy = y - m_lastTouchY;
                tracker->onTouchMove(x, y, dx, dy);
                m_lastTouchX = x;
                m_lastTouchY = y;
            }
            break;
            
        case AMOTION_EVENT_ACTION_CANCEL:
            m_touching = false;
            tracker->onTouchCancel();
            break;
            
        default:
            break;
    }
}

void InputHandler::handleKeyEvent(AInputEvent* event, WorkoutTracker* tracker) {
    int32_t action = AKeyEvent_getAction(event);
    int32_t keyCode = AKeyEvent_getKeyCode(event);
    
    if (action == AKEY_EVENT_ACTION_DOWN) {
        switch (keyCode) {
            case AKEYCODE_BACK:
                tracker->onBackPressed();
                break;
                
            default:
                break;
        }
    }
}

