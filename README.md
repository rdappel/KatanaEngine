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
- **Image**: CPU-side bitmap for assembling and saving images (e.g. sprite sheets built from `RenderTarget` frames); saves straight-alpha PNGs, and `CopyTo` copies an area back into a `RenderTarget` for drawing

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
- **Spline**: A cubic Bézier segment (`Lerp`, drawable with `SpriteBatch::DrawPrimitive`)
- **Curve**: A 1D keyframed curve compatible with XNA's `Curve` (see below)

#### Curve

`Curve` maps a position (usually time) to a value through keys, like XNA's `Microsoft.Xna.Framework.Curve`: the same keys (`Position`, `Value`, `TangentIn`, `TangentOut`, `Continuity`), `PreLoop` / `PostLoop` (`CONSTANT`, `CYCLE`, `CYCLE_OFFSET`, `OSCILLATE`, `LINEAR`), `ComputeTangent(s)` (`FLAT`, `LINEAR`, `SMOOTH`) and `Evaluate`, giving the same values as XNA. Between keys it uses cubic Hermite interpolation; tangents are per segment, as in XNA. (MonoGame's two known differences are not copied: its Step keys compare the raw position instead of the segment fraction, and its LINEAR post-loop uses the first key's tangent.)

```cpp
Curve fade;
fade.AddKey({ 0.0f, 1.0f });
fade.AddKey({ 0.5f, 0.8f });
fade.AddKey({ 1.0f, 0.0f });
fade.ComputeTangents(CurveTangent::SMOOTH);
float alpha = fade.Evaluate(particle.GetNormalizedAge());

// Or load one (XNA content XML; one curve per file):
std::shared_ptr<Curve> curve = resourceManager.Load<Curve>("Curves/Fade.xml");
```

Files use XNA's XML, so curves move freely between XNA, MonoGame (and the XNA Curve Editor sample) and KatanaEngine. `SaveToFile` writes the expanded key form; loading also accepts XNA's compact form (`<Keys>position value tangentIn tangentOut continuity ...</Keys>`).

```xml
<?xml version="1.0" encoding="utf-8"?>
<XnaContent xmlns:Framework="Microsoft.Xna.Framework">
  <Asset Type="Framework:Curve">
    <PreLoop>Constant</PreLoop>
    <PostLoop>Constant</PostLoop>
    <Keys>
      <CurveKey>
        <Position>0</Position>
        <Value>1</Value>
        <TangentIn>0</TangentIn>
        <TangentOut>0</TangentOut>
        <Continuity>Smooth</Continuity>
      </CurveKey>
    </Keys>
  </Asset>
</XnaContent>
```

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
KatanaEngine is a static library (.lib). Games include it as a git submodule at `<GameRepo>/KatanaEngine/` and add `KatanaEngine.vcxproj` to their solution, so it builds before them.

### Build Output
`KatanaEngine\Build\<x64|x86>\<Debug|Release>\KatanaEngine.lib`

### Using KatanaEngine in a game
1. Add the submodule: `git submodule add https://github.com/rdappel/KatanaEngine.git KatanaEngine`
2. Add `KatanaEngine\KatanaEngine.vcxproj` to the game's `.slnx`.
3. In the game's `.vcxproj`:
   ```xml
   <!-- just before <PropertyGroup Label="UserMacros" /> -->
   <ImportGroup Label="PropertySheets">
     <Import Project="..\KatanaEngine\KatanaEngine.props" />
   </ImportGroup>

   <!-- just before Microsoft.Cpp.targets -->
   <ItemGroup>
     <ProjectReference Include="..\KatanaEngine\KatanaEngine.vcxproj">
       <Project>{467fb328-bb2b-4bcc-945e-41b96754a22d}</Project>
     </ProjectReference>
   </ItemGroup>

   <!-- inside <ImportGroup Label="ExtensionTargets"> -->
   <Import Project="..\KatanaEngine\KatanaEngine.targets" />
   ```
   Adjust the `..\` paths to wherever the project sits relative to the submodule.

- `KatanaEngine.props` adds the engine's `Source` folder to the include path.
- `KatanaEngine.targets` imports Allegro, enables the addons the engine needs, and picks the Allegro library type: static libraries get `StaticMonolithRelease`, applications get `DynamicDebug` / `DynamicRelease`, and the DLLs are copied next to the exe. Any `Allegro_*` property a project sets itself overrides these defaults.
- The game does **not** need Allegro in its own `packages.config`. Restoring NuGet packages for the solution restores them through the engine project.

### Hosting a game inside a WinForms app
A game can run inside a .NET WinForms window, e.g. as the viewport of an editor or tool. The Allegro window becomes a borderless child of a WinForms control. The game runs on its own thread, and the two sides exchange string messages.

**Native side (C++ DLL):**
1. Make the game project a DLL (`<ConfigurationType>DynamicLibrary</ConfigurationType>`) and import the engine `.props`/`.targets` as above. The `.targets` file removes Allegro's "magic main" and `allegro_main` for DLLs.
2. In one `.cpp` file:
   ```cpp
   #include "Core/Hosting/HostExports.h"

   std::unique_ptr<KatanaEngine::Game> CreateHostedGame(const KatanaEngine::GameConfig& config)
   {
       return std::make_unique<MyGame>(config);  // config.ParentWindow is already set
   }

   KATANA_DEFINE_HOST_EXPORTS(CreateHostedGame)
   ```
3. In the game, override `OnHostMessage(const std::string&)` to receive messages, and call `SendHostMessage(...)` to reply.

**Managed side (C#):**
1. Reference `KatanaEngine\WinForms\KatanaEngine.WinForms.csproj` (net10.0-windows).
2. Add a `KatanaViewport` control, set `GameLibraryPath = "MyGame.dll"`, then use `Post(message)` and the `MessageReceived` event (raised on the UI thread).
3. Copy the game DLL and its Allegro DLLs next to the app's exe, e.g. with an `AfterBuild` copy target.

**How it works** (`Source/Core/Hosting/GameHost.*`, `Source/Core/Platform/WindowEmbedding.*`):
- `GameConfig.ParentWindow` creates the window offscreen and frameless, then makes it a child of the host window (`SetParent`).
- `Game::RequestExit`, `RequestResize` and `PostHostMessage` are thread-safe. The game thread applies them at the start of its next loop iteration.
- `GameHost::Start` and `Stop` keep servicing window messages sent to the UI thread while they wait. Without that, the parent and child windows (which live on different threads) can deadlock.
- Hosted games install Allegro without `atexit` and uninstall it when the game thread ends. Uninstalling at DLL unload would deadlock under the loader lock.
- One hosted game per DLL. The DLL stays loaded for the life of the process.

## Dependencies
- **Allegro 5** (via NuGet packages; `packages.config` is the single source of truth for the versions)
- **C++17 or later**
- **Platform**: Windows (Visual Studio 2019+)

### Upgrading Allegro
1. Change the versions in `KatanaEngine/packages.config` only. `KatanaEngine.targets` reads them from there. Don't use the NuGet UI: it would add hard-coded imports back into `KatanaEngine.vcxproj`.
2. Restore packages (`msbuild <solution> -t:restore -p:RestorePackagesConfig=true`), build and fix the engine, then commit and push it.
3. Each game picks up the new Allegro version when it updates its `KatanaEngine` submodule.

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
├── KatanaEngine.props   # Import into consuming projects (include path)
├── KatanaEngine.targets # Import into consuming projects (Allegro setup)
├── packages.config      # Allegro NuGet versions
└── Build/               # Output directory
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
