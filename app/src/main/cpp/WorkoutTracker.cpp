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
    , m_bottomInset(0.0f)
    , m_textRenderer(nullptr)
    , m_startButton(nullptr)
    , m_historyButton(nullptr)
    , m_endButton(nullptr)
    , m_chooseExerciseButton(nullptr)
    , m_addSetButton(nullptr)
    , m_debugMode(false)
    , m_showingExerciseList(false)
    , m_lastTouchX(0.0f)
    , m_lastTouchY(0.0f)
{
    m_currentWorkout.isActive = false;
    m_textRenderer = new TextRenderer();
    
    m_startButton = new Button();
    m_startButton->setText("START WORKOUT");
    m_startButton->setColor(0.2f, 0.6f, 0.3f, 1.0f);
    m_startButton->setPressedColor(0.3f, 0.7f, 0.4f, 1.0f);
    m_startButton->setTextScale(8.0f);
    
    m_historyButton = new Button();
    m_historyButton->setText("HISTORY");
    m_historyButton->setColor(0.4f, 0.4f, 0.4f, 1.0f);
    m_historyButton->setPressedColor(0.5f, 0.5f, 0.5f, 1.0f);
    m_historyButton->setTextScale(8.0f);
    
    m_endButton = new Button();
    m_endButton->setText("END WORKOUT");
    m_endButton->setColor(0.6f, 0.2f, 0.2f, 1.0f);
    m_endButton->setPressedColor(0.7f, 0.3f, 0.3f, 1.0f);
    m_endButton->setTextScale(8.0f);
    
    m_chooseExerciseButton = new Button();
    m_chooseExerciseButton->setText("CHOOSE EXERCISE");
    m_chooseExerciseButton->setColor(0.3f, 0.5f, 0.7f, 1.0f);
    m_chooseExerciseButton->setPressedColor(0.4f, 0.6f, 0.8f, 1.0f);
    m_chooseExerciseButton->setTextScale(7.0f);
    
    m_addSetButton = new Button();
    m_addSetButton->setText("Add Set");
    m_addSetButton->setColor(0.2f, 0.6f, 0.3f, 1.0f);
    m_addSetButton->setPressedColor(0.3f, 0.7f, 0.4f, 1.0f);
    m_addSetButton->setTextColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_addSetButton->setTextScale(4.0f);
    
    // Populate available exercises
    m_availableExercises.push_back("Push-ups");
    m_availableExercises.push_back("Squats");
    m_availableExercises.push_back("Plank");

    (void)m_debugMode;
}

WorkoutTracker::~WorkoutTracker() {
    // Cleanup buttons
    if (m_startButton) delete m_startButton;
    if (m_historyButton) delete m_historyButton;
    if (m_endButton) delete m_endButton;
    if (m_chooseExerciseButton) delete m_chooseExerciseButton;
    if (m_addSetButton) delete m_addSetButton;
    if (m_textRenderer) delete m_textRenderer;
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
    if (!m_startButton || !m_historyButton || !m_endButton || !m_chooseExerciseButton) return;
    
    if (!m_currentWorkout.isActive) {
        // Main screen layout with proper spacing
        float buttonWidth = m_screenWidth - (Layout::MARGIN_LARGE * 2);
        float buttonY = Layout::centerY(Layout::BUTTON_HEIGHT_LARGE * 2 + Layout::SPACING_MEDIUM, m_screenHeight);
        
        m_startButton->setBounds(Layout::MARGIN_LARGE, buttonY, buttonWidth, Layout::BUTTON_HEIGHT_LARGE);
        
        float historyButtonY = buttonY + Layout::BUTTON_HEIGHT_LARGE + Layout::SPACING_MEDIUM;
        m_historyButton->setBounds(Layout::MARGIN_LARGE, historyButtonY, buttonWidth, Layout::BUTTON_HEIGHT_LARGE);
    } else {
        // Workout screen layout - account for bottom navigation bar inset
        float endButtonY = m_screenHeight - m_bottomInset - Layout::MARGIN_LARGE - Layout::BUTTON_HEIGHT;
        float buttonWidth = m_screenWidth - (Layout::MARGIN_LARGE * 2);
        m_endButton->setBounds(Layout::MARGIN_LARGE, endButtonY, buttonWidth, Layout::BUTTON_HEIGHT);
        
        // Position Choose Exercise button below header
        float chooseButtonY = Layout::HEADER_HEIGHT + Layout::SPACING_MEDIUM;
        m_chooseExerciseButton->setBounds(Layout::MARGIN_LARGE, chooseButtonY, buttonWidth, Layout::BUTTON_HEIGHT);
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
        m_textRenderer->drawText(titleTextX, titleY + Layout::PADDING_MEDIUM, "WORKOUT TRACKER", 1.0f, 1.0f, 1.0f, 1.0f, 4.5f);
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
    // If showing exercise selection list, render it and return
    if (m_showingExerciseList) {
        renderExerciseSelectionList(renderer);
        return;
    }
    
    // Header with workout name and timer
    renderer->drawRect(0.0f, 0.0f, m_screenWidth, Layout::HEADER_HEIGHT, 0.2f, 0.3f, 0.5f, 1.0f);
    
    // Timer display
    int elapsed = getElapsedSeconds();
    if (m_textRenderer) {
        float timerTextWidth = m_textRenderer->getTextWidth("00:00", 2.0f);
        float timerX = Layout::centerTextX("00:00", timerTextWidth, m_screenWidth);
        m_textRenderer->drawTime(timerX, Layout::PADDING_LARGE + 90.0f, elapsed, 1.0f, 1.0f, 1.0f, 1.0f, 6.0f);
        
        // Workout name
        float nameX = Layout::MARGIN_MEDIUM;
        m_textRenderer->drawText(nameX, Layout::PADDING_SMALL + 40.0f, m_currentWorkout.name, 0.9f, 0.9f, 0.9f, 1.0f, 6.0f);
    }
    
    // Choose Exercise button
    if (m_chooseExerciseButton) {
        m_chooseExerciseButton->render(renderer, m_textRenderer);
    }
    
    // Exercise list area with proper spacing - account for Choose Exercise button and bottom navigation bar inset
    float listY = Layout::HEADER_HEIGHT + Layout::SPACING_MEDIUM + Layout::BUTTON_HEIGHT + Layout::SPACING_SMALL;
    float listHeight = m_screenHeight - listY - m_bottomInset - Layout::BUTTON_HEIGHT - Layout::MARGIN_LARGE - Layout::SPACING_MEDIUM;
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
    
    // Account for Choose Exercise button position
    float listStartY = Layout::HEADER_HEIGHT + Layout::SPACING_MEDIUM + Layout::BUTTON_HEIGHT + Layout::SPACING_SMALL + Layout::PADDING_SMALL;
    float itemX = Layout::MARGIN_MEDIUM;
    float itemWidth = m_screenWidth - (Layout::MARGIN_MEDIUM * 2);
    
    for (size_t i = 0; i < m_currentWorkout.exercises.size(); ++i) {
        const Exercise& exercise = m_currentWorkout.exercises[i];
        float y = listStartY + i * (Layout::EXERCISE_ITEM_HEIGHT + Layout::SPACING_SMALL);
        
        // Exercise card with padding
        float alpha = (i == static_cast<size_t>(m_currentExerciseIndex)) ? 1.0f : 0.7f;
        renderer->drawRect(itemX, y, itemWidth, Layout::EXERCISE_ITEM_HEIGHT, 0.3f, 0.3f, 0.35f, alpha);
        
        if (m_textRenderer) {
            float textX = itemX + Layout::PADDING_MEDIUM;
            float currentY = y + Layout::PADDING_SMALL + 30.0f;
            
            // Exercise name
            m_textRenderer->drawText(textX, currentY, exercise.name, 1.0f, 1.0f, 1.0f, alpha, 5.0f);
            currentY += 50.0f;
            
            // Sets counter and Add Set button
            int completedSets = getCompletedSetsCount((int)i);
            int totalSets = (int)exercise.sets.size();
            std::string setsCounter = std::to_string(completedSets) + "/" + std::to_string(totalSets) + " sets";
            m_textRenderer->drawText(textX, currentY, setsCounter, 0.9f, 0.9f, 0.9f, alpha, 4.5f);
            
            // Add Set button using Button class
            if (m_addSetButton) {
                float addSetButtonX = textX + 500.0f;
                float addSetButtonY = currentY - 65.0f;
                m_addSetButton->setBounds(addSetButtonX, addSetButtonY, Layout::ADD_SET_BUTTON_WIDTH, Layout::ADD_SET_BUTTON_HEIGHT);
                m_addSetButton->render(renderer, m_textRenderer);
            }
            
            currentY += 50.0f;
            
            // Reps field with increment/decrement buttons
            // Find first incomplete set or use first set's reps
            int currentReps = exercise.defaultReps;
            if (!exercise.sets.empty()) {
                // Find first incomplete set
                bool foundIncomplete = false;
                for (size_t j = 0; j < exercise.sets.size(); ++j) {
                    if (!exercise.sets[j].completed) {
                        currentReps = exercise.sets[j].reps;
                        foundIncomplete = true;
                        break;
                    }
                }
                if (!foundIncomplete && !exercise.sets.empty()) {
                    currentReps = exercise.sets[0].reps;
                }
            }
            
            std::string repsLabel = "Reps: " + std::to_string(currentReps);
            m_textRenderer->drawText(textX, currentY, repsLabel, 0.9f, 0.9f, 0.9f, alpha, 4.5f);
            
            // Increment button (↑)
            float incButtonX = textX + 150.0f;
            float incButtonY = currentY - 35.0f;
            float buttonSize = 40.0f;
            renderer->drawRect(incButtonX, incButtonY, buttonSize, buttonSize, 0.3f, 0.5f, 0.7f, alpha);
            m_textRenderer->drawText(incButtonX + 12.0f, incButtonY + 10.0f, "↑", 1.0f, 1.0f, 1.0f, alpha, 4.0f);
            
            // Decrement button (↓)
            float decButtonX = incButtonX + buttonSize + Layout::SPACING_SMALL;
            renderer->drawRect(decButtonX, incButtonY, buttonSize, buttonSize, 0.5f, 0.3f, 0.3f, alpha);
            m_textRenderer->drawText(decButtonX + 12.0f, incButtonY + 10.0f, "↓", 1.0f, 1.0f, 1.0f, alpha, 4.0f);
            
            // Weight display if applicable
            if (exercise.defaultWeight > 0.0f) {
                currentY += 45.0f;
                std::string weightStr = std::to_string((int)exercise.defaultWeight) + "kg";
                m_textRenderer->drawText(textX, currentY, weightStr, 0.7f, 0.7f, 0.7f, alpha, 4.0f);
            }
        }
        
        // Progress indicator (sets completed) with padding
        float progressX = itemX + Layout::PADDING_MEDIUM;
        int totalSets = (int)exercise.sets.size();
        float progressRatio = totalSets > 0 ? (float)getCompletedSetsCount((int)i) / (float)totalSets : 0.0f;
        float progressWidth = (itemWidth - Layout::PADDING_MEDIUM * 2) * progressRatio;
        float progressY = y + Layout::EXERCISE_ITEM_HEIGHT - Layout::PADDING_SMALL - 8.0f;
        renderer->drawRect(progressX, progressY, progressWidth, 8.0f, 0.2f, 0.7f, 0.3f, 1.0f);
    }
}

void WorkoutTracker::renderExerciseSelectionList(Renderer* renderer) {
    // Draw semi-transparent overlay
    renderer->drawRect(0.0f, 0.0f, m_screenWidth, m_screenHeight, 0.0f, 0.0f, 0.0f, 0.7f);
    
    // Draw modal background
    float modalWidth = m_screenWidth - (Layout::MARGIN_LARGE * 2);
    float modalHeight = m_screenHeight * 0.6f;
    float modalX = Layout::MARGIN_LARGE;
    float modalY = Layout::centerY(modalHeight, m_screenHeight);
    
    renderer->drawRect(modalX, modalY, modalWidth, modalHeight, 0.2f, 0.2f, 0.25f, 1.0f);
    
    // Draw title
    if (m_textRenderer) {
        float titleY = modalY + Layout::PADDING_LARGE;
        float titleTextWidth = m_textRenderer->getTextWidth("SELECT EXERCISE", 1.2f);
        float titleTextX = Layout::centerTextX("SELECT EXERCISE", titleTextWidth, m_screenWidth);
        m_textRenderer->drawText(titleTextX, titleY + 40.0f, "SELECT EXERCISE", 1.0f, 1.0f, 1.0f, 1.0f, 6.0f);
    }
    
    // Draw exercise list
    float listStartY = modalY + Layout::PADDING_LARGE * 2 + 60.0f;
    float itemX = modalX + Layout::PADDING_MEDIUM;
    float itemWidth = modalWidth - (Layout::PADDING_MEDIUM * 2);
    float itemHeight = 120.0f;
    float itemSpacing = Layout::SPACING_SMALL;
    
    for (size_t i = 0; i < m_availableExercises.size(); ++i) {
        float itemY = listStartY + i * (itemHeight + itemSpacing);
        
        // Make sure items fit in modal
        if (itemY + itemHeight > modalY + modalHeight - Layout::PADDING_MEDIUM) {
            break;
        }
        
        // Draw exercise item background
        renderer->drawRect(itemX, itemY, itemWidth, itemHeight, 0.35f, 0.35f, 0.4f, 1.0f);
        
        // Draw exercise name
        if (m_textRenderer) {
            float textX = itemX + Layout::PADDING_MEDIUM;
            m_textRenderer->drawText(textX, itemY + Layout::PADDING_MEDIUM + 40.0f, m_availableExercises[i], 1.0f, 1.0f, 1.0f, 1.0f, 5.0f);
        }
    }
}

void WorkoutTracker::showExerciseSelectionList() {
    m_showingExerciseList = true;
}

void WorkoutTracker::hideExerciseSelectionList() {
    m_showingExerciseList = false;
}

void WorkoutTracker::addSetToExercise(int exerciseIndex) {
    if (exerciseIndex >= 0 && exerciseIndex < (int)m_currentWorkout.exercises.size()) {
        Exercise& exercise = m_currentWorkout.exercises[exerciseIndex];
        exercise.sets.push_back(Set(exercise.defaultReps, exercise.defaultWeight));
        LOGI("Added set to exercise: %s (total sets: %d)", exercise.name.c_str(), (int)exercise.sets.size());
    }
}

void WorkoutTracker::markSetCompleted(int exerciseIndex, int setIndex) {
    if (exerciseIndex >= 0 && exerciseIndex < (int)m_currentWorkout.exercises.size()) {
        Exercise& exercise = m_currentWorkout.exercises[exerciseIndex];
        if (setIndex >= 0 && setIndex < (int)exercise.sets.size()) {
            exercise.sets[setIndex].completed = true;
            LOGI("Marked set %d as completed for exercise: %s", setIndex + 1, exercise.name.c_str());
        }
    }
}

int WorkoutTracker::getCompletedSetsCount(int exerciseIndex) const {
    if (exerciseIndex >= 0 && exerciseIndex < (int)m_currentWorkout.exercises.size()) {
        const Exercise& exercise = m_currentWorkout.exercises[exerciseIndex];
        int completed = 0;
        for (size_t i = 0; i < exercise.sets.size(); ++i) {
            if (exercise.sets[i].completed) {
                completed++;
            }
        }
        return completed;
    }
    return 0;
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
        if (m_showingExerciseList) {
            // Exercise selection list is showing
            // Check if clicking on an exercise in the selection list
            float modalWidth = m_screenWidth - (Layout::MARGIN_LARGE * 2);
            float modalHeight = m_screenHeight * 0.6f;
            float modalX = Layout::MARGIN_LARGE;
            float modalY = Layout::centerY(modalHeight, m_screenHeight);
            
            float listStartY = modalY + Layout::PADDING_LARGE * 2 + 60.0f;
            float itemX = modalX + Layout::PADDING_MEDIUM;
            float itemWidth = modalWidth - (Layout::PADDING_MEDIUM * 2);
            float itemHeight = 120.0f;
            float itemSpacing = Layout::SPACING_SMALL;
            
            for (size_t i = 0; i < m_availableExercises.size(); ++i) {
                float itemY = listStartY + i * (itemHeight + itemSpacing);
                
                // Make sure item is within modal bounds
                if (itemY + itemHeight > modalY + modalHeight - Layout::PADDING_MEDIUM) {
                    break;
                }
                
                if (isPointInRect(x, y, itemX, itemY, itemWidth, itemHeight)) {
                    // Add selected exercise with default values
                    std::string exerciseName = m_availableExercises[i];
                    if (exerciseName == "Push-ups") {
                        addExercise(exerciseName, 3, 10, 0.0f);
                    } else if (exerciseName == "Squats") {
                        addExercise(exerciseName, 3, 15, 0.0f);
                    } else if (exerciseName == "Plank") {
                        addExercise(exerciseName, 3, 30, 0.0f);
                    }
                    hideExerciseSelectionList();
                    break;
                }
            }
            
            // Check if clicking outside modal to close
            if (!isPointInRect(x, y, modalX, modalY, modalWidth, modalHeight)) {
                hideExerciseSelectionList();
            }
        } else {
            // Normal workout screen
            if (m_endButton && m_endButton->containsPoint(x, y)) {
                m_endButton->setPressed(true);
                endWorkout();
            } else if (m_chooseExerciseButton && m_chooseExerciseButton->containsPoint(x, y)) {
                m_chooseExerciseButton->setPressed(true);
                showExerciseSelectionList();
            } else {
                // Check if tapping on an exercise in the workout
                float listStartY = Layout::HEADER_HEIGHT + Layout::SPACING_MEDIUM + Layout::BUTTON_HEIGHT + Layout::SPACING_SMALL + Layout::PADDING_SMALL;
                float itemX = Layout::MARGIN_MEDIUM;
                float itemWidth = m_screenWidth - (Layout::MARGIN_MEDIUM * 2);
                
                for (size_t i = 0; i < m_currentWorkout.exercises.size(); ++i) {
                    float itemY = listStartY + i * (Layout::EXERCISE_ITEM_HEIGHT + Layout::SPACING_SMALL);
                    
                    if (isPointInRect(x, y, itemX, itemY, itemWidth, Layout::EXERCISE_ITEM_HEIGHT)) {
                        Exercise& exercise = m_currentWorkout.exercises[i];
                        float textX = itemX + Layout::PADDING_MEDIUM;
                        float currentY = itemY + Layout::PADDING_SMALL + 30.0f + 50.0f; // Position after exercise name
                        
                        // Check for Add Set button click using Button class
                        if (m_addSetButton) {
                            float addSetButtonX = textX + 500.0f;
                            float addSetButtonY = currentY - 65.0f;
                            m_addSetButton->setBounds(addSetButtonX, addSetButtonY, Layout::ADD_SET_BUTTON_WIDTH, Layout::ADD_SET_BUTTON_HEIGHT);
                            
                            if (m_addSetButton->containsPoint(x, y)) {
                                m_addSetButton->setPressed(true);
                                addSetToExercise((int)i);
                                break;
                            }
                        }
                        
                        // Check for Reps increment button (↑)
                        float incButtonX = textX + 150.0f;
                        float incButtonY = currentY + 50.0f - 35.0f; // Position for reps field
                        float buttonSize = 40.0f;
                        
                        if (isPointInRect(x, y, incButtonX, incButtonY, buttonSize, buttonSize)) {
                            // Increment reps for first incomplete set or first set
                            if (!exercise.sets.empty()) {
                                bool found = false;
                                for (size_t j = 0; j < exercise.sets.size(); ++j) {
                                    if (!exercise.sets[j].completed) {
                                        exercise.sets[j].reps++;
                                        found = true;
                                        break;
                                    }
                                }
                                if (!found) {
                                    exercise.sets[0].reps++;
                                }
                            }
                            break;
                        }
                        
                        // Check for Reps decrement button (↓)
                        float decButtonX = incButtonX + buttonSize + Layout::SPACING_SMALL;
                        
                        if (isPointInRect(x, y, decButtonX, incButtonY, buttonSize, buttonSize)) {
                            // Decrement reps for first incomplete set or first set (minimum 1)
                            if (!exercise.sets.empty()) {
                                bool found = false;
                                for (size_t j = 0; j < exercise.sets.size(); ++j) {
                                    if (!exercise.sets[j].completed && exercise.sets[j].reps > 1) {
                                        exercise.sets[j].reps--;
                                        found = true;
                                        break;
                                    }
                                }
                                if (!found && exercise.sets[0].reps > 1) {
                                    exercise.sets[0].reps--;
                                }
                            }
                            break;
                        }
                        
                        // Otherwise, just select the exercise
                        m_currentExerciseIndex = i;
                        m_currentSetIndex = 0;
                        break;
                    }
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
        if (m_showingExerciseList) {
            // Close exercise selection list
            hideExerciseSelectionList();
        } else {
            // End workout on back press
            endWorkout();
        }
    }
}

void WorkoutTracker::startWorkout(const std::string& name) {
    m_currentWorkout.name = name;
    m_currentWorkout.exercises.clear();
    m_currentWorkout.startTime = std::chrono::system_clock::now();
    m_currentWorkout.isActive = true;
    m_currentExerciseIndex = 0;
    m_currentSetIndex = 0;
    m_showingExerciseList = false;
    
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
    exercise.defaultReps = reps;
    exercise.defaultWeight = weight;
    exercise.restTime = 60; // Default 60 seconds rest
    
    // Initialize sets vector with specified number of sets
    for (int i = 0; i < sets; ++i) {
        exercise.sets.push_back(Set(reps, weight));
    }
    
    m_currentWorkout.exercises.push_back(exercise);
    LOGI("Added exercise: %s with %d sets", name.c_str(), sets);
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

