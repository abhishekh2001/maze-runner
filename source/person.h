#include "main.h"
#include "maze.h"

#ifndef PERSON_H
#define PERSON_H

#define PERSON_UP    0
#define PERSON_RIGHT 2
#define PERSON_LEFT  3
#define PERSON_DOWN  1

class Person {
public:
    glm::vec3 position;
    Person() {}
    Person(Maze &maze);
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
    bool get_tasks_done() { return tasks_done; }
    int get_health()         { return health; }
    void set_health(int val) { health = val; }
    int get_score()          { return score; }
    void set_score(int val)  { score = val;  }
    int get_x_curr()             { return x_curr; }
    void set_x_curr(float val)   { x_curr = val;  }
    int get_y_curr()             { return y_curr; }
    void set_y_curr(float val)   { y_curr = val;  }
    int get_x_prev()             { return x_prev; }
    void set_x_prev(float val)   { x_prev = val;  }
    int get_y_prev()             { return y_prev; }
    void set_y_prev(float val)   { y_prev = val;  }

private:
    VAO *object;
    VAO *body;
    VAO *helmet;
    float x_curr, y_curr;
    float x_prev, y_prev;
    int health, score;
    bool tasks_done;
    int cell_x, cell_y;
    bool moving_status;
    int direction_to_move;  // must be one of 1, 2, 3, 4
};

#endif  // PERSON_H
