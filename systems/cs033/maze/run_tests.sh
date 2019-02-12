#!/usr/bin/env bash

./generator maze.txt 1 1
./solver maze.txt 1 1 sol.txt 0 0 0 0
cs033_maze_validator maze.txt sol.txt 0 0 0 0
echo "------------"

./generator maze.txt 1 1
./solver_full maze.txt 1 1 sol.txt 0 0 0 0
cs033_maze_validator maze.txt sol.txt 0 0 0 0
echo "------------"

./generator maze.txt 5 5
./solver maze.txt 5 5 sol.txt 0 0 4 4
cs033_maze_validator maze.txt sol.txt 0 0 4 4
echo "------------"

./generator maze.txt 5 5
./solver maze.txt 5 5 sol.txt 2 2 4 4
cs033_maze_validator maze.txt sol.txt 2 2 4 4
echo "------------"

./generator maze.txt 5 5
./solver maze.txt 5 5 sol.txt 3 3 4 4
cs033_maze_validator maze.txt sol.txt 3 3 4 4
echo "------------"

./generator maze.txt 5 5
./solver_full maze.txt 5 5 sol.txt 0 0 4 4
cs033_maze_validator maze.txt sol.txt 0 0 4 4
echo "------------"

./generator maze.txt 5 5
./solver_full maze.txt 5 5 sol.txt 2 2 4 4
cs033_maze_validator maze.txt sol.txt 2 2 4 4
echo "------------"

./generator maze.txt 5 5
./solver_full maze.txt 5 5 sol.txt 3 3 4 4
cs033_maze_validator maze.txt sol.txt 3 3 4 4
echo "------------"

./generator maze.txt 10 12
./solver maze.txt 10 12 sol.txt 0 0 4 4
cs033_maze_validator maze.txt sol.txt 0 0 4 4
echo "------------"

./generator maze.txt 10 12
./solver maze.txt 10 12 sol.txt 2 2 4 4
cs033_maze_validator maze.txt sol.txt 2 2 4 4
echo "------------"

./generator maze.txt 10 12
./solver maze.txt 10 12 sol.txt 0 0 9 11
cs033_maze_validator maze.txt sol.txt 0 0 9 11
echo "------------"

./generator maze.txt 10 12
./solver_full maze.txt 10 12 sol.txt 0 0 9 11
cs033_maze_validator maze.txt sol.txt 0 0 9 11
echo "------------"

./generator maze.txt 10 12
./solver_full maze.txt 10 12 sol.txt 4 7 9 11
cs033_maze_validator maze.txt sol.txt 4 7 9 11
echo "------------"

./generator maze.txt 10 12
./solver_full maze.txt 10 12 sol.txt 2 2 2 2
cs033_maze_validator maze.txt sol.txt 2 2 2 2
echo "------------"

./generator maze.txt 10 12
./solver maze.txt 10 12 sol.txt 2 2 2 2
cs033_maze_validator maze.txt sol.txt 2 2 2 2
echo "------------"

./generator maze.txt 50 65
./solver maze.txt 50 65 sol.txt 0 0 49 64
cs033_maze_validator maze.txt sol.txt 0 0 49 64
echo "------------"

./generator maze.txt 50 65
./solver_full maze.txt 50 65 sol.txt 0 0 49 64
cs033_maze_validator maze.txt sol.txt 0 0 49 64
echo "------------"

./generator maze.txt 50 65
./solver maze.txt 50 65 sol.txt 15 23 49 64
cs033_maze_validator maze.txt sol.txt 15 23 49 64
echo "------------"

./generator maze.txt 50 65
./solver_full maze.txt 50 65 sol.txt 15 23 49 64
cs033_maze_validator maze.txt sol.txt 15 23 49 64
echo "------------"

./generator maze.txt 50 65
./solver maze.txt 50 65 sol.txt 43 43 43 43
cs033_maze_validator maze.txt sol.txt 43 43 43 43
echo "------------"

./generator maze.txt 50 65
./solver_full maze.txt 50 65 sol.txt 43 43 43 43
cs033_maze_validator maze.txt sol.txt 43 43 43 43
echo "------------"

rm maze.txt
./generator maze.txt 0 0
touch maze.txt
./solver_full maze.txt 50 65 sol.txt 43 43 43 43
cs033_maze_validator maze.txt sol.txt 43 43 43 43
echo "------------"

rm maze.txt
./generator maze.txt 50 65
./solver_full maze.txt 50 65 sol.txt -1 -1 43 43
cs033_maze_validator maze.txt sol.txt -1 -1 43 43
echo "------------"

rm maze.txt
./generator maze.txt 50 65
./solver_full maze.txt 50 65 sol.txt 43 43 50 65
cs033_maze_validator maze.txt sol.txt 43 43 50 65
echo "------------"
