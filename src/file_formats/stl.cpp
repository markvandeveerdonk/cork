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

#include <map>

using std::string;

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

    int readSTL(string filename, FileMesh *data)
    {
        std::map<tuple3d, uint32_t> vertexLookup;

        FILE* f = fopen(filename.c_str(), "rb");
        if (f == NULL) {
            fprintf(stderr, "Could not open file %s\n", filename.c_str());
            return 1;
        }

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