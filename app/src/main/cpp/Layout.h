#ifndef LAYOUT_H
#define LAYOUT_H

class Layout {
public:
    // Spacing constants
    static constexpr float MARGIN_SMALL = 10.0f;
    static constexpr float MARGIN_MEDIUM = 20.0f;
    static constexpr float MARGIN_LARGE = 30.0f;
    static constexpr float PADDING_SMALL = 8.0f;
    static constexpr float PADDING_MEDIUM = 16.0f;
    static constexpr float PADDING_LARGE = 24.0f;
    static constexpr float SPACING_SMALL = 10.0f;
    static constexpr float SPACING_MEDIUM = 20.0f;
    static constexpr float SPACING_LARGE = 30.0f;
    
    // Size constants
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr float BUTTON_HEIGHT_LARGE = 80.0f;
    static constexpr float TITLE_HEIGHT = 60.0f;
    static constexpr float HEADER_HEIGHT = 100.0f;
    static constexpr float EXERCISE_ITEM_HEIGHT = 100.0f;
    
    // Helper functions
    static float centerX(float width, float screenWidth) {
        return (screenWidth - width) / 2.0f;
    }
    
    static float centerY(float height, float screenHeight) {
        return (screenHeight - height) / 2.0f;
    }
    
    static float centerTextX(const char* text, float textWidth, float containerWidth) {
        return (containerWidth - textWidth) / 2.0f;
    }
};

#endif // LAYOUT_H

