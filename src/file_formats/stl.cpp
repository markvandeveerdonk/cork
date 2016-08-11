// +-------------------------------------------------------------------------
// | stl.cpp
// | 
// | Author: Mark van de Veerdonk
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

#include "files.h"
#include <stdio.h>
#include <map>

using std::string;

static size_t getline(char **lineptr, size_t *n, FILE *stream) {
	char *bufptr = NULL;
	char *p = bufptr;
	size_t size;
	int c;

	if (lineptr == NULL) {
		return -1;
	}
	if (stream == NULL) {
		return -1;
	}
	if (n == NULL) {
		return -1;
	}
	bufptr = *lineptr;
	size = *n;

	c = fgetc(stream);
	if (c == EOF) {
		return -1;
	}
	if (bufptr == NULL) {
		bufptr = (char*)malloc(128);
		if (bufptr == NULL) {
			return -1;
		}
		size = 128;
	}
	p = bufptr;
	while (c != EOF) {
		if ((size_t)(p - bufptr) > (size - 1)) {
			size = size + 128;
			bufptr = (char*)realloc(bufptr, size);
			if (bufptr == NULL) {
				return -1;
			}
		}
		*p++ = c;
		if (c == '\n') {
			break;
		}
		c = fgetc(stream);
	}

	*p++ = '\0';
	*lineptr = bufptr;
	*n = size;

	return p - bufptr - 1;
}

namespace Files {
# pragma pack (1)
    struct tuple3d {
        float n[3];
    };

    struct StlTriangle {
        tuple3d normal, v1, v2, v3;
        uint16_t attributeByteCount;
    };
# pragma pack ()

    bool operator<(const tuple3d& a, const tuple3d& b) {
        if (a.n[0] == b.n[0]) {
            if (a.n[1] == b.n[1]) {
                return a.n[2] < b.n[2];
            }
            else {
                return a.n[1] < b.n[1];
            }
        }
        else {
            return a.n[0] < b.n[0];
        }
    }

    static uint32_t get_vertex_index(const tuple3d& vertex, FileMesh *data, std::map<tuple3d, uint32_t>& vertexLookup)
    {
        auto iter = vertexLookup.find(vertex);
        if (iter != vertexLookup.end()) {
            return iter->second;
        }

        uint32_t index = data->vertices.size();

        FileVertex fv;
        fv.pos.x = vertex.n[0];
        fv.pos.y = vertex.n[1];
        fv.pos.z = vertex.n[2];
        data->vertices.push_back(fv);

        vertexLookup[vertex] = index;

        return index;
    }

	enum isAsciiResult {
		FailedToRead,
		IsAsciiStl,
		IsNotAsciiStl
	};

	isAsciiResult isAsciiSTL(std::string filename) {
		isAsciiResult result = FailedToRead;
		FILE* f = fopen(filename.c_str(), "r");

		const int BUFSIZE = 5;
		char buf[BUFSIZE+1];
		if (fread(buf, BUFSIZE, 1, f) != 1) {
			fprintf(stderr, "Could not read header from\n");
		} else {
			if (strstr(buf, "solid") != nullptr) {
				result = IsAsciiStl;
			}
			else {
				result = IsNotAsciiStl;
			}
		}
		return result;
	}

	int readBinaryStl(FileMesh *data, string filename)
	{
		std::map<tuple3d, uint32_t> vertexLookup;

		FILE* f = fopen(filename.c_str(), "rb");
		// skip header
		if (fseek(f, 80, SEEK_SET) != 0) {
			fprintf(stderr, "Could not skip header of file %s\n", filename.c_str());
			fclose(f);
			return 1;
		}

		uint32_t numTriangles;
		if (fread(&numTriangles, sizeof(uint32_t), 1, f) != 1) {
			fprintf(stderr, "Could not read number of triangles in %s\n", filename.c_str());
			fclose(f);
			return 1;
		}
		
		if (numTriangles > 50000000) {
			fprintf(stderr, "Number of triangles exceeds (arbitrarily chosen) maximum number of triangles: %u\n", numTriangles);
			fclose(f);
			return 1;
		}

		for (uint triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++) {
			StlTriangle triangle;
			if (fread(&triangle, sizeof(triangle), 1, f) != 1) {
				fprintf(stderr, "Could not read triangle %u\n (0-based)", triangleIndex);
				fclose(f);
				return 1;
			}
			FileTriangle triData;
			triData.a = get_vertex_index(triangle.v1, data, vertexLookup);
			triData.b = get_vertex_index(triangle.v2, data, vertexLookup);
			triData.c = get_vertex_index(triangle.v3, data, vertexLookup);
			data->triangles.push_back(triData);
		}
		fclose(f);
		return 0;
	}

	enum {
		LINE_SIZE = 100
	};

	static char lineBuf[LINE_SIZE + 1];

	bool readLine(FILE *f, int& nRead) {
		size_t len;
		char *cp = 0;
		nRead = getline(&cp, &len, f);
		if (nRead >= 0) {
			strncpy(lineBuf, cp, LINE_SIZE);
			free(cp);
		}
		return (nRead > 0);
	}

	bool readVertexFromAsciiFile(FILE* f, tuple3d& vertex)
	{
		int nItemsRead = fscanf(f, "%*s %f %f %f\n", &vertex.n[0], &vertex.n[1], &vertex.n[2]);
		return (nItemsRead == 3);
	}

	int readAsciiStl(FileMesh *data, string filename)
	{
		std::map<tuple3d, uint32_t> vertexLookup;
		int nRead;
		lineBuf[LINE_SIZE] = '\0';

		FILE* f = fopen(filename.c_str(), "r");
		// skip rest of header
		readLine(f, nRead);

		if (!readLine(f, nRead)) {
			fprintf(stderr, "Could not read from %s (1)\n", filename.c_str());
			fclose(f);
			return 1;
		}
		int triangleCounter = 0;
		while (strstr(lineBuf, "facet normal") != nullptr) {
			readLine(f, nRead); // skip "outer loop"

			tuple3d vertices[3];
			for (int i = 0; i < 3; i++) {
				if (!readVertexFromAsciiFile(f, vertices[i])) {
					fprintf(stderr, "Could not read from %s (1)\n", filename.c_str());
					fclose(f);
					return 1;
				}
			}

			FileTriangle triData;
			triData.a = get_vertex_index(vertices[0], data, vertexLookup);
			triData.b = get_vertex_index(vertices[1], data, vertexLookup);
			triData.c = get_vertex_index(vertices[2], data, vertexLookup);
			data->triangles.push_back(triData);

			readLine(f, nRead); // skip "outer loop"
			readLine(f, nRead); // skip "endfacet"

			readLine(f, nRead); // Next facet?
		}
		fclose(f);

		if (strstr(lineBuf, "endsolid") == nullptr) {
			fprintf(stderr, "expected 'endsolid' to end file %s\n", filename.c_str());
			fclose(f);
			return 1;
		}

		return 0;
	}

	int readSTL(string filename, FileMesh *data)
    {
		int result = 1;
		isAsciiResult isAscii = isAsciiSTL(filename);
		switch (isAscii)
		{
		case IsAsciiStl:
			result = readAsciiStl(data, filename);
			break;
		case IsNotAsciiStl:
			result = readBinaryStl(data, filename);
			break;
		case FailedToRead:
			fprintf(stderr, "Could not read header of file %s\n", filename.c_str());
			break;
		default:
			fprintf(stderr, "Very unexpected error\n");
			break;
		}
		return result;
    }

    static void fillTriangleWithVertex(tuple3d& vertex, Vec3d& v)
    {
        vertex.n[0] = (float)v.x;
        vertex.n[1] = (float)v.y;
        vertex.n[2] = (float)v.z;
    }

    int writeSTL(string filename, FileMesh *data)
    {
        FILE* f = fopen(filename.c_str(), "wb");
        if (f == NULL) {
            fprintf(stderr, "Could not open file %s\n", filename.c_str());
            return 1;
        }

        const int headerSize = 80;
        char header[headerSize] = "This is a binary stl file that contains zeroed normal vectors\0";
        if (fwrite(header, headerSize, 1, f) != 1) {
            fclose(f);
            return 1;
        }

        uint32_t size = data->triangles.size();
        fwrite(&size, sizeof(size), 1, f);

        StlTriangle triangle;
        triangle.normal.n[0] = triangle.normal.n[2] = triangle.normal.n[2] = 0;
        triangle.attributeByteCount = 0;
        for (auto iter = data->triangles.cbegin(); iter != data->triangles.cend(); iter++) {
            fillTriangleWithVertex(triangle.v1, data->vertices[iter->a].pos);
            fillTriangleWithVertex(triangle.v2, data->vertices[iter->b].pos);
            fillTriangleWithVertex(triangle.v3, data->vertices[iter->c].pos);

            fwrite(&triangle, sizeof(triangle), 1, f);
        }

        fclose(f);
        return 0;
    }
}