/* * * * * * * * * * * * * * * * *
 *      hkmgen.c                 *
 *	ALISTAIR CARSCADDEN          *
 *		2016 11 23               *
 *	                             *
 *	HUNT & KILL MAZE GENERATION  *
 * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void carve(char** maze, int mazeRows, int mazeCols, int carveRow, int carveCol);
void randomDirections(int list[4][2]);
int findSeed(char** maze, int mazeRows, int mazeCols, int* seedRowPtr, int* seedColPtr);
void printMaze(char** maze, int numRows);
void writeMazeToFile(char** maze, int numRows, char filename[]);
void createMaze(char filename[], int rows, int cols);

int main(int argc, char **argv) {
	srand(time(NULL));

	if(argc == 4) {
		createMaze(argv[1], atoi(argv[2]), atoi(argv[3]));
	} else {
		printf("Arguments must be: <file to write> <rows> <cols>\n");
		exit(1);
	}
	
	return 0;
}

void createMaze(char filename[], int rows, int cols) {
	if(rows < 3 || cols < 3) {
		printf("Maze must, at minimum, be 3x3.\n");
		exit(1);
	}
	
	if(rows % 2 == 0) {
		printf("Maze rows must be odd. Increasing by 1. %d --> %d\n", rows, rows + 1);
		rows++;
	}
	
	if(cols % 2 == 0) {
		printf("Maze cols must be odd. Increasing by 1. %d --> %d\n", cols, cols + 1);
		cols++;
	}
	
	char** maze;
	
	if((maze = malloc(rows * sizeof(char*))) == NULL) {
		printf("Failed to allocate memory for maze.\n");
		exit(1);
	}
	
	for(int i = 0; i < rows; i++) {
		if((maze[i] = malloc((1 + cols) * sizeof(char))) == NULL) {
			printf("Failed to allocate memory for maze.\n");
			exit(1);
		}
	}
	
	// Fill maze area with '#'s
	for(int r = 0; r < rows; r++) {
		for(int c = 0; c < cols; c++) {
			if(r < rows && c < cols)
				maze[r][c] = '#';
			else
				maze[r][c] = '\0';
		}
	}
	
	// End each row with '\0's
	for(int r = 0; r < rows; r++)
		maze[r][cols] = '\0';
	
	// Create starting hallway cell
	maze[1][1] = 'O';
	
	// Find seed positions for new hallways and then carve them until there are no seeds left
	// Seeds are wall cells with an open cell adjacent (two over on the grid)
	int seedR, seedC;
	while(findSeed(maze, rows, cols, &seedR, &seedC))
		carve(maze, rows, cols, seedR, seedC);

	// Place the start and goal in each corner
	maze[0][1] = 'S';
	maze[rows - 1][cols - 2] = 'G';
	
	// Write the maze to the given file
	writeMazeToFile(maze, rows, filename);
}

int findSeed(char** maze, int mazeRows, int mazeCols, int* seedRowPtr, int* seedColPtr) {
	int relAdjs[4][2];
	randomDirections(relAdjs);
	
	for(int r = 1; r < mazeRows; r += 2) {
		for(int c = 1; c < mazeCols; c += 2) {
			if(maze[r][c] == '#') {
				for(int i = 0; i < 4; i++) {
					int tempR = r + relAdjs[i][0] * 2;
					int tempC = c + relAdjs[i][1] * 2;
					
					if(tempR >= 0 && tempR < mazeRows && tempC >= 0 && tempC < mazeCols && maze[tempR][tempC] == 'O') {
						maze[r][c] = 'O';
						maze[r + relAdjs[i][0]][c + relAdjs[i][1]] = 'O';
						*seedRowPtr = r;
						*seedColPtr = c;
						return 1;
					}
				}
			}
		}
	}
	
	return 0;
}

void carve(char** maze, int mazeRows, int mazeCols, int carveRow, int carveCol) {
	int movePossible = 1;
	int moves[4][2];
	
	while(movePossible) {
		movePossible = 0;
		randomDirections(moves);
		
		for(int i = 0; i < 4; i++) {
			int tempR = carveRow + moves[i][0] * 2;
			int tempC = carveCol + moves[i][1] * 2;
			
			if(tempR >= 0 && tempR < mazeRows && tempC >= 0 && tempC < mazeCols && maze[tempR][tempC] == '#') {
				maze[tempR][tempC] = 'O';
				maze[carveRow + moves[i][0]][carveCol + moves[i][1]] = 'O';
				carveRow = tempR;
				carveCol = tempC;
				movePossible = 1;
				break;
			}
		}
	}
}

void randomDirections(int list[4][2]) {
	int remain = 4;
	int possibleDirections[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
	
	while(remain > 0) {
		int choose = rand() % remain;
		list[4 - remain][0] = possibleDirections[choose][0];
		list[4 - remain][1] = possibleDirections[choose][1];
		remain--;
		
		for(int i = choose; i < 4; i++) {
			possibleDirections[i][0] = possibleDirections[i + 1][0];
			possibleDirections[i][1] = possibleDirections[i + 1][1];
		}
	}
}

/* writeMazeToFile
 * purpose: writes filename (file) with text in maze (string array)
 */
 
void writeMazeToFile(char** maze, int mazeRows, char filename[]) {

	FILE* file = fopen(filename, "w");
	
	for(int i = 0; i < mazeRows; i++)
		fprintf(file, "%s\n", maze[i]);
	
	fclose(file);
}

/* printMaze
 * purpose: prints the maze
 */

void printMaze(char** maze, int mazeRows) {
	for(int i = 0; i < mazeRows; i++)
		printf("%s\n", maze[i]);
}