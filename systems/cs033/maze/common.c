#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
/*
 * Gets the index of a one-dimensional array that corresponds to the given row
 * and column in a two-dimensional grid. This is used to convert from a two-dimensional
 * coordinate to a one-dimensional array index. If you are using two-dimensional arrays,
 * you do not need this function - feel free to comment it out.
 *
 * Parameters:
 *	- row: row of the room being queried
 *	- col: column of the room being queried
 *	- num_cols: number of columns in the maze
 *
 * Returns:
 *	- the index in the one dimensional array that corresponds to the given
 *	  row and column
 */
int get_index(int row, int col, int num_cols) {
  return row * num_cols + col;
}

/*
 * Determines whether or not the room at [row][col] is a valid room within the maze with
 * dimensions num_rows x num_cols
 *
 * Parameters:
 *	- row: row of the current room
 *	- col: column of the current room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *
 * Returns:
 *	- 0 if room is not in the maze, 1 if room is in the maze
 */
int is_in_range(int row, int col, int num_rows, int num_cols) {
  if(row < 0 || row >= num_rows || col < 0 || col >= num_cols)
    return 0;
  else
    return 1;
}

/*
 * Given a pointer to the room and a Direction to travel in, return a pointer to the room
 * that neighbors the current room on the given Direction.
 * For example: get_neighbor(&maze[3][4], EAST) should return &maze[3][5]
 *
 * Parameters:
 *	- maze: an array of maze_room structs representing your maze
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- room: pointer to the current room
 *	- dir: Direction to get the neighbor from
 *
 * Returns:
 *	- pointer to the neighboring room
 */
struct maze_room *get_neighbor(struct maze_room maze[], int num_cols,
				struct maze_room *room, Direction dir) {
  int rows = (room - maze) / num_cols;
  int cols = (room - maze) % num_cols;

  // update position of neighbor
  if(dir == NORTH)
    rows = rows - 1;
  else if(dir == SOUTH)
    rows = rows + 1;
  else if(dir == EAST)
    cols = cols + 1;
  else
    cols = cols - 1;

  // return pointer to neighbor
  return &maze[get_index(rows, cols, num_cols)];
}

/*
 * Check special cases before calling get_neighbor. Use this function since,
 * although we picture the maze as a two dimensional space the memory is
 * laid out sequentially. This means, for example, if there's a 4x4 matrix,
 * going west from position maze[3][0] won't take you out of bounds, but will
 * actually be equivalent to maze[2][3] == maze[11].
 */
bool check_bounds(int row, int col, int num_rows, int num_cols, Direction dir) {
  if(col == 0 && dir == WEST)
    return false;
  if(col == (num_cols - 1) && dir == EAST)
    return false;
  if(row == 0 && dir == NORTH)
    return false;
  if(row == (num_rows - 1) && dir == SOUTH)
    return false;

  return true;
}

/*
 * Helper function to take a pointer to a room, direction, and connection
 * and update the connection from that particular room in that particular
 * direction. Used in drunken_walk algorithm.
 */
void update_connection(struct maze_room* m, Direction dir, Connection c) {
  switch(dir) {
    case(0):
      m->north = c;
      break;
    case(1):
      m->south = c;
      break;
    case(2):
      m->west = c;
      break;
    case(3):
      m->east = c;
      break;
    default:
      return;
  }
}

/*
 * Helper function to return the connection of a room in a
 * particular direction.
 */
Connection get_connection(struct maze_room* m, Direction dir) {
  switch(dir) {
    case(0):
      return (m->north);
    case(1):
      return (m->south);
    case(2):
      return (m->west);
    case(3):
      return (m->east);
    default:
      abort();
  }
}

/*
 * Convert connection to int. Use to encode connections and directions
 * to hex values.
 */
int check_connect(Connection c) {
  if (c == OPEN)
    return 0;
  else if (c == WALL)
    return 1;
  else
    abort();
}

/*
 * Make sure all elements in struct start at correct values
 */
void zero_room(struct maze_room* room) {
  room->row = 0;
  room->col = 0;
  room->visited = false;
  room->north = UNINITIALIZED;
  room->south = UNINITIALIZED;
  room->east = UNINITIALIZED;
  room->west = UNINITIALIZED;
  room->next = NULL;
}

void zero_maze(struct maze_room* maze, int rows, int cols) {

  for(int i = 0; i < rows; i++)
  {
    for(int k = 0; k < cols; k++)
      zero_room(&maze[get_index(i, k, cols)]);
  }
}

/*
 * Take the bitwise and of a direction and a hex value.
 */
bool hex_to_dir (int dir, unsigned int hex) {
  return (dir & hex);
}

/*
 * traverse linked list, printing out row and column
 */
void traverse(struct maze_room* room, FILE* fp) {
  if(!room)
    return;
  else {
    fprintf(fp, "%i, %i\n", room->row, room->col);
    traverse(room->next, fp);
  }
}

/*
 * Debugging functions; print out contents of room.
 */
char* from_connect(Connection c) {
  if (c == OPEN)
    return "open";
  else if (c == WALL)
    return "wall";
  else
    return "uninitialized";
}

void print_room(struct maze_room room) {
  printf("row: %i\n", room.row);
  printf("col: %i\n", room.col);

  if(room.visited)
    printf("visited: true\n");
  else
    printf("visited: false\n");

  printf("North: %s\n", from_connect(room.north));
  printf("South: %s\n", from_connect(room.south));
  printf("East: %s\n", from_connect(room.east));
  printf("West: %s\n", from_connect(room.west));
}

void print_maze(struct maze_room* maze, int rows, int cols) {
  for(int i = 0; i < rows; i++) {
    for(int k = 0; k < cols; k++) {
      print_room(maze[get_index(i, k, cols)]);
      printf("\n");
    }
  }
}
