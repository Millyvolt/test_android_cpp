#ifndef WORKOUT_TRACKER_H
#define WORKOUT_TRACKER_H

#include <string>
#include <vector>
#include <chrono>

class Renderer;
class TextRenderer;
class Button;

struct Exercise {
    std::string name;
    int sets;
    int reps;
    float weight; // in kg
    int restTime; // in seconds
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
    
    bool isPointInRect(float x, float y, float rectX, float rectY, float rectW, float rectH);

    TextRenderer * m_textRenderer;
    Button * m_startButton, * m_historyButton, * m_endButton, * m_chooseExerciseButton;
    bool m_debugMode;
    bool m_showingExerciseList;
    std::vector<std::string> m_availableExercises;
    double m_lastTouchX, m_lastTouchY;
};

#endif // WORKOUT_TRACKER_H

