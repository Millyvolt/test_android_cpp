#include "WorkoutTracker.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "Button.h"
#include "Layout.h"
#include <android/log.h>
#include <sstream>
#include <iomanip>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "WorkoutTracker", __VA_ARGS__))

WorkoutTracker::WorkoutTracker()
    : m_currentExerciseIndex(0)
    , m_currentSetIndex(0)
    , m_screenWidth(0.0f)
    , m_screenHeight(0.0f)
    , m_textRenderer(nullptr)
    , m_startButton(nullptr)
    , m_historyButton(nullptr)
    , m_endButton(nullptr)
    , m_debugMode(false)
    , m_lastTouchX(0.0f)
    , m_lastTouchY(0.0f)
{
    m_currentWorkout.isActive = false;
    m_textRenderer = new TextRenderer();
    
    m_startButton = new Button();
    m_startButton->setText("START WORKOUT");
    m_startButton->setColor(0.2f, 0.6f, 0.3f, 1.0f);
    m_startButton->setPressedColor(0.3f, 0.7f, 0.4f, 1.0f);
    m_startButton->setTextScale(5.0f);
    
    m_historyButton = new Button();
    m_historyButton->setText("HISTORY");
    m_historyButton->setColor(0.4f, 0.4f, 0.4f, 1.0f);
    m_historyButton->setPressedColor(0.5f, 0.5f, 0.5f, 1.0f);
    m_historyButton->setTextScale(5.0f);
    
    m_endButton = new Button();
    m_endButton->setText("END WORKOUT");
    m_endButton->setColor(0.6f, 0.2f, 0.2f, 1.0f);
    m_endButton->setPressedColor(0.7f, 0.3f, 0.3f, 1.0f);
    m_endButton->setTextScale(5.0f);

    (void)m_debugMode;
}

WorkoutTracker::~WorkoutTracker() {
}

void WorkoutTracker::update() {
    // Update workout timer, etc.
}

void WorkoutTracker::render(Renderer* renderer) {
    if (!renderer) {
        return;
    }
    
    m_screenWidth = (float)renderer->getWidth();
    m_screenHeight = (float)renderer->getHeight();
    
    // Initialize text renderer if needed
    if (m_textRenderer && !m_textRenderer->initialize(renderer)) {
        // Text renderer initialization failed, but continue without text
    }
    
    // Update button positions based on screen size
    updateButtonLayouts();
    
    // Clear screen with dark background
    renderer->clear(0.1f, 0.1f, 0.15f, 1.0f);
    
    if (m_currentWorkout.isActive) {
        renderWorkoutScreen(renderer);
    } else {
        renderMainScreen(renderer);
    }
    
    if (m_debugMode) {
        renderDebugOverlay(renderer);
    }
}

void WorkoutTracker::updateButtonLayouts() {
    if (!m_startButton || !m_historyButton || !m_endButton) return;
    
    if (!m_currentWorkout.isActive) {
        // Main screen layout with proper spacing
        float buttonWidth = m_screenWidth - (Layout::MARGIN_LARGE * 2);
        float buttonY = Layout::centerY(Layout::BUTTON_HEIGHT_LARGE * 2 + Layout::SPACING_MEDIUM, m_screenHeight);
        
        m_startButton->setBounds(Layout::MARGIN_LARGE, buttonY, buttonWidth, Layout::BUTTON_HEIGHT_LARGE);
        
        float historyButtonY = buttonY + Layout::BUTTON_HEIGHT_LARGE + Layout::SPACING_MEDIUM;
        m_historyButton->setBounds(Layout::MARGIN_LARGE, historyButtonY, buttonWidth, Layout::BUTTON_HEIGHT_LARGE);
    } else {
        // Workout screen layout
        float endButtonY = m_screenHeight - Layout::MARGIN_LARGE - Layout::BUTTON_HEIGHT;
        float buttonWidth = m_screenWidth - (Layout::MARGIN_LARGE * 2);
        m_endButton->setBounds(Layout::MARGIN_LARGE, endButtonY, buttonWidth, Layout::BUTTON_HEIGHT);
    }
}

void WorkoutTracker::renderMainScreen(Renderer* renderer) {
    // Title with proper margins
    float titleY = Layout::MARGIN_MEDIUM;
    float titleWidth = m_screenWidth - (Layout::MARGIN_SMALL * 2);
    renderer->drawRect(Layout::MARGIN_SMALL, titleY, titleWidth, Layout::TITLE_HEIGHT, 0.2f, 0.4f, 0.6f, 1.0f);
    if (m_textRenderer) {
        float titleTextWidth = m_textRenderer->getTextWidth("WORKOUT TRACKER", 1.5f);
        float titleTextX = Layout::centerTextX("WORKOUT TRACKER", titleTextWidth, m_screenWidth);
        m_textRenderer->drawText(titleTextX, titleY + Layout::PADDING_MEDIUM, "WORKOUT TRACKER", 1.0f, 1.0f, 1.0f, 1.0f, 1.5f);
    }
    
    // Render buttons
    if (m_startButton) {
        m_startButton->render(renderer, m_textRenderer);
    }
    if (m_historyButton) {
        m_historyButton->render(renderer, m_textRenderer);
    }
}

void WorkoutTracker::renderWorkoutScreen(Renderer* renderer) {
    // Header with workout name and timer
    renderer->drawRect(0.0f, 0.0f, m_screenWidth, Layout::HEADER_HEIGHT, 0.2f, 0.3f, 0.5f, 1.0f);
    
    // Timer display
    int elapsed = getElapsedSeconds();
    if (m_textRenderer) {
        float timerTextWidth = m_textRenderer->getTextWidth("00:00", 2.0f);
        float timerX = Layout::centerTextX("00:00", timerTextWidth, m_screenWidth);
        m_textRenderer->drawTime(timerX, Layout::PADDING_LARGE + 10.0f, elapsed, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f);
        
        // Workout name
        float nameX = Layout::MARGIN_MEDIUM;
        m_textRenderer->drawText(nameX, Layout::PADDING_SMALL, m_currentWorkout.name, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    }
    
    // Exercise list area with proper spacing
    float listY = Layout::HEADER_HEIGHT + Layout::SPACING_MEDIUM;
    float listHeight = m_screenHeight - listY - Layout::BUTTON_HEIGHT - Layout::MARGIN_LARGE - Layout::SPACING_MEDIUM;
    float listWidth = m_screenWidth - (Layout::MARGIN_SMALL * 2);
    renderer->drawRect(Layout::MARGIN_SMALL, listY, listWidth, listHeight, 0.15f, 0.15f, 0.2f, 1.0f);
    
    // Render exercises
    renderExerciseList(renderer);
    
    // End workout button at bottom
    if (m_endButton) {
        m_endButton->render(renderer, m_textRenderer);
    }
}

void WorkoutTracker::renderExerciseList(Renderer* renderer) {
    if (m_currentWorkout.exercises.empty()) {
        if (m_textRenderer) {
            float noExTextWidth = m_textRenderer->getTextWidth("NO EXERCISES", 1.0f);
            float noExTextX = Layout::centerTextX("NO EXERCISES", noExTextWidth, m_screenWidth);
            m_textRenderer->drawText(noExTextX, Layout::centerY(0, m_screenHeight), "NO EXERCISES", 0.7f, 0.7f, 0.7f, 1.0f, 1.0f);
        }
        return;
    }
    
    float listStartY = Layout::HEADER_HEIGHT + Layout::SPACING_MEDIUM + Layout::PADDING_SMALL;
    float itemX = Layout::MARGIN_MEDIUM;
    float itemWidth = m_screenWidth - (Layout::MARGIN_MEDIUM * 2);
    
    for (size_t i = 0; i < m_currentWorkout.exercises.size(); ++i) {
        const Exercise& exercise = m_currentWorkout.exercises[i];
        float y = listStartY + i * (Layout::EXERCISE_ITEM_HEIGHT + Layout::SPACING_SMALL);
        
        // Exercise card with padding
        float alpha = (i == static_cast<size_t>(m_currentExerciseIndex)) ? 1.0f : 0.7f;
        renderer->drawRect(itemX, y, itemWidth, Layout::EXERCISE_ITEM_HEIGHT, 0.3f, 0.3f, 0.35f, alpha);
        
        // Exercise name and details with proper padding
        if (m_textRenderer) {
            float textX = itemX + Layout::PADDING_MEDIUM;
            m_textRenderer->drawText(textX, y + Layout::PADDING_SMALL, exercise.name, 1.0f, 1.0f, 1.0f, alpha, 1.0f);
            std::string setsReps = std::to_string(exercise.sets) + "x" + std::to_string(exercise.reps);
            m_textRenderer->drawText(textX, y + Layout::PADDING_MEDIUM + 10.0f, setsReps, 0.8f, 0.8f, 0.8f, alpha, 0.9f);
            if (exercise.weight > 0.0f) {
                std::string weightStr = std::to_string((int)exercise.weight) + "kg";
                m_textRenderer->drawText(textX, y + Layout::PADDING_MEDIUM + 25.0f, weightStr, 0.7f, 0.7f, 0.7f, alpha, 0.8f);
            }
        }
        
        // Progress indicator (sets completed) with padding
        float progressX = itemX + Layout::PADDING_MEDIUM;
        float progressWidth = (itemWidth - Layout::PADDING_MEDIUM * 2) * ((float)m_currentSetIndex / (float)exercise.sets);
        float progressY = y + Layout::EXERCISE_ITEM_HEIGHT - Layout::PADDING_SMALL - 8.0f;
        renderer->drawRect(progressX, progressY, progressWidth, 8.0f, 0.2f, 0.7f, 0.3f, 1.0f);
    }
}

void WorkoutTracker::renderDebugOverlay(Renderer* renderer) {
    if (!m_textRenderer) return;
    
    // Draw touch coordinates
    std::string touchStr = "Touch: " + std::to_string((int)m_lastTouchX) + "," + std::to_string((int)m_lastTouchY);
    m_textRenderer->drawText(10.0f, m_screenHeight - 60.0f, touchStr, 1.0f, 1.0f, 0.0f, 1.0f, 0.8f);
    
    // Draw state info
    std::string stateStr = m_currentWorkout.isActive ? "Active" : "Inactive";
    m_textRenderer->drawText(10.0f, m_screenHeight - 40.0f, "State: " + stateStr, 1.0f, 1.0f, 0.0f, 1.0f, 0.8f);
    
    // Draw exercise count
    std::string exCountStr = "Exercises: " + std::to_string(m_currentWorkout.exercises.size());
    m_textRenderer->drawText(10.0f, m_screenHeight - 20.0f, exCountStr, 1.0f, 1.0f, 0.0f, 1.0f, 0.8f);
}

void WorkoutTracker::onTouchDown(float x, float y) {
    m_lastTouchX = x;
    m_lastTouchY = y;
    if (!m_currentWorkout.isActive) {
        // Main screen - check for start workout button
        if (m_startButton && m_startButton->containsPoint(x, y)) {
            m_startButton->setPressed(true);
            startWorkout("Workout " + std::to_string(m_workoutHistory.size() + 1));
        } else if (m_historyButton && m_historyButton->containsPoint(x, y)) {
            m_historyButton->setPressed(true);
            // TODO: Show history
        }
    } else {
        // Workout screen
        if (m_endButton && m_endButton->containsPoint(x, y)) {
            m_endButton->setPressed(true);
            endWorkout();
        } else {
            // Check if tapping on an exercise
            float listStartY = Layout::HEADER_HEIGHT + Layout::SPACING_MEDIUM + Layout::PADDING_SMALL;
            float itemX = Layout::MARGIN_MEDIUM;
            float itemWidth = m_screenWidth - (Layout::MARGIN_MEDIUM * 2);
            
            for (size_t i = 0; i < m_currentWorkout.exercises.size(); ++i) {
                float itemY = listStartY + i * (Layout::EXERCISE_ITEM_HEIGHT + Layout::SPACING_SMALL);
                if (isPointInRect(x, y, itemX, itemY, itemWidth, Layout::EXERCISE_ITEM_HEIGHT)) {
                    m_currentExerciseIndex = i;
                    m_currentSetIndex = 0;
                    break;
                }
            }
        }
    }
}

void WorkoutTracker::onTouchUp(float x, float y) {
    // Handle touch up events if needed
    (void)x; (void)y;
}

void WorkoutTracker::onTouchMove(float x, float y, float dx, float dy) {
    // Handle swipe gestures, scrolling, etc.
    (void)x; (void)y; (void)dx; (void)dy;
}

void WorkoutTracker::onTouchCancel() {
    // Handle touch cancellation
}

void WorkoutTracker::onBackPressed() {
    if (m_currentWorkout.isActive) {
        // Pause or end workout on back press
        endWorkout();
    }
}

void WorkoutTracker::startWorkout(const std::string& name) {
    m_currentWorkout.name = name;
    m_currentWorkout.exercises.clear();
    m_currentWorkout.startTime = std::chrono::system_clock::now();
    m_currentWorkout.isActive = true;
    m_currentExerciseIndex = 0;
    m_currentSetIndex = 0;
    
    // Add some default exercises for demo
    addExercise("Push-ups", 3, 10, 0.0f);
    addExercise("Squats", 3, 15, 0.0f);
    addExercise("Plank", 3, 30, 0.0f);
    
    LOGI("Started workout: %s", name.c_str());
}

void WorkoutTracker::endWorkout() {
    if (m_currentWorkout.isActive) {
        m_currentWorkout.endTime = std::chrono::system_clock::now();
        m_currentWorkout.isActive = false;
        m_workoutHistory.push_back(m_currentWorkout);
        LOGI("Ended workout: %s", m_currentWorkout.name.c_str());
    }
}

void WorkoutTracker::addExercise(const std::string& name, int sets, int reps, float weight) {
    Exercise exercise;
    exercise.name = name;
    exercise.sets = sets;
    exercise.reps = reps;
    exercise.weight = weight;
    exercise.restTime = 60; // Default 60 seconds rest
    
    m_currentWorkout.exercises.push_back(exercise);
    LOGI("Added exercise: %s", name.c_str());
}

void WorkoutTracker::completeSet(int exerciseIndex) {
    if (exerciseIndex >= 0 && exerciseIndex < (int)m_currentWorkout.exercises.size()) {
        // Mark set as complete
        // This can be expanded to track individual set completion
    }
}

int WorkoutTracker::getElapsedSeconds() const {
    if (!m_currentWorkout.isActive) {
        return 0;
    }
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        now - m_currentWorkout.startTime);
    return (int)duration.count();
}

bool WorkoutTracker::isPointInRect(float x, float y, float rectX, float rectY, float rectW, float rectH) {
    return (x >= rectX && x <= rectX + rectW && y >= rectY && y <= rectY + rectH);
}

