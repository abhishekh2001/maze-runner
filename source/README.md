# CG assignment 1
## Controls
<kbd>up</kbd> move down

<kbd>down</kbd> move up

<kbd>left</kbd> move left

<kbd>right</kbd> move right

<kbd>v</kbd> toggle light

<kbd>q</kbd> quit

## About the game

* Player is spawned at a starting location
* You must move towards the exit of the maze, which is situated at the *left or top ends of the maze*.
* You must complete two tasks, kill the enemy and spawn the power ups & obstacles.
* You have 5 health points and you can skillfully counter the enemy by dashing through him, which costs one health point. 
* Remaining at the same location alongside the enemy will result in imminent death.
* Remaining in darkness will boost your points.
* Kill enemy by standing atop of the *green* button
* Powerups and obstacles are spawned by the *red* button.
* You have 30 seconds to reach the exit.

The powerups are gold coins embedded with a triangle that boost your score, while the obstacles are bombs that reduce your points.

## Instructions to run the game
```bash
mkdir build
cd build
cmake ..
make
./Hello-World
```