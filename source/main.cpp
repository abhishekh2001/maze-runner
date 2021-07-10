#include "main.h"
#include "timer.h"
// #include "ball.h"
#include "maze.h"
#include "person.h"
#include "component.h"
#include "button.h"
#include "enemy.h"
#include "lighting.h"
#include "filesystem.h"
#include "shader.h"

#define GLT_IMPLEMENTATION
#include "glText.h"

#include <ft2build.h>
#include FT_FREETYPE_H  

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

float bg_color[3] = {1.0f, 0.7f, 1.0f};

/**************************
* Customizable functions *
**************************/
Maze       maze1;
Person     player;
bool       player_has_moved = false;
Button     powerupButton;
Button     enemyVaporize;
bool       isEnemyExists = true;
Enemy      enemy;
Lighting   lighting;
bool       lightStatus = false;
int        time_left = 35;
glm::mat4  textProjection;
Shader     textShader;
color_t    powerupDropColor = {255, 0, 0};
color_t    enemyVaporizeColor = {90, 126, 122};
bool       gameOver = false;
bool       reachedEnd = true;

GLTtext *text;

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
unsigned int textVAO, textVBO;

// Ball ball1;


float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;
float camera_rotation_angle = 0;

Timer t60(1.0 / 60);
Timer t1(1.0);

// render line of text
// -------------------
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}



/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT);

    // glEnable(GL_CULL_FACE);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // RenderText(textShader, "HP:"+to_string(player.get_health()) + 
    //     ",T:"+to_string(time_left) + ",S:"+to_string(player.get_score())+
    //     ",O:"+to_string(maze1.get_is_components_generated()+!isEnemyExists)+"/2"+
    //     ",L:"+(lightStatus ? "On" : "Off"), 
    //     0.0f, 750.0f, 0.7f, glm::vec3(1.0f, 0.0f, 0.2f));
    // glDisable(GL_BLEND);
    // glDisable(GL_CULL_FACE);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up1 (0, 1, 0);

    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    // Compute Camera matrix (view)
    // Matrices.view = glm::lookAt( eye, target, up1 ); // Rotating Camera for 3D
    Matrices.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    // Don't change unless you are sure!!
    // Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render
    // ball1.draw(VP);
    maze1.draw(VP);
    powerupButton.draw(VP);
    enemyVaporize.draw(VP);
    if (isEnemyExists) {
        enemy.updateMovement(player.get_cell_x(), player.get_cell_y(), maze1);
        enemy.move(VP);
        enemy.draw(VP);
    }
    if (player.get_moving_status())
        player.move(VP);
    player.draw(VP);

    // glDisable(GL_DEPTH_TEST);
    if (lightStatus) {
        lighting.draw(VP, maze1, player);
    }
    // glEnable(GL_DEPTH_TEST);



    std::string toDisplay = "HP:"+to_string(player.get_health()); 
         
    GLTtext *text = gltCreateText();
    gltSetText(text, toDisplay.c_str());

    // Begin text drawing (this for instance calls glUseProgram)
    gltBeginDraw();

    // Draw any amount of text between begin and end
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2D(text, 0, 0, 1.0f);

    toDisplay = "Time left:"+to_string(time_left);
    gltSetText(text, toDisplay.c_str());
    gltDrawText2D(text, 0, 20, 1.0f);

    toDisplay = "Score:"+to_string(player.get_score());
    gltSetText(text, toDisplay.c_str());
    gltDrawText2D(text, 0, 40, 1.0f);

    toDisplay = "Tasks:"+to_string(maze1.get_is_components_generated()+!isEnemyExists)+"/2";
    gltSetText(text, toDisplay.c_str());
    gltDrawText2D(text, 100, 0, 1.0f);


    toDisplay = "Light: off";
    if (lightStatus)
        toDisplay = "Light: on";
    gltSetText(text, toDisplay.c_str());
    gltDrawText2D(text, 100, 40, 1.0f);
    // Finish drawing text
    gltEndDraw();

    // Deleting text
    gltDeleteText(text);

}



/* Render the scene with openGL */
/* Edit this function according to your assignment */
void drawEndScreen() {
    std::string toDisplay = reachedEnd ? "You won!" : "You Lost";
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT);

    GLTtext *text = gltCreateText();

    // Begin text drawing (this for instance calls glUseProgram)
    gltBeginDraw();

    // Draw any amount of text between begin and end


    gltSetText(text, "Game Over");
    gltDrawText2D(text, 100, 100, 2.0f);

    gltSetText(text, toDisplay.c_str());
    gltDrawText2D(text, 100, 300, 2.0f);

    toDisplay = "Score: " + to_string(player.get_score());
    gltSetText(text, toDisplay.c_str());
    gltDrawText2D(text, 100, 500, 2.0f);

    // Finish drawing text
    gltEndDraw();

    // Deleting text
    gltDeleteText(text);
}

bool is_point_in_maze(int x, int y) {
    return x >= 0 && x < maze1.get_width() && y >= 0 && y < maze1.get_height();
}

void tick_input(GLFWwindow *window) {
    int left_move  = glfwGetKey(window, GLFW_KEY_LEFT);
    int right_move = glfwGetKey(window, GLFW_KEY_RIGHT);
    int up_move    = glfwGetKey(window, GLFW_KEY_UP);
    int down_move  = glfwGetKey(window, GLFW_KEY_DOWN);

    int cell_x = player.get_cell_x();
    int cell_y = player.get_cell_y();

    if (player.get_moving_status())
        return;

    // Check if the move will lead player out of the maze
    if (cell_x == maze1.get_goal_x() && cell_y == maze1.get_goal_y() && !isEnemyExists && maze1.get_is_components_generated()) {
        if (left_move && !is_point_in_maze(cell_x-1, cell_y)) {
            gameOver = true; reachedEnd = true;
        }
        else if (right_move && !is_point_in_maze(cell_x+1, cell_y)) {
            gameOver = true; reachedEnd = true;
        }
        else if (down_move && !is_point_in_maze(cell_x, cell_y-1)) {
            gameOver = true; reachedEnd = true;
        }
        else if (up_move && !is_point_in_maze(cell_x, cell_y+1)) {
            gameOver = true; reachedEnd = true;
        }

        if (gameOver && reachedEnd) {
            return;
        }
    }
    if (left_move && maze1.cellXY(cell_x, cell_y).road[left] && is_point_in_maze(cell_x-1, cell_y)) {
        player.set_direction_to_move(PERSON_LEFT);
        player.set_cell_x(cell_x - 1);
    }
    else if (right_move && maze1.cellXY(cell_x, cell_y).road[right] && is_point_in_maze(cell_x+1, cell_y)) {
        player.set_direction_to_move(PERSON_RIGHT);
        player.set_cell_x(cell_x + 1);
    }
    else if (down_move && maze1.cellXY(cell_x, cell_y).road[down] && is_point_in_maze(cell_x, cell_y-1)) {
        player.set_direction_to_move(PERSON_DOWN);
        player.set_cell_y(cell_y - 1);
    }
    else if (up_move && maze1.cellXY(cell_x, cell_y).road[up] && is_point_in_maze(cell_x, cell_y+1)) {
        player.set_direction_to_move(PERSON_UP);
        player.set_cell_y(cell_y + 1);
    }

    // Perform functions related to player's cell location
    cell_x = player.get_cell_x();
    cell_y = player.get_cell_y();
    // std::cout << "cell x, y: " << cell_x << " " << cell_y << std::endl;
    if (maze1.componentXY(cell_x, cell_y) != nullptr) {
        if (maze1.componentXY(cell_x, cell_y)->get_type() == POWERUP)
            player.set_score(player.get_score() + 1);
        else
            player.set_score(player.get_score() - 1);
        maze1.setComponentXYtoNull(cell_x, cell_y);

        std::cout << "score: " << player.get_score() << std::endl;
    }

    if (powerupButton.get_x_cell() == player.get_cell_x() && powerupButton.get_y_cell() == player.get_cell_y()) {
        if (!maze1.get_is_components_generated())
            player.set_score(player.get_score() + 10);
        maze1.generate_components();
    }

    if (enemyVaporize.get_x_cell() == player.get_cell_x() && enemyVaporize.get_y_cell() == player.get_cell_y()) {
        isEnemyExists = false;
    }

    if (isEnemyExists && player.get_cell_x() == enemy.get_cell_x() && player.get_cell_y() == enemy.get_cell_y()) {
        player.set_health(player.get_health() - 1);
    }
}

void tick_elements() {
    // ball1.tick();
    // camera_rotation_angle += 1;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models

    // ball1       = Ball(0, 0, COLOR_RED);
    maze1         = Maze(10, 10);
    player        = Person(maze1);
    int temp1     = rand() % (maze1.get_height() * maze1.get_width());
    int temp2     = rand() % (maze1.get_height() * maze1.get_width());
    powerupButton = Button(temp1 % maze1.get_width(), temp1 / maze1.get_width(), powerupDropColor);
    enemyVaporize = Button(temp2 % maze1.get_width(), temp2 / maze1.get_width(), enemyVaporizeColor);
    enemy         = Enemy(3, 4);
    lighting      = Lighting(maze1.position);


    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("../source/shaders/shader.vert", "../source/shaders/shader.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    // glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearColor(bg_color[0], bg_color[1], bg_color[2], 1.0);
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);


    gltInit();
    gltCreateText();
    gltSetText(text, "Hello World!");


    // textShader = Shader("../source/shaders/text.vert", "../source/shaders/text.frag");
    // textProjection = glm::ortho(0.0f, static_cast<float>(800), 0.0f, static_cast<float>(800));
    // textShader.use();
    // glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    // // FreeType
    // // --------
    // FT_Library ft;
    // // All functions return a value different than 0 whenever an error occurred
    // if (FT_Init_FreeType(&ft))
    // {
    //     std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    //     return;
    // }

    // std::string font_name = "../assets/roboto.ttf";
	
	// // load font as face
    // FT_Face face;
    // if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
    //     std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    //     return;
    // }
    // else {
    //     // set size to load glyphs as
    //     FT_Set_Pixel_Sizes(face, 0, 48);

    //     // disable byte-alignment restriction
    //     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //     // load first 128 characters of ASCII set
    //     for (unsigned char c = 0; c < 128; c++)
    //     {
    //         // Load character glyph 
    //         if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    //         {
    //             std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    //             continue;
    //         }
    //         // generate texture
    //         unsigned int texture;
    //         glGenTextures(1, &texture);
    //         glBindTexture(GL_TEXTURE_2D, texture);
    //         glTexImage2D(
    //             GL_TEXTURE_2D,
    //             0,
    //             GL_RED,
    //             face->glyph->bitmap.width,
    //             face->glyph->bitmap.rows,
    //             0,
    //             GL_RED,
    //             GL_UNSIGNED_BYTE,
    //             face->glyph->bitmap.buffer
    //         );
    //         // set texture options
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //         // now store character for later use
    //         Character character = {
    //             texture,
    //             glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
    //             glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
    //             static_cast<unsigned int>(face->glyph->advance.x)
    //         };
    //         Characters.insert(std::pair<char, Character>(c, character));
    //     }
    //     glBindTexture(GL_TEXTURE_2D, 0);
    // }
    // // destroy FreeType once we're finished
    // FT_Done_Face(face);
    // FT_Done_FreeType(ft);

    // glGenVertexArrays(1, &textVAO);
    // glGenBuffers(1, &textVBO);
    // glBindVertexArray(textVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);





    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        lightStatus = !lightStatus;
    }
}

int main(int argc, char **argv) {
    srand(time(0));
    int width  = 800;
    int height = 800;

    window = initGLFW(width, height);

    initGL (window, width, height);

    // For single key press lighting enable
    glfwSetKeyCallback(window, key_callback);


    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers

        if (player.get_health() <= 0) {
            gameOver = true;
            reachedEnd = false;
        }

        if (gameOver) {
            drawEndScreen();
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
        } else {
            if (t60.processTick()) {
                // 60 fps
                // OpenGL Draw commands
                draw();
                // Swap Frame Buffer in double buffering
                glfwSwapBuffers(window);

                tick_elements();
                tick_input(window);
            }

            if (t1.processTick()) {
                time_left--;

                if (time_left <= 0) {
                    gameOver = true;
                    reachedEnd = false;
                }

                if (lightStatus) {
                    player.set_score(player.get_score() + 1);
                }
            }
        }
        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
           (abs(a.y - b.y) * 2 < (a.height + b.height));
}

void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left1   = screen_center_x - 4 / screen_zoom;
    float right1  = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::ortho(0.0f, 120.0f, 120.0f, 0.0f, 0.5f, 5.0f);
    // Matrices.projection = glm::ortho(0.0f, 120.0f, 120.0f, 0.0f, 0.5f, 4.5f);
    // Matrices.projection = glm::ortho(left1, right1, bottom, top);
}
