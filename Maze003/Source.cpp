#include <iostream>
#include <fstream>
#include "MazeObject.h"

int main() {
	MazeObject MO;

	MazeObject::SizeType W = 1920/3;
	MazeObject::SizeType H = 1080/3;

	MO.Mining(W, H);

	std::ostream* os = nullptr;

#if 0

	os = &std::cout;
	ShowMaze(*os,MO);
#else
	std::ofstream ofs("Maze.pbm");
	os = &ofs;
	MazeToPBM(*os, MO);
#endif
	return 0;
}