# Pong

A simple implementation of the classic Pong game using the SFML library

![Alt Text](https://github.com/Xbz-24/pong/blob/main/ezgif.com-video-to-gif.gif)



## Description

This project showcases a boucing ball (representing the classic Pong ball) that moves around the screen and bounces off the edges. The game window has a resolution of 850x500 pixels.

## Prerequisites

- SFML library: This game uses the Simple and Fast Multimedia Library (SFML) for rendering and handling user input. Ensure you have SFML. Installed and set up correctly.

## How to Run

1. Clone this repo to your local machine
2. Navigate to the project directory
3. Compile the game using C++ compiler, ensuring you link against the necessary SFML libraries.
4. Run the compiled executable.


## Code Overview

- The game initializes a ball with a texture loaded from a local path.
- The ball is set to move with a certain velocity in both the x and y directions.
- The game loop checks for window close events and updates the ball's position.
- If the ball reaches the edges of the window, it reverses its velocity to simulate a bounce.


## Future Improvements

- Add paddles for two players.
- Implement scoring mechanism
- Add sound effects for ball bounces.


