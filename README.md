# Basic Pong 

## First Milestone 

Adjusting the ball collisions, I used Box2D so this was pretty straightforward (the library does most of the collision logic)

![Pong Gameplay](https://github.com/Xbz-24/pong/blob/main/gifs/ezgif.com-video-to-gif%20(1).gif)

Also for this milestone I added textures for the sprites and FullScreen Support (using SFML window & sf::VideoMode)

## Overview

This project is a modern reinterpretation fo the classic Pong game, developed using C++ with the SFML and Box2D libraries. It features a two-dimensional table tennis-like game where players control paddes to hit a ball back and forth.

## Features

- High-resolution graphics and smooth gameplay.
- Physics-based movement and collision handling using Box2D.
- Customizable paddle and ball textures.
- FPS counter for performance monitoring.
- Scalable to different screen resolutions.

## Prerequisites

- SFML 2.5 or higher 
- Box2D 
- CMake 3.0 or higher
- A C++20 compatible compiler

## How to Run

1. Clone this repo to your local machine
2. Navigate to the project directory
3. Compile the game using Cmake

### For linux/macos you can use:
1. mkdir -p ~/games
2. git clone -b main https://github.com/Xbz-24/pong.git ~/games 
3. cd ~/games/build 
4. rm -rf *
5. cmake -B . -S ..
6. make

Run the compiled executable:

7. cd ~/games/build
8. ./pong
9. Enjoy.

## Code Overview

- The game initializes a ball with a texture loaded from a local path.
- The ball is set to move with a certain velocity in both the x and y directions.
- The game loop checks for window close events and updates the ball's position.
- If the ball reaches the edges of the window, it reverses its velocity to simulate a bounce.

## Future Improvements

- Add paddles for two players.
- Implement scoring mechanism
- Add sound effects for ball bounces.


