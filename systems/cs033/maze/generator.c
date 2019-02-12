#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "generator.h"

#define DIRS 4

/*
 * Given a direction, get its opposite
 *
 * Parameters:
 *	- dir: direction
 *
 * Returns:
 *	- the opposite direction to dir
 */
Direction get_opposite_dir(Direction dir) {
  if(dir == NORTH)
    return SOUTH;
  else if(dir == SOUTH)
    return NORTH;
  else if(dir == EAST)
    return WEST;
  else
    return EAST;
}

/*
 * Given an array of the four directions, randomizes the order of the directions
 *
 * Parameters:
 *	- directions: an array of Direction enums of size 4
 *
 * Returns:
 *	- nothing - the array should be shuffled in place
 */
void shuffle_array(Direction directions[]) {
  for(int i = 0; i < DIRS; i++) {
    // get random number
    int index = rand() % DIRS;

    // switch values
    Direction temp = directions[i];
    directions[i] = directions[index];
    directions[index] = temp;
  }
}

/*
 * Recursive function for stepping through a maze and setting its connections/walls using
 * the drunken_walk algorithm
 *
 * Parameters:
 *	- maze: an array of maze_room structs representing your maze
 * 	- row: row of the current room
 *	- col: column of the current room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *
 * Returns:
 *	- nothing - the state of your maze should be saved in the maze array being passed
 *	  in (make sure to use pointers correctly!).
 */
void drunken_walk(struct maze_room maze[], int row, int col, int num_rows, int num_cols) {
  // create a pointer to the room
  struct maze_room* m = &maze[get_index(row, col, num_cols)];

  // create an array of directions and randomize it
  Direction dir[] = {NORTH, SOUTH, EAST, WEST};
  shuffle_array(dir);

  // initialize values
  m->row = row;
  m->col = col;
  m->visited = true;

  for(int i = 0; i < DIRS; i++) {
    struct maze_room* neighbor = get_neighbor(maze, num_cols, m, dir[i]);

    if(!check_bounds(row, col, num_rows, num_cols, dir[i]))
      update_connection(m, dir[i], WALL);
    else if(!is_in_range((neighbor-maze)/num_cols, (neighbor-maze)%num_cols, num_rows, num_cols))
      update_connection(m, dir[i], WALL);
    else {
      if(!(neighbor->visited)) {
        update_connection(m, dir[i], OPEN);
	drunken_walk(maze, (neighbor-maze)/num_cols, (neighbor-maze)%num_cols, num_rows, num_cols);
      }
      else {
	Direction opposite = get_opposite_dir(dir[i]);

	Connection connect = get_connection(neighbor, opposite);
	if(connect == WALL || connect == OPEN)
	  update_connection(m, dir[i], connect);
	else
	  update_connection(m, dir[i], WALL);
      }
    }
  }
}

/*
 * Represents a maze_room struct as a hexadecimal number based on its connections
 *
 * Parameters:
 *	- room: a struct maze_room to be converted to a hexadecimal number
 *
 * Returns:
 *	- the hexadecimal representation of room
 */
int encode_room(struct maze_room room) {
  return (check_connect(room.east) * 8 + check_connect(room.west) * 4 +
	  check_connect(room.south) * 2 + check_connect(room.north));
}

/*
 * Represents a maze_room array as a hexadecimal array based on its connections
 *
 * Parameters:
 *	- maze: an array of maze_room structs representing the maze
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- result: an array of hexadecimal numbers representing the maze
 *
 * Returns:
 *	- nothing - each maze_room in the maze should be represented
 *	  as a hexadecimal number and put in nums at the corresponding index
 */
void encode_maze(struct maze_room maze[], int num_rows, int num_cols, int result[]) {
  for(int i = 0; i < num_rows; i++) {
    for(int k = 0; k < num_cols; k++) {
      result[get_index(i, k, num_cols)] = encode_room(maze[get_index(i, k, num_cols)]);
    }
  }
}

/*
 * Main function
 *
 * Parameters:
 *	- argc: the number of command line arguments - for this function 4
 *	- **argv: a pointer to the first element in the command line
 *            arguments array - for this function:
 *		      ["generator", <output file>, <number of rows>, <number of columns>]
 *
 * Returns:
 *	- 0 if program exits correctly, 1 if there is an error
 */
int main(int argc, char **argv) {
        // seed random number
        srand(time(NULL));

	char *file_name;
	int num_rows;
	int num_cols;

	if (argc != 4) {
		printf("Incorrect number of arguments.\n");
		printf("./generator <output file> <number of rows> <number of columns>\n");
		return 1;
	} else {
		file_name = argv[1];
		num_rows = atoi(argv[2]);
		num_cols = atoi(argv[3]);
	}

	if(num_rows <= 0 || num_cols <= 0) {
	  fprintf(stderr, "Invalid size of maze.\n");
	  return 1;
	}

	// create empty maze array
	struct maze_room maze[num_rows * num_cols];
        zero_maze(maze, num_rows, num_cols);

	// create maze
	drunken_walk(maze, 0, 0, num_rows, num_cols);

	// create empty array of ints and encode it
	int encoded_maze[num_rows * num_cols];
	encode_maze(maze, num_rows, num_cols, encoded_maze);

	// open file
	FILE* fp = fopen(file_name, "w");

	// check for NULL fp
	if(!fp) {
          fprintf(stderr, "Could not open file.\n");
	  return 1;
 	}

	// write out to file
	for(int i = 0; i < num_rows; i++) {
	  for(int k = 0; k < num_cols; k++) {
	    fprintf(fp, "%x", encoded_maze[get_index(i, k, num_cols)]);
	  }
	  fprintf(fp, "\n");
	}

	// close file
	fclose(fp);

	return 0;
}
