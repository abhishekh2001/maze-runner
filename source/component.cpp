#include "main.h"
#include "component.h"

Component::Component(int cell_x, int cell_y, int type) {
    int xpos = 20.0 + 10.0 * cell_x - 5.0;
    int ypos = 20.0 + 10.0 * cell_y - 5.0;
    this->position = glm::vec3(xpos, ypos, 0.0);
    this->type = type;
    is_collected = false;

    int numberOfSides = 50;
    float x = xpos;
    float y = ypos;
    float z = 0.0f;
    float radius = 2.5f;
    int numberOfVertices = numberOfSides;
    
    GLfloat twicePi = 2.0f * M_PI;
    
    GLfloat circleVerticesX[numberOfVertices];
    GLfloat circleVerticesY[numberOfVertices];
    GLfloat circleVerticesZ[numberOfVertices];
    
    // circleVerticesX[0] = x;
    // circleVerticesY[0] = y;
    // circleVerticesZ[0] = z;
    
    for ( int i = 0; i < numberOfVertices; i++ )
    {
        circleVerticesX[i] = ( radius * cos( i *  twicePi / numberOfSides ) );
        circleVerticesY[i] = ( radius * sin( i * twicePi / numberOfSides ) );
        circleVerticesZ[i] = z;
    }
    
    GLfloat allCircleVertices[( numberOfVertices ) * 3];
    
    for ( int i = 0; i < numberOfVertices; i++ )
    {
        allCircleVertices[i * 3] = circleVerticesX[i];
        allCircleVertices[( i * 3 ) + 1] = circleVerticesY[i];
        allCircleVertices[( i * 3 ) + 2] = circleVerticesZ[i];
    }
    std::cout <<  "total " << numberOfVertices * 3 << std::endl;


    if (type == POWERUP) {
        color_t color = {218, 165, 32};

        this->object = create3DObject(GL_TRIANGLE_FAN, numberOfVertices, allCircleVertices, color, GL_FILL);

        float triangleVer[] = {
            -2.0f, -1.0f,  0.0f,
             2.0f, -1.0f,  0.0f,
             0.0f,  2.0f,  0.0f
        };
        color_t triCol = {255, 255, 0};
        this->object1 = create3DObject(GL_TRIANGLES, 3, triangleVer, triCol, GL_FILL);
    }
    if (type == OBSTACLE) {
        color_t color = {0, 0, 0};
        this->object = create3DObject(GL_TRIANGLE_FAN, numberOfVertices, allCircleVertices, color, GL_FILL);


        float triangleVer[] = {
            -2.5f, -2.0f,  0.0f,
             2.5f, -2.0f,  0.0f,
             0.0f, -3.0f,  0.0f
        };
        color_t triCol = {255,69, 0};
        this->object1 = create3DObject(GL_TRIANGLES, 3, triangleVer, triCol, GL_FILL);
    }
}

void Component::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= translate;
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
    draw3DObject(this->object1);
}