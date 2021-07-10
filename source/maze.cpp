#include "maze.h"
#include "main.h"

int Maze::width;
int Maze::height;
int Maze::starting_x;
int Maze::starting_y;	// starting position
int Maze::goal_x;
int Maze::goal_y;	// position of goal
int *Maze::chosen;
Cell *Maze::cell;
float Maze::bgr[3];
int Maze::state;

// Overlay the open edges to give the illusion of 
// connectivity
void Maze::remove_wall(std::vector<float> &vertices,
                 std::vector<float> &colorData,
                 int x, int y, int dest) {
    std::vector<float> p1, p2;
    float openColor[3] = {0.0f, 1.0f, 0.0f};

	if (dest == up) {
		p1 = { (x+1.0f)*10.0f+0.02, (y+2)*10.0f, 0.0f };
		p2 = { (x+2.0f)*10.0f-0.02, (y+2)*10.0f, 0.0f };
	}
	if (dest == down) {
        p1 = { (x+1)*10+0.02, (y+1)*10, 0.0f };
        p2 = { (x+2)*10-0.02, (y+1)*10, 0.0f };
	}
	if (dest == right) {
        p1 = { (x+2)*10, (y+1)*10+0.02, 0.0f };
        p2 = { (x+2)*10, (y+2)*10-0.02, 0.0f };
	}
	if (dest == left) {
        p1 = { (x+1)*10, (y+1)*10+0.02, 0.0f };
        p2 = { (x+1)*10, (y+2)*10-0.0, 0.0f };
	}

    vertices.insert(vertices.end(), p1.begin(), p1.end());
    vertices.insert(vertices.end(), p2.begin(), p2.end());
    colorData.insert(colorData.end(), bgr, bgr + 3);
    colorData.insert(colorData.end(), bgr, bgr + 3);
}

// Alg to maze gen
void Maze::generate_maze() {
    int x, y;
	int dest;
	static int length = 0;
	int tmp;

	if( length == width * height) {
        state = 1;
		for(int i = 0; i < width*height; i++)
			cell[i].is_open = false;
		return;
	}

	// generate starting and goal points.
	if( length == 0 ){
		dest = rand()%2 + 1;
		if( dest == down ){
			starting_x = x = rand()%width;
			starting_y = y = height - 1;
			cellXY(x, y).road[up] = true;
			goal_x = x = rand()%width;
			goal_y = y = 0;
			cellXY(x, y).road[down] = true;
		}
		else{
			starting_x = x = width - 1;
			starting_y = y = rand()%height;
			cellXY(x, y).road[right] = true;
			goal_x = x = 0;
			goal_y = y = rand()%height;
			cellXY(x, y).road[left] = true;
		}
		chosen = new int [height * width];
		x = rand()%width;
		y = rand()%height;
		cellXY(x, y).is_open = true;
		chosen[0] = width*y + x;

		length = 1;
	}

	bool cellOpen = false;
	while (!cellOpen) {
		tmp = chosen[rand()%length];
		x = tmp % width;
		y = tmp / width;

		int new_x, new_y;
		int dest_from_new;

		// set the neighbor cell to connect
		dest = rand()%4;

		if (dest == up) {
			if( y == height-1 || cellXY(x, y + 1).is_open == true)
				continue;
			dest_from_new = down;

			new_x = x;
			new_y = y+1;
		}
		if (dest == down) {
			if( y == 0 || cellXY(x, y - 1).is_open == true )
				continue;

			dest_from_new = up;

			new_x = x;
			new_y = y-1;
		}
		if (dest == right) {
			if( x == width-1 || cellXY(x + 1, y).is_open == true )
				continue;

			dest_from_new = left;


			new_x = x + 1;
			new_y = y;
		}
		if (dest == left) {
			if( x == 0 || cellXY(x - 1,  y).is_open == true )
				continue;

			dest_from_new = right;


			new_x = x - 1;
			new_y = y;
		}

		cellXY(new_x, new_y).is_open = true;
		cellXY(new_x, new_y).road[dest_from_new] = true;
		cellXY(x, y).road[dest] = true;
		chosen[length] = width * new_y + new_x;
		length++;
		cellOpen = true;
	}
}

void Maze::maze_edges(std::vector<float> &vertices, std::vector<float> &colorData){
	int i;
	int x, y;

	for( i = 0 ; i < width*height ; i++ ){
		x = i % width;
		y = i / width;

		if(cell[i].road[right])	    remove_wall(vertices, colorData, x, y, right);
		if(cell[i].road[up]   )		remove_wall(vertices, colorData, x, y, up);
		if(cell[i].road[down] ) 	remove_wall(vertices, colorData, x, y, down);
		if(cell[i].road[left] )		remove_wall(vertices, colorData, x, y, left);
	}
}

// Fill components in maze
// when player decides
void Maze::generate_components() {
	if (is_components_generated)
		return;
	
	int num_components = rand() % 5 + 1;
	for (int i = 0; i < num_components; i++) {
		int temp = rand() % (height * width);
		int x = temp % width;
		int y = temp / width;

		if (componentXY(x, y) == nullptr) {
			components[y * width + x] = new Component(x, y, POWERUP);
		}

		temp = rand() % (height * width);
		x = temp % width;
		y = temp / width;

		if (componentXY(x, y) == nullptr) {
			components[y * width + x] = new Component(x, y, OBSTACLE);
		}
	}
	is_components_generated = true;
	std::cout << "Done generating all components" << std::endl;
}

Maze::Maze(int width, int height) {
    this->position = glm::vec3(0.0, 0.0, 0);
    this->rotation = 0;
    this->width = width;
    this->height = height;
    this->state = 0;
    this->bgr[0] = 1.0, this->bgr[1] = 0.7, this->bgr[2] = 1.0;
    this->cell = new Cell[width * height];
	this->is_components_generated = false;
	for (int i = 0; i < width * height; i++)
		components[i] = nullptr;

    std::vector<float> vertices;
    std::vector<float> colorData;
    float blockedColor[3] = {-bgr[0], -bgr[1], -bgr[2]};
    
    for(int x = 1 ; x < width+2 ; x++ ){
        vertices.push_back(x*10);
        vertices.push_back(10.0);  // glVertex2f( x*10, 10.0 );
        vertices.push_back(0.0);
        colorData.insert(colorData.end(), blockedColor, blockedColor + 3);

        vertices.push_back(x*10);
        vertices.push_back(height*10+10.0);  // glVertex2f( x*10, height*10+10.0 );
        vertices.push_back(0.0);
        colorData.insert(colorData.end(), blockedColor, blockedColor + 3);
	}
	for(int x = 1 ; x < height+2; x++ ){
        vertices.push_back(10.0);
        vertices.push_back(x*10);  // glVertex2f( 10.0 , x*10 );
        vertices.push_back(0.0);
        colorData.insert(colorData.end(), blockedColor, blockedColor + 3);

        vertices.push_back(width*10 + 10.0);
        vertices.push_back(x*10);  // glVertex2f( width*10+10.0 , x*10 );
        vertices.push_back(0.0);
        colorData.insert(colorData.end(), blockedColor, blockedColor + 3);
	}

    // vertices.clear();
    // float p1[] = {100.0f, 0.0f, 0.0f};
    // float p2[] = {100.0f, 100.0f, 0.0f};
    
    // vertices.insert(vertices.end(), p1, p1 + 3);
    // vertices.insert(vertices.end(), p2, p2 + 3);

    
    while(!state)
        generate_maze();

    maze_edges(vertices, colorData);
    this->object = create3DObject(GL_LINES, vertices.size(), &vertices[0], &colorData[0], GL_FILL);
}

void Maze::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= translate;
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);

	for (int i = 0; i < width * height; i++) {
		if (components[i]) {
			components[i]->draw(VP);
		}
	}
}