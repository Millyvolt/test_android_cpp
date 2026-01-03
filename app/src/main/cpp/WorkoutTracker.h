#ifndef WORKOUT_TRACKER_H
#define WORKOUT_TRACKER_H

#include <string>
#include <vector>
#include <chrono>

#define SELECT_EXERCISE         "SELECT EXERCISE"   // "ВЫБОР УПРАЖНЕНИЯ"
#define REPS_INCR_BUT_TEXT      "+"  /*"↑"*/
#define REPS_DECR_BUT_TEXT      "-"  /*"↓"*/
#define BUT_LIT_DELAY_MS        30

class Renderer;
class TextRenderer;
class Button;

struct Set {
    int reps;
    float weight; // in kg
    bool completed;
    
    Set() : reps(0), weight(0.0f), completed(false) {}
    Set(int r, float w) : reps(r), weight(w), completed(false) {}
};

struct Exercise {
    std::string name;
    std::vector<Set> sets;
    int defaultReps; // default reps for new sets
    float defaultWeight; // default weight for new sets
    int restTime; // in seconds
    
    Exercise() : defaultReps(0), defaultWeight(0.0f), restTime(60) {}
};

struct Workout {
    std::string name;
    std::vector<Exercise> exercises;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    bool isActive;
};

class WorkoutTracker {
public:
    WorkoutTracker();
    ~WorkoutTracker();
    
    void update();
    void render(Renderer* renderer);
    
    // Input handling
    void onTouchDown(float x, float y);
    void onTouchUp(float x, float y);
    void onTouchMove(float x, float y, float dx, float dy);
    void onTouchCancel();
    void onBackPressed();
    
    // Workout management
    void startWorkout(const std::string& name);
    void endWorkout();
    void addExercise(const std::string& name, int sets, int reps, float weight);
    void completeSet(int exerciseIndex);
    
    // Getters
    bool isWorkoutActive() const { return m_currentWorkout.isActive; }
    const Workout& getCurrentWorkout() const { return m_currentWorkout; }
    int getElapsedSeconds() const;
    
    // Bottom inset setter for navigation bar
    void setBottomInset(int inset) { m_bottomInset = (float)inset; }
    
private:
    Workout m_currentWorkout;
    std::vector<Workout> m_workoutHistory;
    
    int m_currentExerciseIndex;
    int m_currentSetIndex;
    
    float m_screenWidth;
    float m_screenHeight;
    float m_bottomInset;
    
    void updateButtonLayouts();
    void renderMainScreen(Renderer* renderer);
    void renderWorkoutScreen(Renderer* renderer);
    void renderExerciseList(Renderer* renderer);
    void renderExerciseSelectionList(Renderer* renderer);
    void renderDebugOverlay(Renderer* renderer);
    
    void showExerciseSelectionList();
    void hideExerciseSelectionList();
    
    void addSetToExercise(int exerciseIndex);
    void markSetCompleted(int exerciseIndex, int setIndex);
    int getCompletedSetsCount(int exerciseIndex) const;
    
    bool isPointInRect(float x, float y, float rectX, float rectY, float rectW, float rectH);

    TextRenderer * m_textRenderer;
    Button * m_startButton, * m_historyButton, * m_endButton, * m_chooseExerciseButton, * m_addSetButton;
    Button * m_repsIncrementButton, * m_repsDecrementButton;
    bool m_debugMode;
    bool m_showingExerciseList;
    std::vector<std::string> m_availableExercises;
    double m_lastTouchX, m_lastTouchY;
    
    // Button press state tracking
    std::chrono::system_clock::time_point m_buttonPressTime;
    Button* m_lastPressedButton;
    bool m_buttonPressPending;
};

#endif // WORKOUT_TRACKER_H

