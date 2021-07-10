#include "person.h"
#include "maze.h"
#include "person.h"

Person::Person(Maze &maze) {
    x_prev = x_curr = 20.0 + 10.0 * maze.get_starting_x() - 5.0;
    y_prev = y_curr = 20.0 + 10.0 * maze.get_starting_y() - 5.0;
    cell_x = maze.get_starting_x();
    cell_y = maze.get_starting_y();
    tasks_done = false;

    // std::cout << "starting cell x, y: " << cell_x << " "<< cell_y << std::endl;
    this->position = glm::vec3(x_curr, y_curr, 0.0f);
    this->health = 5;
    this->score = 0;

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

    color_t color = {0, 57, 255};

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

void Person::set_direction_to_move(int direction) {
    direction_to_move = direction;
    moving_status = true;
}

// Uses direction_to_move
// to move the player
// from one cell to another
// (animate until moved)
void Person::move(glm::mat4 VP) {
    float speed = 1.0f;
    if      (direction_to_move == PERSON_UP)
        y_curr += speed;
    else if (direction_to_move == PERSON_RIGHT)
        x_curr += speed;
    else if (direction_to_move == PERSON_LEFT)
        x_curr -= speed;
    else if (direction_to_move == PERSON_DOWN)
        y_curr -= speed;
    
    position = glm::vec3(x_curr, y_curr, 0.0);

    if (fabs(x_curr - x_prev) >= 10.0 || fabs(y_curr - y_prev) >= 10.0) {
        if (direction_to_move == PERSON_RIGHT)
            x_curr = x_prev + 10.0f;
        else if (direction_to_move == PERSON_LEFT)
            x_curr = x_prev - 10.0f;
        
        if (direction_to_move == PERSON_UP)
            y_curr = y_prev + 10.0f;
        else if (direction_to_move == PERSON_DOWN)
            y_curr = y_prev - 10.0f;
        
        x_prev = x_curr;
        y_prev = y_curr;

        position = glm::vec3(x_curr, y_curr, 0.0);
        moving_status = false;
    }

    draw(VP);
}

void Person::draw(glm::mat4 VP) {
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
