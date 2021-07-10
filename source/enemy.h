#include "main.h"
#include "maze.h"

#ifndef ENEMY_H
#define ENEMY_H

#define ENEMY_UP    0
#define ENEMY_RIGHT 2
#define ENEMY_LEFT  3
#define ENEMY_DOWN  1

class Point {
public:
    int x;
    int y;
    Point *parent;
    Point(int x, int y, Point *p) {
        this->x = x;
        this->y = y;
        this->parent = p;
    }
    Point* get_parent() {
        return this->parent;
    }
};

class Enemy {
public:
    glm::vec3 position;
    Enemy() {}
    Enemy(int cell_x, int cell_y);
    void move(glm::mat4 VP);
    void draw(glm::mat4 VP);
    bool get_moving_status() { return moving_status; }
    int get_cell_x() { return cell_x; }
    int get_cell_y() { return cell_y; }
    void set_cell_x(int val) { cell_x = val; }
    void set_cell_y(int val) { cell_y = val; }
    void set_direction_to_move(int direction);
    void set_position(float x, float y);
    void set_moving_status(bool val)  { moving_status = val; }
    int get_x_curr()             { return x_curr; }
    void set_x_curr(float val)   { x_curr = val;  }
    int get_y_curr()             { return y_curr; }
    void set_y_curr(float val)   { y_curr = val;  }
    int get_x_prev()             { return x_prev; }
    void set_x_prev(float val)   { x_prev = val;  }
    int get_y_prev()             { return y_prev; }
    void set_y_prev(float val)   { y_prev = val;  }
    void getBFSPath(int x, int y, int destx, int desty, Maze &maze);
    void updateMovement(int destx, int desty, Maze &maze);

private:
    VAO *object;
    VAO *helmet;
    VAO *body;
    float x_curr, y_curr;
    float x_prev, y_prev;
    int cell_x, cell_y;
    bool moving_status;
    int direction_to_move;  // must be one of 1, 2, 3, 4
    std::stack<std::pair<int, int>> bfs_path;
};

#endif  // ENEMY_H
