// TestLibwincork.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\..\src\cork.h"

#include <iostream>

void Assert(bool f) {
	if (!f) {
		std::cerr << "Fail!" << std::endl;
		exit(1);
	}
}

int main()
{
	if (!CorkCommander::LoadMesh("ID1", "..\\..\\samples\\ballA.off")) {
		std::cerr << "Failed to load file 1\n";
		return 1;
	}

	if (!CorkCommander::LoadMesh("ID2", "..\\..\\samples\\ballB.off")) {
		std::cerr << "Failed to load file 2\n";
		return 1;
	}

	Assert(CorkCommander::Union("ID1", "ID2", "Out1"));
	Assert(CorkCommander::SaveMesh("Out1", "Union.stl"));
	Assert(CorkCommander::DeleteMesh("Out1"));
	std::cout << "Union done" << std::endl;

	Assert(CorkCommander::Intersection("ID1", "ID2", "Out1"));
	Assert(CorkCommander::SaveMesh("Out1", "Intersection.stl"));
	Assert(CorkCommander::DeleteMesh("Out1"));
	std::cout << "Intersection done" << std::endl;

	Assert(CorkCommander::Difference("ID1", "ID2", "Out1"));
	Assert(CorkCommander::SaveMesh("Out1", "Difference.stl"));
	Assert(CorkCommander::DeleteMesh("Out1"));
	std::cout << "Difference done" << std::endl;

	Assert(CorkCommander::Xor("ID1", "ID2", "Out1"));
	Assert(CorkCommander::SaveMesh("Out1", "Xor.stl"));
	std::cout << "Xor done" << std::endl;

	Assert(CorkCommander::TranslateZ("Out1", 5.0));
	Assert(CorkCommander::SaveMesh("Out1", "difference_moved.stl"));
	std::cout << "Move done" << std::endl;

	Assert(CorkCommander::CopyMesh("Out1", "Out2"));

	Assert(CorkCommander::Rotate180X("Out1"));
	Assert(CorkCommander::SaveMesh("Out1", "Difference_moved_and_rotatedX.stl"));
	std::cout << "RotateX done" << std::endl;

	Assert(CorkCommander::Rotate180Y("Out2"));
	Assert(CorkCommander::SaveMesh("Out2", "difference_moved_and_rotatedY.stl"));
	std::cout << "RotateY done" << std::endl;

	return 0;
}

