#include "main.h"
#include "maze.h"
#include "person.h"

#ifndef LIGHTING_H
#define LIGHTING_H


class Lighting {
public:
    Lighting() {}
    Lighting(glm::vec3 mazePosition);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP, Maze &maze, Person &player);
    void set_position(float x, float y);
private:
    VAO *object;
    std::vector<std::vector<int>> lightingMap;
};

#endif // LIGHTING_H
