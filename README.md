🌀 Maze Game

A top-down 2D maze game built using SDL3, where you explore procedurally generated mazes, collect coins, find keys, and reach the exit door!

🎮 Features

Procedurally generated maze every game

Collectible coins with glowing effects ✨

Key and exit door mechanics 🔑🚪

Player steps and time tracking ⏱️

High score tracking 🏆

Neon-style menu buttons with hover effects

Rules screen for new players

Game over screen with final score, coins collected, and elapsed time

Semi-transparent HUD and in-game messages

🕹️ Gameplay

Enter your name in the menu (default: Player1)

Click Play to view the rules

Move using W/A/S/D keys

Collect all coins scattered in the maze 💰

Find the key to unlock the door

Reach the exit door to finish the level 🎉

Score is based on coins collected and time taken

Press ESC anytime to return to the main menu

⌨️ Controls
Action	Key / Mouse
Move Up	W
Move Down	S
Move Left	A
Move Right	D
Return / Confirm	ENTER
Back / Quit	ESC
Click buttons	Mouse
💾 Installation

Clone the repository:

git clone https://github.com/yourusername/maze-game.git
cd maze-game


Install dependencies:

SDL3

SDL3_image

SDL3_ttf

Compile the game:

gcc main.c -o maze_game -I/path/to/SDL3/include -L/path/to/SDL3/lib -lSDL3 -lSDL3_image -lSDL3_ttf


Run the executable:

./maze_game

🖼️ Screenshots

Add screenshots inside your assets/screenshots/ folder and include them like this:

![Menu Screen](assets/screenshots/menu.png)
![Gameplay](assets/screenshots/gameplay.png)
![Game Over](assets/screenshots/gameover.png)


Example:


(Make sure your screenshot paths match your repo!)

🧩 Assets

Place all game assets in the assets folder:

assets/
├── player.png
├── key.png
├── door.png
├── wall.png
├── floor.png
├── coin.png
├── background.png
├── dejavusans.ttf
├── screenshots/  ← Add your screenshots here

⚙️ Dependencies

SDL3

SDL3_image

SDL3_ttf

Standard C libraries (stdio.h, stdlib.h, time.h, math.h)

📜 License

Open-source and free to use for educational purposes.
Feel free to modify and enhance the game!
-----


