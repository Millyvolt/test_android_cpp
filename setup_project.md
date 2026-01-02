# Android Native Activity Workout Tracker

A workout tracking Android application built primarily with C++ using Native Activity and OpenGL ES for rendering.

## Project Structure

This project uses the Native Activity approach, meaning the application is written almost entirely in C++ with minimal Java code. The UI is rendered using OpenGL ES 2.0.

## Requirements

- Android Studio (latest version recommended)
- Android NDK (r21 or later)
- CMake 3.18+
- Android SDK (API 21+)
- Gradle 7.0+

## Setup Instructions

1. **Install Android Studio and NDK:**
   - Download and install Android Studio from [developer.android.com](https://developer.android.com/studio)
   - Open Android Studio and go to **File > Settings > Appearance & Behavior > System Settings > Android SDK**
   - In the **SDK Tools** tab, check:
     - NDK (Side by side)
     - CMake
   - Click **Apply** to install

2. **Configure SDK Path:**
   - Edit `local.properties` and set `sdk.dir` to your Android SDK path
   - Example: `sdk.dir=C\:\\Users\\YourUsername\\AppData\\Local\\Android\\Sdk`

3. **Build the Project:**
   - Open the project in Android Studio
   - Wait for Gradle sync to complete
   - Build the project: **Build > Make Project**

4. **Run on Device/Emulator:**
   - Connect an Android device or start an emulator
   - Click **Run** button or press `Shift+F10`

## Architecture

The application follows a modular architecture:

```
Native Activity Entry Point (main.cpp)
    ↓
App Class (Lifecycle Management)
    ├── Renderer (OpenGL ES Rendering)
    ├── InputHandler (Touch/Input Processing)
    └── WorkoutTracker (Business Logic)
```

### Key Components

- **main.cpp**: Native Activity entry point and event loop
- **App.h/cpp**: Main application class managing lifecycle, rendering, and input
- **Renderer.h/cpp**: OpenGL ES 2.0 rendering system
- **InputHandler.h/cpp**: Touch event processing
- **WorkoutTracker.h/cpp**: Core workout tracking logic

## Features

- Pure C++ implementation using Native Activity
- OpenGL ES 2.0 rendering
- Touch input handling
- Workout tracking foundation (exercises, sets, timers)
- Extensible architecture for future features

## Future Enhancements

The architecture supports future additions such as:
- Database integration (SQLite via C++)
- Sensor access (accelerometer, gyroscope)
- File I/O for data persistence
- Network capabilities
- Additional UI screens and navigation
- Advanced graphics and animations

## Building from Command Line

```bash
# On Windows (PowerShell)
.\gradlew assembleDebug

# On Linux/Mac
./gradlew assembleDebug
```

The APK will be generated at: `app/build/outputs/apk/debug/app-debug.apk`

## Troubleshooting

- **NDK not found**: Ensure NDK is installed via Android Studio SDK Manager
- **CMake errors**: Verify CMake is installed and path is correct
- **Build errors**: Check that `local.properties` has correct SDK path
- **Runtime crashes**: Check logcat for error messages: `adb logcat | grep WorkoutTracker`

## License

This project is provided as-is for educational and development purposes.

