#include "lighting.h"
#include "main.h"
#include "maze.h"
#include "person.h"

Lighting::Lighting(glm::vec3 mazePosition) {
    this->position = glm::vec3(mazePosition.x, mazePosition.y, 0.7f);
}

bool is_inMaze(int cellx, int celly, Maze &maze) {
    return cellx >= 0 && cellx < maze.get_width() && celly >= 0 && celly < maze.get_height();
}

void Lighting::draw(glm::mat4 VP, Maze &maze, Person &player) {
    this->lightingMap = std::vector<std::vector<int>>(maze.get_height()+2, std::vector<int> (maze.get_width()+2, 0));
    int cellx = player.get_cell_x(), celly = player.get_cell_y();
    this->lightingMap[celly][cellx] = 1;

    auto cell = maze.cellXY(cellx, celly);
    if (cell.road[left] && is_inMaze(celly, cellx-1, maze))
        this->lightingMap[celly][cellx-1] = 1;
    if (cell.road[right] && is_inMaze(celly, cellx+1, maze))
        this->lightingMap[celly][cellx+1] = 1;
    if (cell.road[up] && is_inMaze(celly+1, cellx, maze))
        this->lightingMap[celly+1][cellx] = 1;
    if (cell.road[down] && is_inMaze(celly-1, cellx, maze))
        this->lightingMap[celly-1][cellx] = 1;

    std::vector<float> vertices;
    std::vector<float> colorData;

    int width = maze.get_width(), height = maze.get_height();
    for( int i = 0 ; i < width*height ; i++ ){
		int x = i % width;
		int y = i / width;

        if (!this->lightingMap[y][x]) {
            std::vector<float> p1, p2, p3;

            p1 = { (x+1.0f)*10.0f, (y+2)*10.0f, 0.0f };
		    p2 = { (x+2.0f)*10.0f, (y+2)*10.0f, 0.0f };
            p3 = { (x+1)*10, (y+1)*10, 0.0f  };

            vertices.insert(vertices.end(), p1.begin(), p1.end());
            vertices.insert(vertices.end(), p2.begin(), p2.end());
            vertices.insert(vertices.end(), p3.begin(), p3.end());

            p1 = { (x+2.0f)*10.0f, (y+2)*10.0f, 0.0f };
		    p2 = { (x+2.0f)*10.0f, (y+1)*10.0f, 0.0f };
            p3 = { (x+1)*10, (y+1)*10, 0.0f  };

            vertices.insert(vertices.end(), p1.begin(), p1.end());
            vertices.insert(vertices.end(), p2.begin(), p2.end());
            vertices.insert(vertices.end(), p3.begin(), p3.end());

        }
    }

    color_t black = {0, 0, 0};
    this->object = create3DObject(GL_TRIANGLES, vertices.size(), &vertices[0], black);

    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);
    Matrices.model *= translate;
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}