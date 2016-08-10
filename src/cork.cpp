// +-------------------------------------------------------------------------
// | cork.cpp
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
#include "cork.h"

#include "mesh.h"


void freeCorkTriMesh(CorkTriMesh *mesh)
{
    delete[] mesh->triangles;
    delete[] mesh->vertices;
    mesh->n_triangles = 0;
    mesh->n_vertices = 0;
}


struct CorkTriangle;

struct CorkVertex :
    public MinimalVertexData,
    public RemeshVertexData,
    public IsctVertexData,
    public BoolVertexData
{
    void merge(const CorkVertex &v0, const CorkVertex &v1) {
        double                              a0 = 0.5;
        if(v0.manifold && !v1.manifold)     a0 = 0.0;
        if(!v0.manifold && v1.manifold)     a0 = 1.0;
        double a1 = 1.0 - a0;
        
        pos         = a0 * v0.pos       + a1 * v1.pos;
    }
    void interpolate(const CorkVertex &v0, const CorkVertex &v1) {
        double a0 = 0.5;
        double a1 = 0.5;
        pos         = a0 * v0.pos       + a1 * v1.pos;
    }
    
    
    void isct(IsctVertEdgeTriInput<CorkVertex,CorkTriangle> input)
    {
        Vec2d       a_e     = Vec2d(1,1)/2.0;
        Vec3d       a_t     = Vec3d(1,1,1)/3.0;
        a_e /= 2.0;
        a_t /= 2.0;
    }
    void isct(IsctVertTriTriTriInput<CorkVertex,CorkTriangle> input)
    {
        Vec3d       a[3];
        for(uint k=0; k<3; k++) {
            a[k]    = Vec3d(1,1,1)/3.0;
            a[k] /= 3.0;
        }
        for(uint i=0; i<3; i++) {
          for(uint j=0; j<3; j++) {
        }}
    }
    void isctInterpolate(const CorkVertex &v0, const CorkVertex &v1) {
        double a0 = len(v1.pos - pos);
        double a1 = len(v0.pos - pos);
        if(a0 + a1 == 0.0) a0 = a1 = 0.5; // safety
        double sum = a0+a1;
        a0 /= sum;
        a1 /= sum;
    }
};

struct CorkTriangle :
    public MinimalTriangleData,
    public RemeshTriangleData,
    public IsctTriangleData,
    public BoolTriangleData
{
    void merge(const CorkTriangle &, const CorkTriangle &) {}
    static void split(CorkTriangle &, CorkTriangle &,
                      const CorkTriangle &) {}
    void move(const CorkTriangle &) {}
    void subdivide(SubdivideTriInput<CorkVertex,CorkTriangle> input)
    {
        bool_alg_data = input.pt->bool_alg_data;
    }
};

//using RawCorkMesh = RawMesh<CorkVertex, CorkTriangle>;
//using CorkMesh = Mesh<CorkVertex, CorkTriangle>;
typedef RawMesh<CorkVertex, CorkTriangle>   RawCorkMesh;
typedef Mesh<CorkVertex, CorkTriangle>      CorkMesh;

void corkTriMesh2CorkMesh(
    CorkTriMesh in,
    CorkMesh *mesh_out
) {
    RawCorkMesh raw;
    raw.vertices.resize(in.n_vertices);
    raw.triangles.resize(in.n_triangles);
    if(in.n_vertices == 0 || in.n_triangles == 0) {
        CORK_ERROR("empty mesh input to Cork routine.");
        *mesh_out = CorkMesh(raw);
        return;
    }
    
    uint max_ref_idx = 0;
    for(uint i=0; i<in.n_triangles; i++) {
        raw.triangles[i].a = in.triangles[3*i+0];
        raw.triangles[i].b = in.triangles[3*i+1];
        raw.triangles[i].c = in.triangles[3*i+2];
        max_ref_idx = std::max(
                        std::max(max_ref_idx,
                                 in.triangles[3*i+0]),
                        std::max(in.triangles[3*i+1],
                                 in.triangles[3*i+2])
                      );
    }
    if(max_ref_idx > in.n_vertices) {
        CORK_ERROR("mesh input to Cork routine has an out of range reference "
              "to a vertex.");
        raw.vertices.clear();
        raw.triangles.clear();
        *mesh_out = CorkMesh(raw);
        return;
    }
    
    for(uint i=0; i<in.n_vertices; i++) {
        raw.vertices[i].pos.x = in.vertices[3*i+0];
        raw.vertices[i].pos.y = in.vertices[3*i+1];
        raw.vertices[i].pos.z = in.vertices[3*i+2];
    }
    
    *mesh_out = CorkMesh(raw);
}
void corkMesh2CorkTriMesh(
    CorkMesh *mesh_in,
    CorkTriMesh *out
) {
    RawCorkMesh raw = mesh_in->raw();
    
    out->n_triangles = raw.triangles.size();
    out->n_vertices  = raw.vertices.size();
    
    out->triangles = new uint[(out->n_triangles) * 3];
    out->vertices  = new float[(out->n_vertices) * 3];
    
    for(uint i=0; i<out->n_triangles; i++) {
        (out->triangles)[3*i+0] = raw.triangles[i].a;
        (out->triangles)[3*i+1] = raw.triangles[i].b;
        (out->triangles)[3*i+2] = raw.triangles[i].c;
    }
    
    for(uint i=0; i<out->n_vertices; i++) {
        (out->vertices)[3*i+0] = static_cast<float>(raw.vertices[i].pos.x);
        (out->vertices)[3*i+1] = static_cast<float>(raw.vertices[i].pos.y);
        (out->vertices)[3*i+2] = static_cast<float>(raw.vertices[i].pos.z);
    }
}


bool isSolid(CorkTriMesh cmesh)
{
    CorkMesh mesh;
    corkTriMesh2CorkMesh(cmesh, &mesh);
    
    bool solid = true;
    
    if(mesh.isSelfIntersecting()) {
        CORK_ERROR("isSolid() was given a self-intersecting mesh");
        solid = false;
    }
    
    if(!mesh.isClosed()) {
        CORK_ERROR("isSolid() was given a non-closed mesh");
        solid = false;
    }
    
    return solid;
}

void computeUnion(
    CorkTriMesh in0, CorkTriMesh in1, CorkTriMesh *out
) {
    CorkMesh cmIn0, cmIn1;
    corkTriMesh2CorkMesh(in0, &cmIn0);
    corkTriMesh2CorkMesh(in1, &cmIn1);
    
    cmIn0.boolUnion(cmIn1);
    
    corkMesh2CorkTriMesh(&cmIn0, out);
}

void computeDifference(
    CorkTriMesh in0, CorkTriMesh in1, CorkTriMesh *out
) {
    CorkMesh cmIn0, cmIn1;
    corkTriMesh2CorkMesh(in0, &cmIn0);
    corkTriMesh2CorkMesh(in1, &cmIn1);
    
    cmIn0.boolDiff(cmIn1);
    
    corkMesh2CorkTriMesh(&cmIn0, out);
}

void computeIntersection(
    CorkTriMesh in0, CorkTriMesh in1, CorkTriMesh *out
) {
    CorkMesh cmIn0, cmIn1;
    corkTriMesh2CorkMesh(in0, &cmIn0);
    corkTriMesh2CorkMesh(in1, &cmIn1);
    
    cmIn0.boolIsct(cmIn1);
    
    corkMesh2CorkTriMesh(&cmIn0, out);
}

void computeSymmetricDifference(
    CorkTriMesh in0, CorkTriMesh in1, CorkTriMesh *out
) {
    CorkMesh cmIn0, cmIn1;
    corkTriMesh2CorkMesh(in0, &cmIn0);
    corkTriMesh2CorkMesh(in1, &cmIn1);
    
    cmIn0.boolXor(cmIn1);
    
    corkMesh2CorkTriMesh(&cmIn0, out);
}

void resolveIntersections(
    CorkTriMesh in0, CorkTriMesh in1, CorkTriMesh *out
) {
    CorkMesh cmIn0, cmIn1;
    corkTriMesh2CorkMesh(in0, &cmIn0);
    corkTriMesh2CorkMesh(in1, &cmIn1);
    
    cmIn0.disjointUnion(cmIn1);
    cmIn0.resolveIntersections();
    
    corkMesh2CorkTriMesh(&cmIn0, out);
}

void translateZ(CorkTriMesh& in0, float deltaZ)
{
	for (uint i = 0; i < in0.n_vertices; i++) {
		in0.vertices[3 * i + 2] += deltaZ;
	}
}

void translate(CorkTriMesh& in0, float deltaX, float deltaY, float deltaZ)
{
	for (uint i = 0; i < in0.n_vertices; i++) {
		in0.vertices[3 * i + 0] += deltaX;
		in0.vertices[3 * i + 1] += deltaY;
		in0.vertices[3 * i + 2] += deltaZ;
	}
}

void rotate180X(CorkTriMesh& in0)
{
	float* pF = in0.vertices + 1; // Take address of first Y-coordinate
	for (uint i = 0; i < in0.n_vertices; i++) {
		pF[0] = -pF[0]; // invert Y
		pF[1] = -pF[1]; // invert Z
		pF += 3; // Go to next vertex
	}
}

void rotate180Y(CorkTriMesh& in0)
{
	float* pF = in0.vertices; // Take address of first X-coordinate
	for (uint i = 0; i < in0.n_vertices; i++) {
		pF[0] = -pF[0]; // invert X
		pF[2] = -pF[2]; // invert Z
		pF += 3; // Go to next vertex
	}
}

extern bool loadMesh(std::string filename, CorkTriMesh& out);
extern void saveMesh(std::string filename, CorkTriMesh in);

#include <map>
static std::map<int, CorkTriMesh*> meshDict;
	
bool LoadMesh(int ID, char* fileName)
{
	CorkTriMesh* pMesh = new CorkTriMesh();
	bool success = loadMesh(fileName, *pMesh);
	if (success) {
		meshDict[ID] = pMesh;
	}
	else {
		delete pMesh;
	}
	return success;
}

bool SaveMesh(int ID, char* fileName)
{
	auto it = meshDict.find(ID);
	if (it == meshDict.end()) {
		return false;
	}
	saveMesh(fileName, *it->second);
	return true;
}

void ClearAllMeshes()
{
	for (auto it = meshDict.begin(); it != meshDict.end(); it++) {
		delete(it->second);
	}
	meshDict.clear();
}

bool DeleteMesh(int ID)
{
	auto it = meshDict.find(ID);
	if (it == meshDict.end()) {
		return false;
	}
	delete(it->second);
	meshDict.erase(it);
	return true;
}

bool CopyMesh(int srcID, int destID)
{
	auto it = meshDict.find(srcID);
	if (it == meshDict.end()) {
		return false;
	}
	CorkTriMesh* pMesh = new CorkTriMesh();
	*pMesh = *it->second;
	meshDict[destID] = pMesh;
	return true;
}

bool IsSolid(int ID)
{
	auto it = meshDict.find(ID);
	if (it == meshDict.end()) {
		return false;
	}
	return isSolid(*it->second);
}

bool Union(int InID1, int InID2, int OutID)
{
	auto it1 = meshDict.find(InID1);
	if (it1 == meshDict.end()) {
		return false;
	}
	auto it2 = meshDict.find(InID2);
	if (it2 == meshDict.end()) {
		return false;
	}
	DeleteMesh(OutID);

	CorkTriMesh* pMesh = new CorkTriMesh();
	computeUnion(*it1->second, *it2->second, pMesh);
	meshDict[OutID] = pMesh;
	return true;
}

bool Difference(int InID1, int InID2, int OutID)
{
	auto it1 = meshDict.find(InID1);
	if (it1 == meshDict.end()) {
		return false;
	}
	auto it2 = meshDict.find(InID2);
	if (it2 == meshDict.end()) {
		return false;
	}
	DeleteMesh(OutID);

	CorkTriMesh* pMesh = new CorkTriMesh();
	computeDifference(*it1->second, *it2->second, pMesh);
	meshDict[OutID] = pMesh;
	return true;
}

bool Intersection(int InID1, int InID2, int OutID)
{
	auto it1 = meshDict.find(InID1);
	if (it1 == meshDict.end()) {
		return false;
	}
	auto it2 = meshDict.find(InID2);
	if (it2 == meshDict.end()) {
		return false;
	}
	DeleteMesh(OutID);

	CorkTriMesh* pMesh = new CorkTriMesh();
	computeIntersection(*it1->second, *it2->second, pMesh);
	meshDict[OutID] = pMesh;
	return true;
}

bool Xor(int InID1, int InID2, int OutID)
{
	auto it1 = meshDict.find(InID1);
	if (it1 == meshDict.end()) {
		return false;
	}
	auto it2 = meshDict.find(InID2);
	if (it2 == meshDict.end()) {
		return false;
	}
	DeleteMesh(OutID);

	CorkTriMesh* pMesh = new CorkTriMesh();
	computeSymmetricDifference(*it1->second, *it2->second, pMesh);
	meshDict[OutID] = pMesh;
	return true;
}

bool TranslateZ(int ID, float deltaZ)
{
	auto it = meshDict.find(ID);
	if (it == meshDict.end()) {
		return false;
	}
	translateZ(*it->second, deltaZ);
	return true;
}

bool Translate(int ID, float deltaX, float deltaY, float deltaZ)
{
	auto it = meshDict.find(ID);
	if (it == meshDict.end()) {
		return false;
	}
	translate(*it->second, deltaX, deltaY, deltaZ);
	return true;
}

bool Rotate180X(int ID)
{
	auto it = meshDict.find(ID);
	if (it == meshDict.end()) {
		return false;
	}
	rotate180X(*it->second);
	return true;
}
	
bool Rotate180Y(int ID)
{
	auto it = meshDict.find(ID);
	if (it == meshDict.end()) {
		return false;
	}
	rotate180Y(*it->second);
	return true;
}

