#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "solver.h"

#define NORTH_VALUE 1
#define SOUTH_VALUE 2
#define WEST_VALUE 4
#define EAST_VALUE 8

#define DIRS 4

/*
 * Given a pointer to a maze_room, set its connections in all four directions based
 * on the hex value passed in.
 *
 * For example:
 *
 * 		create_room_connections(&maze[3][4], 0xb)
 *
 * 0xb is 1011 in binary, which means we have walls on all sides except the WEST.
 * This means that every connection in maze[3][4] should be set to 1, except the WEST
 * connection, which should be set to 0.
 *
 * See the handout for more details about our hexadecimal representation, as well as
 * examples on how to use bit masks to extract information from this hexadecimal
 * representation.
 *
 * Parameters:
 *	- room: pointer to the current room
 *	- hex: hexadecimal integer (between 0-15 inclusive) that represents the connections
 *		   in all four directions from the given room.
 *
 * Returns:
 *	- nothing. The connections should be saved in the maze_room struct pointed to by
 *	  the parameter (make sure to use pointers correctly!).
 */
void create_room_connections(struct maze_room *room, unsigned int hex) {
  int dirvs[] = {NORTH_VALUE, SOUTH_VALUE, EAST_VALUE, WEST_VALUE};
  Direction dirs[] = {NORTH, SOUTH, EAST, WEST};

  for(int i = 0; i < DIRS; i++) {
    if(hex_to_dir(dirvs[i], hex))
      update_connection(room, dirs[i], WALL);
    else
      update_connection(room, dirs[i], OPEN);
  }
}

/*
 * Recursive depth-first search algorithm for solving your maze.
 * This function should also print out either every visited room as it goes, or
 * the final pruned solution, depending on whether the FULL macro is set.
 *
 * Note that you may need to add extra parameters to this function's signature,
 * depending on how you choose to store the pruned solution.
 *
 * See handout for more details, as well as a pseudocode implementation.
 *
 * Parameters:
 *	- row: row of the current room
 *	- col: column of the current room
 *	- goal_row: row of the goal room
 *	- goal_col: col of the goal room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- maze: an array of maze_room structs representing your maze
 *	- file: the file to write the solution to
 *
 * Returns:
 *	- 1 if the current branch finds a valid solution, 0 if no branches are valid.
 */
int dfs(int row, int col, int goal_row, int goal_col, int num_rows, int num_cols,
		struct maze_room maze[], FILE *file) {
  if(row == goal_row && col == goal_col) {
    fprintf(file, "%i, %i\n", row, col);
    return true;
  }
  else {
    // pointer to the current room
    struct maze_room* m = &maze[get_index(row, col, num_cols)];

    // mark as visited
    m->visited = true;

    Direction dirs[] = {NORTH, SOUTH, EAST, WEST};

    for(int i = 0; i < DIRS; i++) {
      struct maze_room* neighbor = get_neighbor(maze, num_cols, m, dirs[i]);

      if(get_connection(m, dirs[i]) == OPEN && !(neighbor->visited)) {
	fprintf(file, "%i, %i\n", row, col);

	if(dfs(neighbor->row, neighbor->col, goal_row, goal_col, num_rows, num_cols, maze, file)) {
          m->next = neighbor;
	  return true;
	}
      }
    }

    fprintf(file, "%i, %i\n", row, col);
    return false;
  }
}


/*
 * Main function
 *
 * Parameters:
 *	- argc: the number of command line arguments - for this function 9
 *	- **argv: a pointer to the first element in the command line
 *            arguments array - for this function:
 *		      ["solver", <input maze file>, <number of rows>, <number of columns>
 *				 <output path file>, <starting row>, <starting column>,
 *				 <ending row>, <ending column>]
 *
 * Returns:
 *	- 0 if program exits correctly, 1 if there is an error
 */

int main(int argc, char **argv) {
	int num_rows,num_cols,start_row,start_col,goal_row,goal_col;
	char *maze_file;
	char *path_file;
	if (argc != 9) {
		printf("Incorrect number of arguments.\n");
		printf("./solver <input maze file> <number of rows> <number of columns>");
		printf(" <output path file> <starting row> <starting column>");
		printf(" <ending row> <ending column>\n");
		return 1;
	} else {
		maze_file = argv[1];
		num_rows = atoi(argv[2]);
		num_cols = atoi(argv[3]);
		path_file = argv[4];
		start_row = atoi(argv[5]);
		start_col = atoi(argv[6]);
		goal_row = atoi(argv[7]);
		goal_col = atoi(argv[8]);
	}

	// quick error checking on some inputs
	if(num_rows <= 0 || num_cols <= 0) {
	  fprintf(stderr, "Invalid size of maze.\n");
	  return 1;
	}

	if(start_row < 0 || start_col < 0 || start_row >= num_rows || start_col >= num_cols) {
	  fprintf(stderr, "Invalid starting position.\n");
	  return 1;
	}

	if(goal_row >= num_rows || goal_col >= num_cols) {
	  fprintf(stderr, "Invalid ending position.\n");
	  return 1;
	}

	// create new maze
	struct maze_room maze[num_rows * num_cols];
	zero_maze(maze, num_rows, num_cols);

	// open maze file
        FILE* in = fopen(maze_file, "r");

	// if there's an error, such as in pointing to NULL
	if(!in)	{
	  fprintf(stderr, "Could not open file.\n");
	  return 1;
	}

	// create program representation of maze
	for(int i = 0; i < num_rows; i++) {
	  for(int k = 0; k < num_cols; k++) {
            unsigned int input_hex;

	    // check size of incoming file
	    if(!fscanf(in, "%1x", &input_hex)) {
	      fprintf(stderr, "Problem reading in character.\n");
              return 1;
	    }

	    int index = get_index(i, k, num_cols);

	    // set inintial conditions for the room
	    create_room_connections(&maze[index], input_hex);
	    maze[index].row = i;
	    maze[index].col = k;
	    maze[index].visited = false;
	  }
	}

	// close in file
	fclose(in);

	// open file
	FILE* out = fopen(path_file, "w");

	if(!out) {
	  fprintf(stderr, "Could not open file.\n");
	  return 1;
	}

	// by default FULL
        fprintf(out, "%s\n", "FULL");

	// call dfs - it should always find a path. If it doesn't, report so
	if(dfs(start_row, start_col, goal_row, goal_col, num_rows, num_cols, maze, out)) {
	  #ifdef FULL
	    fclose(out);
	  #else
	    // we want to overwrite any output from dfs function
	    fclose(out);
	    out = fopen(path_file, "w");

	    if(!out) {
	      fprintf(stderr, "Could not open file.\n");
	      return 1;
	    }

	    fprintf(out, "%s\n", "PRUNED");
	    traverse(&maze[get_index(start_row, start_col, num_cols)], out);
	    fclose(out);
	  #endif
	}
	else {
	  fclose(out);
	  out = fopen(path_file, "w");

   	  if(!out) {
	    fprintf(stderr, "Could not open file.\n");
	    return 1;
	  }

	  fprintf(out, "%s\n", "NO VALID PATHS");
	  fclose(out);
	}

	return 0;
}
