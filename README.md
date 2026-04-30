# KatanaEngine

## Overview
KatanaEngine is a lightweight 2D game engine built on top of the Allegro 5 library. It provides a solid foundation for building 2D games with a focus on simplicity, performance, and ease of use.

## Purpose
KatanaEngine serves as the core foundation layer for all game projects, providing:
- **Rendering System** (2D graphics, sprites, primitives)
- **Input Management** (keyboard, mouse)
- **Resource Management** (textures, fonts, animations, audio)
- **Game Structure** (game loop, timing, lifecycle)
- **Screen Management** (scene/state management)
- **Camera System** (viewport and world-space rendering)
- **UI Components** (buttons, panels, textboxes, checkboxes)
- **Particle System** (visual effects)

## Architecture
```
Game Projects (AltarsGame, etc.)
    ↓
Reusable Game Layer (KatanaRPG, KatanaShmup, KatanaPlatformer - optional)
    ↓
KatanaEngine (core engine)
    ↓
Allegro 5 (platform layer)
```

## Core Systems

### Game Framework
- **Game**: Base class for all games, provides main game loop and lifecycle
  - Configurable screen size, fullscreen, and frame rate
  - Automatic initialization and shutdown
  - Virtual methods for game logic (LoadContent, Update, Draw, HandleInput)
  
- **GameTime**: Time tracking for frame-independent game logic
  - Delta time
  - Total elapsed time
  - Fixed timestep support

### Graphics System
- **SpriteBatch**: Efficient 2D rendering with batching
  - Sprite drawing with transforms (position, rotation, scale, color tint)
  - Texture region rendering (sprite sheets)
  - Primitive shapes (rectangles, circles, lines)
  - Camera/viewport support
  
- **Color**: RGB/RGBA color representation with named colors
- **Camera**: 2D camera with position, zoom, and target following
- **Texture**: Image resource management

### Resource Management
- **ResourceManager**: Centralized asset loading and caching
  - Textures
  - Fonts
  - Animations
  - Effects
  
- **Animation**: Frame-based sprite animation
- **AnimationSet**: Collections of related animations (walk, run, idle, etc.)

### Input System
- **InputState**: Unified input handling
  - Keyboard state (keys pressed, held, released)
  - Mouse state (position, buttons, wheel)
  - Input polling and event detection

### Screen Management
- **ScreenManager**: Stack-based screen/scene management
  - Multiple screens with layering
  - Screen transitions
  - Input/update/draw blocking control
  
- **Screen**: Base class for game screens/states
  - Menu screens
  - Gameplay screens
  - Pause screens
  - Transitions

### UI System (GUI)
- **Control**: Base class for all UI elements
- **Button**: Clickable buttons with hover states
- **Panel**: Container for grouping controls
- **Textbox**: Text input fields
- **Checkbox**: Toggle controls

### Particle System
- **ParticleEmitter**: Flexible particle effect system
- **Particle**: Individual particle with behaviors
- **ParticleBehaviors**: Pre-built particle behaviors (movement, fading, scaling)

### Math Utilities
- **Vector2**: 2D vector math
- **Rectangle**: Axis-aligned bounding boxes
- **Circle**: Circular collision shapes
- **Line**: Line segments
- **Math**: Common math operations (lerp, clamp, etc.)

## Usage Example

### Basic Game Setup
```cpp
#include "Core/Game.h"

class MyGame : public KatanaEngine::Game
{
public:
    MyGame() : Game(KatanaEngine::GameConfig{
        .UseScreenManager = true,
        .ScreenWidth = 1920,
        .ScreenHeight = 1080,
        .Fullscreen = false
    })
    {
        SetTitle("My Awesome Game");
        SetTargetFrameRate(60.0);
    }

protected:
    void LoadContent(ResourceManager& resourceManager) override
    {
        // Load textures, fonts, etc.
        m_texture = resourceManager.Load<Texture>("hero.png");
    }

    void HandleInput(const InputState& input) override
    {
        if (input.IsKeyPressed(Keys::ESCAPE))
            Exit();
    }

    void Update(const GameTime& gameTime) override
    {
        // Update game logic
    }

    void Draw(SpriteBatch& spriteBatch) override
    {
        Clear(Color::BLACK);
        spriteBatch.Begin();
        spriteBatch.Draw(m_texture, Vector2(100, 100));
        spriteBatch.End();
    }

private:
    std::shared_ptr<Texture> m_texture;
};

int main(int argc, char** argv)
{
    MyGame game;
    return game.Run();
}
```

### Using Screen Manager
```cpp
class MenuScreen : public KatanaEngine::Screen
{
public:
    void LoadContent(ResourceManager& resourceManager) override
    {
        // Load menu assets
    }

    void HandleInput(const InputState& input) override
    {
        if (input.IsKeyPressed(Keys::ENTER))
        {
            // Switch to gameplay screen
            auto gameplayScreen = std::make_shared<GameplayScreen>();
            GetScreenManager()->AddScreen(gameplayScreen);
        }
    }

    void Draw(SpriteBatch& spriteBatch) override
    {
        // Draw menu
    }
};
```

## Building
KatanaEngine is a static library (.lib) that automatically builds before dependent projects in the solution.

### Build Output
- **Debug**: `KatanaEngine\Build\x64\Debug\KatanaEngine.lib`
- **Release**: `KatanaEngine\Build\x64\Release\KatanaEngine.lib`

## Dependencies
- **Allegro 5** (via NuGet packages)
  - Allegro.5.2.11.3
  - AllegroDeps.1.16.0
- **C++17 or later**
- **Platform**: Windows (Visual Studio 2019+)

## Project Structure
```
KatanaEngine/
├── Source/
│   ├── Core/           # Core engine systems
│   │   ├── Game.h/cpp
│   │   ├── GameTime.h
│   │   ├── Camera.h/cpp
│   │   ├── Graphics/   # Rendering
│   │   ├── Input/      # Input handling
│   │   ├── Resource/   # Asset management
│   │   ├── Screens/    # Screen management
│   │   ├── Math/       # Math utilities
│   │   └── Platform/   # Platform abstraction
│   ├── GUI/            # User interface controls
│   ├── Particles/      # Particle system
│   └── pch.h/cpp       # Precompiled header
└── Build/              # Output directory
```

## Configuration
KatanaEngine uses `GameConfig` for initialization:
```cpp
GameConfig config;
config.UseScreenManager = true;
config.ContentRoot = "../../../Content/";
config.ScreenWidth = 1920;
config.ScreenHeight = 1080;
config.Fullscreen = false;
```

## Best Practices
1. **Use ResourceManager** for all asset loading to benefit from caching
2. **Inherit from Screen** for different game states (menu, gameplay, etc.)
3. **Use SpriteBatch.Begin/End** pairs for efficient rendering
4. **Use Vector2/Rectangle** for position and collision detection
5. **Use GameTime.GetElapsedTime()** for frame-independent movement
6. **Preload assets** in LoadContent() rather than during gameplay

## Performance Notes
- SpriteBatch uses batched rendering for optimal performance
- Resources are cached by the ResourceManager
- Fixed timestep game loop prevents timing issues
- Precompiled headers (pch.h) speed up compilation
