#include <stdbool.h>

/*
 * Enum to represent the four directions
 * Here is an example of how to use an enum:
 *
 * Direction dir = EAST;
 * if (dir == EAST) {
 *     printf("Heading EAST!");
 * }
 */
typedef enum {
	NORTH = 0,
	SOUTH = 1,
	WEST = 2,
	EAST = 3
} Direction;

/*
 * Enum to represent whether a connection is
 * open, a wall, or has yet to be initialized;
 */
typedef enum {
	UNINITIALIZED = 0,
	OPEN = 1,
	WALL = 2,
} Connection;

/*
 * Struct to represent a room in the maze.
 * Must store:
 * 	- The location of the room
 * 	- Whether or not this room has been visited
 *	- The connections to neighboring rooms in all
 *	  four directions.
 */
struct maze_room {
	int row;
	int col;
	bool visited;
	Connection north;
	Connection south;
	Connection east;
	Connection west;
	struct maze_room* next;
};

int get_index(int row, int col, int num_cols);

int is_in_range(int row, int col, int num_rows, int num_cols);

struct maze_room *get_neighbor(struct maze_room maze[], int num_cols,
				struct maze_room *room, Direction dir);

bool check_bounds(int row, int col, int num_rows, int num_cols, Direction dir);

void update_connection (struct maze_room* m, Direction dir, Connection c);

Connection get_connection(struct maze_room* m, Direction dir);

int check_connect(Connection c);

void zero_room(struct maze_room* room);

void zero_maze(struct maze_room* maze, int rows, int cols);

bool hex_to_dir (int dir, unsigned int hex);

void traverse(struct maze_room* room, FILE* fp);

char* from_connect(Connection c);

void print_room(struct maze_room room);

void print_maze(struct maze_room* maze, int rows, int cols);
