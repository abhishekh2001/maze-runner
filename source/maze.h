#include "main.h"
#include "component.h"

#ifndef MAZE_H
#define MAZE_H


class Maze {
public:
    Maze() {}
    Maze(int width, int height);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    int get_starting_x() { return starting_x; }
    int get_starting_y() { return starting_y; }
    int get_goal_x()     { return goal_x;     }
    int get_goal_y()     { return goal_y;     }
    int get_width()      { return width;      }
    int get_height()     { return height;     }
    void generate_components();
    bool get_is_components_generated() { return is_components_generated; }
    Cell & cellXY(int x, int y) {
	    return cell[y * width + x];
    }
    Component *componentXY(int x, int y) {
        if (y * width + height < 0 || y * width + x >= width * height)
            return nullptr;
        return components[y * width + x];
    }
    void setComponentXYtoNull(int x, int y) {
        if (y * width + height < 0 || y * width + x >= width * height)
            return;
        // std::cout << "testing" << std::endl;
        if (components[y * width + x] != nullptr) {
            delete components[y * width + x];
            components[y * width + x] = nullptr;
        }
        // std::cout << "done testing" << std::endl;
    }
private:
    VAO *object;
    static Cell *cell;
    Component *components[1000];
    static int width, height;	// the size of maze
    static int starting_x, starting_y;	// starting position
    static int goal_x, goal_y;	// position of goal
    static int *chosen;
    static int state;
    static float bgr[3];
    bool is_components_generated;
    void remove_wall( std::vector<float> &vertices,
                 std::vector<float> &colorData,
                 int x, int y, int dest );
    void generate_maze();
    void maze_edges(std::vector<float> &vertices, std::vector<float> &colorData);
};


#endif // MAZE_H
