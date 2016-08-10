// +-------------------------------------------------------------------------
// | cork.h
// | 
// | Author: Gilbert Bernstein
// +-------------------------------------------------------------------------
// | COPYRIGHT:
// |    Copyright Gilbert Bernstein 2013
// |    See the included COPYRIGHT file for further details.
// |    
// |    This file is part of the Cork library.
// |
// |    Cork is free software: you can redistribute it and/or modify
// |    it under the terms of the GNU Lesser General Public License as
// |    published by the Free Software Foundation, either version 3 of
// |    the License, or (at your option) any later version.
// |
// |    Cork is distributed in the hope that it will be useful,
// |    but WITHOUT ANY WARRANTY; without even the implied warranty of
// |    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// |    GNU Lesser General Public License for more details.
// |
// |    You should have received a copy 
// |    of the GNU Lesser General Public License
// |    along with Cork.  If not, see <http://www.gnu.org/licenses/>.
// +-------------------------------------------------------------------------
#pragma once

#include <string>

#ifdef _WIN32
	#ifdef CORKLIBRARY_EXPORTS
		#define CORKLIBRARY_API extern "C" __declspec(dllexport) 
	#else
		#define CORKLIBRARY_API extern "C" __declspec(dllimport) 
	#endif
#else
	#define CORKLIBRARY_API 
#endif

#ifndef uint
typedef unsigned int uint;
#endif

// if a mesh is taken as input, the client must manage the memory
// if a mesh is given as output, please use the provided
// function to free the allocated memory.
struct CorkTriMesh
{
    uint    n_triangles;
    uint    n_vertices;
    uint    *triangles;
    float   *vertices;
};

CORKLIBRARY_API void freeCorkTriMesh(CorkTriMesh *mesh);

CORKLIBRARY_API bool loadMesh(std::string filename, CorkTriMesh& out);
CORKLIBRARY_API void saveMesh(std::string filename, CorkTriMesh in);

// the inputs to Boolean operations must be "solid":
//  -   closed (aka. watertight; see comment at bottom)
//  -   non-self-intersecting
// additionally, inputs should use a counter-clockwise convention
// for triangle facing.  If the triangles are presented in clockwise
// orientation, the object is interpreted as its unbounded complement

// This function will test whether or not a mesh is solid
CORKLIBRARY_API bool isSolid(CorkTriMesh mesh);

// Boolean operations follow
// result = A U B
CORKLIBRARY_API void computeUnion(CorkTriMesh in0, CorkTriMesh in1, CorkTriMesh *out);

// result = A - B
CORKLIBRARY_API void computeDifference(CorkTriMesh in0, CorkTriMesh in1, CorkTriMesh *out);

// result = A ^ B
CORKLIBRARY_API void computeIntersection(CorkTriMesh in0, CorkTriMesh in1, CorkTriMesh *out);

// result = A XOR B
CORKLIBRARY_API void computeSymmetricDifference(CorkTriMesh in0, CorkTriMesh in1, CorkTriMesh *out);

// Not a Boolean operation, but related:
//  No portion of either surface is deleted.  However, the
//  curve of intersection between the two surfaces is made explicit,
//  such that the two surfaces are now connected.
CORKLIBRARY_API void resolveIntersections(CorkTriMesh in0, CorkTriMesh in1, CorkTriMesh *out);

CORKLIBRARY_API void translateZ(CorkTriMesh& in0, float deltaZ);
CORKLIBRARY_API void translate(CorkTriMesh& in0, float deltaX, float deltaY, float deltaZ);
CORKLIBRARY_API void rotate180X(CorkTriMesh& in0);
CORKLIBRARY_API void rotate180Y(CorkTriMesh& in0);
CORKLIBRARY_API void rotateZ(CorkTriMesh& in0, float angle);

/*
 * C# friendly versions of the functions above.
 * (Friendly as long as you don't require access to the 3D model itself)
 */

/* Management functions */

// Deletes all meshes from memory
CORKLIBRARY_API void ClearAllMeshes();

// Deletes mesh with given ID from memory
CORKLIBRARY_API bool DeleteMesh(int ID);

// Creates a copy of mesh identified by srcID and store under destID
CORKLIBRARY_API bool CopyMesh(int srcID, int destID);

// Loads the mesh from the file specified by fileName and stores it internally under ID
CORKLIBRARY_API bool LoadMesh(int ID, char* fileName);

// Save the mesh stored internally under ID as file fileName.
CORKLIBRARY_API bool SaveMesh(int ID, char* fileName);

/* Cork native functions */

// Can cork handle the model?
CORKLIBRARY_API bool IsSolid(int ID);

// Create union model from models stored internally and identified by InID1 and InID2 
// and store as OutID.
CORKLIBRARY_API bool Union(int ID1, int ID2, int OutID);

// Create difference model from model stored internally and identified by InID1 and InID2 
// and store as OutID. Model ID2 is subtraced from ID1.
CORKLIBRARY_API bool Difference(int ID1, int ID2, int OutID);

// Create intersection model from models stored internally and identified by InID1 and InID2 
// and store as OutID.
CORKLIBRARY_API bool Intersection(int ID1, int ID2, int OutID);

// Perform XOR operation on models stored internally and identified by InID1 and InID2 
// and store as OutID.
CORKLIBRARY_API bool Xor(int ID1, int ID2, int OutID);

/* Added manipulation functions */

CORKLIBRARY_API bool TranslateZ(int ID, float deltaZ);
CORKLIBRARY_API bool Translate(int ID, float deltaX, float deltaY, float deltaZ);
CORKLIBRARY_API bool Rotate180X(int ID);
CORKLIBRARY_API bool Rotate180Y(int ID);
CORKLIBRARY_API bool RotateZ(int ID, float angle);
