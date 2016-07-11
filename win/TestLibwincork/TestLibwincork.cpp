// TestLibwincork.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\..\src\cork.h"

#include <iostream>

int main()
{
	CorkTriMesh in1;
	CorkTriMesh in2;
	CorkTriMesh out1;

	if (!loadMesh("..\\..\\samples\\ballA.off", in1)) {
		std::cerr << "Failed to load file 1\n";
		return 1;
	}

	if (!loadMesh("..\\..\\samples\\ballB.off", in2)) {
		std::cerr << "Failed to load file 2\n";
		return 1;
	}

	computeUnion(in1, in2, &out1);
	saveMesh("union.stl", out1);
	freeCorkTriMesh(&out1);
	std::cout << "Union done" << std::endl;

	computeIntersection(in1, in2, &out1);
	saveMesh("intersection.stl", out1);
	freeCorkTriMesh(&out1);
	std::cout << "Intersection done" << std::endl;

	computeSymmetricDifference(in1, in2, &out1);
	saveMesh("xor.stl", out1);
	std::cout << "XOR done" << std::endl;
	freeCorkTriMesh(&out1);

	computeDifference(in1, in2, &out1);
	saveMesh("difference.stl", out1);
	std::cout << "Difference done" << std::endl;

	translateZ(out1, 5.0);
	saveMesh("difference_moved.stl", out1);
	std::cout << "Move done" << std::endl;

	CorkTriMesh out2 = out1;

	rotate180X(out1);
	saveMesh("difference_moved_and_rotatedX.stl", out1);
	std::cout << "RotateX done" << std::endl;

	rotate180Y(out2);
	saveMesh("difference_moved_and_rotatedY.stl", out2);
	std::cout << "RotateY done" << std::endl;

	return 0;
}

