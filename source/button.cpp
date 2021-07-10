#include "button.h"
#include "main.h"

Button::Button(int x_cell, int y_cell, color_t color) {
    this->x_cell = x_cell;
    this->y_cell = y_cell;
    float xpos = 20.0 + 10.0 * x_cell - 5.0;
    float ypos = 20.0 + 10.0 * y_cell - 5.0;
    this->position = glm::vec3(xpos, ypos, 0.0);
    // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices

    int numberOfSides = 50;
    float x = xpos;
    float y = ypos;
    float z = 0.0f;
    float radius = 3.0f;
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

    this->object = create3DObject(GL_TRIANGLE_FAN, numberOfVertices, allCircleVertices, color, GL_FILL);
}


void Button::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= translate;
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

