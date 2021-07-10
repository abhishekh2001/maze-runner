#include "enemy.h"
#include "main.h"
#include "maze.h"


Enemy::Enemy(int cell_x, int cell_y) {
    this->set_cell_x(cell_x);
    this->set_cell_y(cell_y);
    float x_curr = 20.0 + 10.0 * this->get_cell_x() - 5.0;
    float y_curr = 20.0 + 10.0 * this->get_cell_y() - 5.0;
    this->set_y_prev(y_curr);
    this->set_y_curr(y_curr);
    this->set_x_prev(x_curr);
    this->set_x_curr(x_curr);
    this->position = glm::vec3(x_curr, y_curr, 0.0f);

    moving_status = false;
    
    int numberOfSides = 50;
    float x = 0;
    float y = -3.0f;
    float z = 0.0f;
    float radius = 1.9f;
    int numberOfVertices = numberOfSides;
    
    GLfloat twicePi = 2.0f * M_PI;
    
    GLfloat circleVerticesX[numberOfVertices];
    GLfloat circleVerticesY[numberOfVertices];
    GLfloat circleVerticesZ[numberOfVertices];
    
    circleVerticesX[0] = x;
    circleVerticesY[0] = y;
    circleVerticesZ[0] = z;
    
    for ( int i = 0; i < numberOfVertices; i++ )
    {
        circleVerticesX[i] = x + ( radius * cos( i *  twicePi / numberOfSides ) );
        circleVerticesY[i] = y + ( radius * sin( i * twicePi / numberOfSides ) );
        circleVerticesZ[i] = z;
    }
    
    GLfloat allCircleVertices[( numberOfVertices ) * 3];
    
    for ( int i = 0; i < numberOfVertices; i++ )
    {
        allCircleVertices[i * 3] = circleVerticesX[i];
        allCircleVertices[( i * 3 ) + 1] = circleVerticesY[i];
        allCircleVertices[( i * 3 ) + 2] = circleVerticesZ[i];
    }

    color_t color = {200, 57, 0};

    this->object = create3DObject(GL_TRIANGLE_FAN, numberOfVertices, allCircleVertices, color, GL_FILL);

    float bodyVertices[] = {
        -2.0f, -2.5f, 0.0f,
        -2.0f,  2.0f, 0.0f,
         2.0f, -2.5f, 0.0f,
        -2.0f,  2.0f, 0.0f,
         2.0f, -2.5f, 0.0f,
         2.0f,  2.0f, 0.0f,

        -2.0f, 2.9f, 0.0f,
        -2.0f, 2.0f, 0.0f,
        -1.5f, 2.9f, 0.0f,
        -2.0f, 2.0f, 0.0f,
        -1.5f, 2.9f, 0.0f,
        -1.5f, 2.0f, 0.0f,

         2.0f, 2.9f, 0.0f,
         2.0f, 2.0f, 0.0f,
         1.5f, 2.9f, 0.0f,
         2.0f, 2.0f, 0.0f,
         1.5f, 2.9f, 0.0f,
         1.5f, 2.0f, 0.0f,
    };
    this->body = create3DObject(GL_TRIANGLES, 18, bodyVertices, color, GL_FILL);

    float helmetVertices[] = {
         0.0f, -3.0f, 0.0f,
         2.0f, -3.0f, 0.0f,
         0.0f, -4.0f, 0.0f,

         0.0f, -4.0f, 0.0f,
         2.0f, -3.0f, 0.0f,
         2.0f, -4.0f, 0.0f
    };
    color_t hColor = {255, 255, 230};
    this->helmet = create3DObject(GL_TRIANGLES, 6, helmetVertices, hColor, GL_FILL);
}

bool inMaze(int cellx, int celly, Maze& maze) {
    return cellx >= 0 && cellx < maze.get_width() && celly >= 0 && celly < maze.get_height();
}

// Implement Lee's shortest path finding algortihm
void Enemy::getBFSPath(int x, int y, int destx, int desty, Maze &maze) {
    std::vector<std::vector<int>> arr(maze.get_height(), std::vector<int> (maze.get_width(), 0));
    arr[destx][desty] = 9;
    std::queue<Point*> q;

    q.push(new Point(x, y, nullptr));

    while (!q.empty()) {
        Point *p = q.front();
        q.pop();

        if (arr[p->x][p->y] == 9) {
            bfs_path = {};
            Point *x = p;
            while (x->parent != nullptr) {
                bfs_path.push({x->x, x->y});
                x = x->parent;
            }
            return;
        }

        if (maze.cellXY(p->x, p->y).road[right] && inMaze(p->x+1, p->y, maze) && (arr[p->x+1][p->y] == 0 || arr[p->x+1][p->y] == 9)) {
            arr[p->x][p->y] = -1;
            q.push(new Point(p->x+1, p->y, p));

            // std::cout << "going to " << p->x+1 << " " << p->y << " from " << p->x << " " << p->y << std::endl;
        }
        if (maze.cellXY(p->x, p->y).road[left] && inMaze(p->x-1, p->y, maze) && (arr[p->x-1][p->y] == 0 || arr[p->x-1][p->y] == 9)) {
            arr[p->x][p->y] = -1;
            q.push(new Point(p->x-1, p->y, p));

            // std::cout << "going to " << p->x-1 << " " << p->y << " from " << p->x << " " << p->y << std::endl;
        }
        if (maze.cellXY(p->x, p->y).road[up] && inMaze(p->x, p->y+1, maze) && (arr[p->x][p->y+1] == 0 || arr[p->x][p->y+1] == 9)) {
            arr[p->x][p->y] = -1;
            q.push(new Point(p->x, p->y+1, p));

            // std::cout << "going to " << p->x << " " << p->y+1 << " from " << p->x << " " << p->y << std::endl;
        }
        if (maze.cellXY(p->x, p->y).road[down] && inMaze(p->x, p->y-1, maze) && (arr[p->x][p->y-1] == 0 || arr[p->x][p->y-1] == 9)) {
            arr[p->x][p->y] = -1;
            q.push(new Point(p->x, p->y-1, p));

            // std::cout << "going to " << p->x+1 << " " << p->y-1 << " from " << p->x << " " << p->y << std::endl;
        }
    }

}


// Compute path every single time.
// Move the enemy unit from
// information stored in bfs_path
void Enemy::updateMovement(int destx, int desty, Maze &maze) {
    if (this->moving_status)
        return;
    
    // std::cout << "before getting BFS path, xcurr, ycurr: " << this->x_curr << " " << this->y_curr << " dest: " << destx << " " << desty << std::endl;
    // std::cout << "before getting path " << desty << std::endl;
    this->getBFSPath(this->cell_x, this->cell_y, destx, desty, maze);

    // std::cout << "updating movement, empty? " << this->bfs_path.empty() << std::endl;
    if (this->bfs_path.empty()) {
        set_direction_to_move(-1);
        return;
    }

    auto pt = this->bfs_path.top();
    bfs_path.pop();

    // std::cout << "inside cellx, celly: " << cell_x << " " << cell_y << " to: " << pt.first << " " << pt.second << std::endl;

    // char ch; std::cin >> ch;
    if (pt.first == cell_x + 1) {
        set_direction_to_move(ENEMY_RIGHT);
        set_cell_x(get_cell_x() + 1);
    } else if (pt.first == cell_x - 1) {
        set_direction_to_move(ENEMY_LEFT);
        set_cell_x(get_cell_x() - 1);
    } else if (pt.second == cell_y + 1) {
        set_direction_to_move(ENEMY_UP);
        set_cell_y(get_cell_y() + 1);
    } else if (pt.second == cell_y - 1) {
        set_direction_to_move(ENEMY_DOWN);
        set_cell_y(get_cell_y() - 1);
    } else {
        set_direction_to_move(-1);
    }
}

void Enemy::set_direction_to_move(int direction) {
    direction_to_move = direction;
    moving_status = true;
}


// Same as from person.cpp
// But uses the path stored in
void Enemy::move(glm::mat4 VP) {
    float speed = 0.2f;
    if      (direction_to_move == ENEMY_UP)
        y_curr += speed;
    else if (direction_to_move == ENEMY_RIGHT)
        x_curr += speed;
    else if (direction_to_move == ENEMY_LEFT)
        x_curr -= speed;
    else if (direction_to_move == ENEMY_DOWN)
        y_curr -= speed;
    else
        moving_status = false;
    
    
    position = glm::vec3(x_curr, y_curr, 0.0);

    if (fabs(x_curr - x_prev) >= 10.0 || fabs(y_curr - y_prev) >= 10.0) {
        if (direction_to_move == ENEMY_RIGHT)
            x_curr = x_prev + 10.0f;
        else if (direction_to_move == ENEMY_LEFT)
            x_curr = x_prev - 10.0f;
        
        if (direction_to_move == ENEMY_UP)
            y_curr = y_prev + 10.0f;
        else if (direction_to_move == ENEMY_DOWN)
            y_curr = y_prev - 10.0f;
        
        x_prev = x_curr;
        y_prev = y_curr;

        position = glm::vec3(x_curr, y_curr, 0.0);
        moving_status = false;
    }

    draw(VP);
}

void Enemy::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= translate;
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
    draw3DObject(this->body);
    draw3DObject(this->helmet);
}
