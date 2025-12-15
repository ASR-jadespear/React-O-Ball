
# React-O-Ball

**React-O-Ball** is a dynamic, elemental brick-breaker game built with C++ and Raylib. It reimagines the classic breakout genre by introducing elemental physics, a unique text-based level generator, and a chaotic "Pinball Mode" that transforms the gameplay mechanics in real-time.

## 🎮 Game Overview

In React-O-Ball, your goal is to clear the screen of elemental bricks. However, unlike standard brick-breakers, the ball and bricks possess elemental properties (Fire, Water, Ice, Electric, Wind) that interact with each other. A fire ball might melt ice bricks, while an electric reaction can chain through adjacent blocks.

To add to the challenge, a timer constantly ticks down. When it hits zero, the game shifts into **Pinball Mode**, replacing your standard paddle with rotating flippers, requiring entirely different reflexes to keep the ball in play.

## ✨ Key Features

*   **Elemental Reaction System**: Complex interactions between elements (e.g., Fire melts Ice, Water conducts Electricity).
*   **Text-to-Level Generation**: The game allows players to input a 5-letter word at the start. The game parses this text and constructs the level layout using the shapes of the letters provided.
*   **Pinball Mode**: A 10-second timer toggles the control scheme between a standard sliding paddle and rotating pinball flippers.
*   **Animated Visuals**: Uses a custom GIF animation system to render animated bricks (flaming fire, flowing water, spinning wind).
*   **Physics & Polish**:
    *   Ball trail effects based on the current element.
    *   Trajectory prediction line (aim assist) when the ball is near the paddle.
    *   Dynamic ball speed adjustments and gravity simulation.
*   **High Score System**: Persists your best scores to a local file.

## 🕹️ Controls

### Interface
*   **Typing**: Enter text to generate a level.
*   **Enter**: Start the game.
*   **Backspace**: Delete character.

### Gameplay
*   **Paddle Movement**: Mouse (Horizontal) or `Left` / `Right` Arrow Keys.
*   **Launch Ball**: `Left Mouse Button` or `Space`.
*   **Pinball Flippers** (Active during Pinball Mode):
    *   **Left Flipper**: `Left Mouse Button` or `Up Arrow`.
    *   **Right Flipper**: `Right Mouse Button` or `Down Arrow`.
*   **Pause**: `Tab`.
*   **Fullscreen**: `F`.
*   **Restart**: `R` (On Game Over/Win screen).

### Debug / Cheats
*   **1**: Change Ball to **Fire**.
*   **2**: Change Ball to **Ice**.
*   **3**: Change Ball to **Water**.
*   **O**: Instant Win (Clears all active elemental bricks).

## ⚗️ Elemental Mechanics

The core of the game revolves around how the ball's element interacts with the brick's element upon collision:

| Element | Interaction |
| :--- | :--- |
| **Fire** 🔥 | Melts **Ice** into Water. Destroys **Wind**. |
| **Water** 💧 | Extinguishes **Fire**. Conducts **Electric** (Shock). Destroys **Wind**. |
| **Ice** ❄️ | Freezes **Water** into Ice. Destroys **Wind**. |
| **Electric** ⚡ | Triggers a chain reaction, destroying adjacent bricks. |
| **Wind** 🍃 | Spreads the reaction of the hitting element to its neighbors. |
| **Earth** 🪨 | Non-reactive (often acts as a blocker or requires specific elements). |

## 🛠️ Technical Implementation

This project is written in **C++** using the **Raylib** library. Below are insights into how specific features were implemented:

### 1. Custom GIF Animation Structure
Raylib handles static textures well, but this project implements a custom `GifAnimation` struct to handle animated sprites for the elements.
*   **Logic**: It loads a GIF as an `Image`, calculates frames based on dimensions, and updates the GPU texture pointer (`UpdateTexture`) at a set frame delay interval.
*   **Usage**: Used for all elemental bricks (FireAnim, WaterAnim, etc.) and the intro sequence.

### 2. Level Generation (`InitBricks`)
The game uses a bitmap-style mapping system.
*   **Letter Map**: Characters (A-Z, 0-9, symbols) are defined as 2D binary arrays (1s and 0s) in `letters.h`.
*   **Construction**: When the user types a word, the `InitBricks` function iterates through the input string, looks up the corresponding grid, and instantiates `Brick` structs at the calculated screen coordinates, assigning random elements to them.

### 3. Recursive Reaction Engine
The `reactAdjacent` function implements a recursive depth-limited search (Flood Fill variant).
*   When an **Electric** or **Wind** brick is hit, it triggers a reaction on its neighbors.
*   To prevent infinite loops (stack overflow), a static `reactionDepth` counter limits the recursion depth.

### 4. Collision & Physics
*   **Paddle**: Standard AABB (Axis-Aligned Bounding Box) vs. Circle collision.
*   **Pinball Flippers**: Uses **Circle-Line collision** math. The flippers are defined as line segments that rotate around a pivot point. When the ball hits a flipper, the reflection vector is calculated based on the line's normal, and a speed boost is applied to simulate the "kick" of a pinball bumper.
*   **Cooldowns**: A `collisionCooldown` timer prevents the ball from getting stuck inside the rotating flippers or triggering multiple collisions instantly.

### 5. State Management
The game loop is controlled via boolean flags (`inInterface`, `gameOver`, `PinballMode`, `isPaused`). The `PinballMode` is toggled by a simple float timer (`gameTimer`) that resets every 10 seconds.

## 🚀 How to Build

This project is designed for the **Raylib C++ Starter Template for VSCode**.

1.  **Prerequisites**:
    *   C++ Compiler (MinGW, Clang, or MSVC).
    *   VSCode with C/C++ Extension.
    *   Raylib library installed or linked in the project.

2.  **Setup**:
    *   Clone the repository.
    *   Open the folder in VSCode.
    *   Ensure your `.vscode/tasks.json` and `Makefile` (if applicable) are configured to link `raylib`, `opengl32`, `gdi32`, and `winmm`.

3.  **Compilation**:
    *   Press `F5` in VSCode to build and debug.
    *   Or run `make` in the terminal if a Makefile is present.

## 📂 Assets

*   **Fonts**: Custom fonts loaded from `fonts/`.
*   **Audio**: WAV files for sound effects (collisions, reactions) and background music.
*   **Images**: GIF animations and PNG icons located in `Gifs/` and `Icons/`.


=======

