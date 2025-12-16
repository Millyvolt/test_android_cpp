#ifndef STYLE_H
#define STYLE_H

namespace Style {
    // Color palette
    namespace Colors {
        // Background colors
        constexpr float BACKGROUND_DARK[4] = {0.1f, 0.1f, 0.15f, 1.0f};
        constexpr float BACKGROUND_CARD[4] = {0.3f, 0.3f, 0.35f, 1.0f};
        constexpr float BACKGROUND_HEADER[4] = {0.2f, 0.3f, 0.5f, 1.0f};
        
        // Primary colors
        constexpr float PRIMARY[4] = {0.2f, 0.4f, 0.6f, 1.0f};
        constexpr float PRIMARY_DARK[4] = {0.15f, 0.3f, 0.45f, 1.0f};
        constexpr float PRIMARY_LIGHT[4] = {0.3f, 0.5f, 0.7f, 1.0f};
        
        // Accent colors
        constexpr float ACCENT_GREEN[4] = {0.2f, 0.6f, 0.3f, 1.0f};
        constexpr float ACCENT_GREEN_PRESSED[4] = {0.3f, 0.7f, 0.4f, 1.0f};
        constexpr float ACCENT_RED[4] = {0.6f, 0.2f, 0.2f, 1.0f};
        constexpr float ACCENT_RED_PRESSED[4] = {0.7f, 0.3f, 0.3f, 1.0f};
        constexpr float ACCENT_GRAY[4] = {0.4f, 0.4f, 0.4f, 1.0f};
        constexpr float ACCENT_GRAY_PRESSED[4] = {0.5f, 0.5f, 0.5f, 1.0f};
        
        // Text colors
        constexpr float TEXT_PRIMARY[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        constexpr float TEXT_SECONDARY[4] = {0.8f, 0.8f, 0.8f, 1.0f};
        constexpr float TEXT_TERTIARY[4] = {0.7f, 0.7f, 0.7f, 1.0f};
        
        // Progress/Status colors
        constexpr float PROGRESS[4] = {0.2f, 0.7f, 0.3f, 1.0f};
        constexpr float PROGRESS_BACKGROUND[4] = {0.15f, 0.15f, 0.2f, 1.0f};
    }
    
    // Sizes
    namespace Sizes {
        constexpr float BUTTON_HEIGHT = 60.0f;
        constexpr float BUTTON_HEIGHT_LARGE = 80.0f;
        constexpr float TITLE_HEIGHT = 60.0f;
        constexpr float HEADER_HEIGHT = 100.0f;
        constexpr float EXERCISE_ITEM_HEIGHT = 100.0f;
        constexpr float ICON_SIZE = 24.0f;
        constexpr float ICON_SIZE_LARGE = 32.0f;
    }
    
    // Spacing
    namespace Spacing {
        constexpr float MARGIN_SMALL = 10.0f;
        constexpr float MARGIN_MEDIUM = 20.0f;
        constexpr float MARGIN_LARGE = 30.0f;
        constexpr float PADDING_SMALL = 8.0f;
        constexpr float PADDING_MEDIUM = 16.0f;
        constexpr float PADDING_LARGE = 24.0f;
        constexpr float SPACING_SMALL = 10.0f;
        constexpr float SPACING_MEDIUM = 20.0f;
        constexpr float SPACING_LARGE = 30.0f;
    }
}

#endif // STYLE_H

