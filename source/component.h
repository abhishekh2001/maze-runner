#include "main.h"
// class for powerups & obstacles.

#ifndef COMPONENT_H
#define COMPONENT_H

#define POWERUP 1
#define OBSTACLE 2

class Component {
public:
    Component() {}
    Component(int cell_x, int cell_y, int type);
    glm::vec3 position;
    void draw(glm::mat4 VP);
    int get_type() { return type; }
    bool get_is_collected()         { return is_collected; }
    void set_is_collected(bool val) { is_collected = val; }
private:
    VAO *object;
    VAO *object1;
    int type;
    bool is_collected;
};

#endif  // COMPONENT_H