#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <android/input.h>

class WorkoutTracker;

class InputHandler {
public:
    InputHandler();
    ~InputHandler();
    
    int32_t handleEvent(AInputEvent* event, WorkoutTracker* tracker);
    
private:
    void handleTouchEvent(AInputEvent* event, WorkoutTracker* tracker);
    void handleKeyEvent(AInputEvent* event, WorkoutTracker* tracker);
    
    float m_lastTouchX;
    float m_lastTouchY;
    bool m_touching;
};

#endif // INPUT_HANDLER_H

