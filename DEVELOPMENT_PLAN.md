# Android Workout Tracker - Development Plan

## Current State Analysis

### What's Working:
- Basic OpenGL ES 2.0 rendering system
- Text rendering and UI buttons
- Touch input handling (basic)
- Workout creation and management (in-memory)
- Exercise display in list format
- Timer display (elapsed time)
- Basic navigation between main screen and workout screen

### What's Missing/Incomplete:

## 1. Critical Missing Features (High Priority)

### Data Persistence
- **Status**: Workouts stored only in memory (`std::vector<Workout> m_workoutHistory`)
- **Problem**: All data lost when app closes
- **Solution Needed**: SQLite integration OR file-based storage using Android's AssetManager/JNI
- **Files to Modify**: `WorkoutTracker.h/cpp`, potentially new `StorageManager.h/cpp`

### Workout History Screen
- **Status**: History button exists but doesn't work (TODO at line 226 in `WorkoutTracker.cpp`)
- **Problem**: Cannot view past workouts
- **Solution Needed**: Implement history view to display past workouts with details
- **Files to Modify**: `WorkoutTracker.h/cpp`, add `renderHistoryScreen()` method

### Set Completion Tracking
- **Status**: `completeSet()` function is empty (lines 309-314)
- **Problem**: Progress indicator exists but doesn't update
- **Solution Needed**: Track completed sets per exercise, update progress indicators
- **Files to Modify**: `WorkoutTracker.h/cpp`, `Exercise` struct may need completion tracking

### Exercise Management UI
- **Status**: Can only add exercises programmatically (hardcoded defaults)
- **Problem**: No UI to add/edit/remove exercises during workout setup
- **Solution Needed**: Forms or dialogs for exercise creation
- **Files to Modify**: `WorkoutTracker.h/cpp`, new UI components

## 2. Enhanced Functionality (Medium Priority)

### Rest Timer
- **Status**: `restTime` field exists in Exercise struct but isn't used
- **Problem**: No rest period functionality
- **Solution Needed**: Timer countdown between sets with visual/audio cues
- **Files to Modify**: `WorkoutTracker.h/cpp`, potentially new `Timer.h/cpp`

### Exercise Detail View
- **Status**: Tapping exercise only selects it
- **Problem**: No detailed view with instructions/media
- **Solution Needed**: Full-screen view with exercise details, GIFs/images, instructions
- **Files to Modify**: `WorkoutTracker.h/cpp`, add `renderExerciseDetailScreen()` method

### Touch Interactions
- **Status**: `onTouchUp()` and `onTouchMove()` are empty (lines 251-259)
- **Problem**: No scrolling for long exercise lists
- **Solution Needed**: Swipe gestures, scrollable lists, better button press feedback
- **Files to Modify**: `WorkoutTracker.h/cpp`, `InputHandler.h/cpp`

### Exercise Progress Tracking
- **Status**: Current set tracking is incomplete
- **Problem**: Cannot track individual set performance
- **Solution Needed**: Per-set tracking (reps completed, weight used)
- **Files to Modify**: `Exercise` struct, `WorkoutTracker.h/cpp`

## 3. Media and Visual Enhancements (Your Original Question)

### Image/GIF Support
- **Status**: Renderer only supports colored rectangles
- **Problem**: No texture rendering capability
- **Solution Needed**:
  - Texture loading from assets or files
  - GIF decoding library (giflib) OR Android MediaMetadataRetriever via JNI
  - Texture shader program for image rendering
  - GIF animation playback system
- **Files to Modify**: `Renderer.h/cpp` (add texture support), new `GifDecoder.h/cpp` or use JNI, `CMakeLists.txt` (if using giflib)

### UI Improvements
- **Status**: Basic rectangles and text
- **Problem**: Limited visual appeal
- **Solution Needed**: Rounded corners (partially implemented), icons, better styling
- **Files to Modify**: `Renderer.cpp` (improve `drawRoundedRect()`), `Style.h`

## 4. Architecture Enhancements (Low Priority)

### State Management
- **Status**: Single screen state tracking
- **Problem**: Limited navigation capability
- **Solution Needed**: Screen state machine (Main → Workout Setup → Active Workout → History → Exercise Detail)
- **Files to Modify**: `WorkoutTracker.h/cpp`, add state enum/state machine

### Error Handling
- **Status**: Basic error handling
- **Problem**: Limited user feedback
- **Solution Needed**: Better validation, user feedback for errors
- **Files to Modify**: Throughout codebase

### Performance
- **Status**: Rendering appears fine, but no optimization
- **Problem**: May have performance issues with large datasets
- **Solution Needed**: Consider frame rate limiting, efficient rendering
- **Files to Modify**: `App.cpp`, `Renderer.cpp`

## Recommended Development Roadmap

### Phase 1: Core Functionality (Essential)
1. **Implement Data Persistence** (SQLite or JSON file storage)
   - Priority: Critical
   - Estimated Complexity: High
   - Dependencies: Android NDK file I/O or SQLite

2. **Implement Workout History Screen**
   - Priority: High
   - Estimated Complexity: Medium
   - Dependencies: Data persistence

3. **Complete Set Completion Tracking**
   - Priority: High
   - Estimated Complexity: Low-Medium
   - Dependencies: None

4. **Add Exercise Editing UI**
   - Priority: High
   - Estimated Complexity: Medium
   - Dependencies: None

### Phase 2: Enhanced Tracking (Important)
5. **Implement Rest Timer**
   - Priority: Medium
   - Estimated Complexity: Medium
   - Dependencies: Set completion tracking

6. **Add Exercise Detail View**
   - Priority: Medium
   - Estimated Complexity: Medium
   - Dependencies: State management improvements

7. **Improve Touch Interactions** (scrolling, gestures)
   - Priority: Medium
   - Estimated Complexity: Medium
   - Dependencies: None

### Phase 3: Media Support (Enhancement)
8. **Add Texture Rendering to Renderer**
   - Priority: Medium (for GIF support)
   - Estimated Complexity: High
   - Dependencies: OpenGL ES texture knowledge

9. **Integrate GIF/Image Support** (giflib or Android APIs)
   - Priority: Medium (your original question)
   - Estimated Complexity: High
   - Dependencies: Texture rendering

10. **Add GIF Display to Exercise Detail View**
    - Priority: Medium
    - Estimated Complexity: Medium
    - Dependencies: GIF support

### Phase 4: Polish (Nice to Have)
11. **UI/UX Improvements**
    - Priority: Low
    - Estimated Complexity: Medium
    - Dependencies: None

12. **Performance Optimization**
    - Priority: Low
    - Estimated Complexity: Medium-High
    - Dependencies: All core features

13. **Testing and Bug Fixes**
    - Priority: Ongoing
    - Estimated Complexity: Variable
    - Dependencies: All features

## Technical Considerations for GIF Support

Since you asked about GIF implementation earlier, here's what's needed:

### Requirements:
1. **Texture Support in OpenGL ES**
   - New shader program with texture sampling
   - Texture loading from files/assets
   - Texture coordinate handling

2. **GIF Decoding**
   - Option A: giflib library (C library, needs CMake integration)
   - Option B: Android MediaMetadataRetriever via JNI (easier integration)

3. **Animation Loop Management**
   - Frame timing (milliseconds per frame)
   - Texture updates per frame
   - Loop control

4. **Memory Management**
   - Loading/unloading GIFs efficiently
   - Frame caching strategy
   - Texture memory management

### Implementation Approach:
- **Storage**: GIF files in `app/src/main/assets/` folder
- **Display Location**: Exercise detail view (full screen) + optionally thumbnails in list
- **Library Choice**: Android MediaMetadataRetriever via JNI (recommended for easier integration)

## Files Structure Reference

### Current Key Files:
- `app/src/main/cpp/WorkoutTracker.h/cpp` - Core business logic
- `app/src/main/cpp/Renderer.h/cpp` - OpenGL rendering
- `app/src/main/cpp/App.h/cpp` - Application lifecycle
- `app/src/main/cpp/TextRenderer.h/cpp` - Text rendering
- `app/src/main/cpp/Button.h/cpp` - Button UI component
- `app/CMakeLists.txt` - Build configuration
- `app/build.gradle` - Gradle build file

### New Files Likely Needed:
- `StorageManager.h/cpp` - Data persistence
- `GifDecoder.h/cpp` - GIF loading/decoding (if custom implementation)
- `Timer.h/cpp` - Rest timer functionality
- `StateMachine.h/cpp` - Screen state management (optional)

## Notes

- This plan is based on codebase analysis as of current state
- Priorities may shift based on user requirements
- Each phase builds upon previous phases
- Consider breaking large features into smaller milestones

---

**Last Updated**: Based on codebase analysis
**Next Steps**: Choose a phase/feature to begin implementation

