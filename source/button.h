#include "main.h"

#ifndef BALL_H
#define BALL_H


class Button {
public:
    Button() {}
    Button(int cell_x, int cell_y, color_t color);
    glm::vec3 position;
    int get_x_cell() { return x_cell; }
    int get_y_cell() { return y_cell; }
    void draw(glm::mat4 VP);
private:
    VAO *object;
    int x_cell, y_cell;
};

#endif // BALL_H
