#include "stdafx.h"
#include "STEPImport.h"
#include "SimpleVectors.h"
#include <algorithm>

struct IndexedVertex {
	Vector3 vertex;
	int32_t index;
	IndexedVertex(Vector3 v, int32_t i) : vertex(v), index(i) {};
	
	bool operator<(const IndexedVertex& otherFace) const {
		if (this->vertex < otherFace.vertex)
			return true;
		return false;
	}
};

std::vector<IndexedVertex> indexedVerticies;

/* The main geometry structures */
std::vector<Vector3> vertices;
std::vector<int32_t> indices;
int32_t idxCounter = 0;

bool VertexLessThanComparator(const IndexedVertex& a, const IndexedVertex& b) {
	return (a.vertex < b.vertex);
}

//  Get3DGeometry gets called from outside the DLL
STEP_API int ImportSTEPFile(const char* filepath, char** vertexBuffer, int32_t* vertexEntryCount, char** indexbuffer, int32_t* indexCount) {
	
	int returncode = 0;
	STEPControl_Reader reader;
	
	reader.ReadFile(filepath);
	
	Standard_Integer NbRoots = reader.NbRootsForTransfer(); // Gets the number of transferable roots
	std::cout << "Number of transferable roots: " << NbRoots << std::endl;
	Standard_Integer NbTrans = reader.TransferRoots();		// Translates all transferable roots, and returns the number of successful translations
	std::cout << "Number of successful translations: " << NbTrans << std::endl;

	if (NbTrans != NbRoots)
		returncode = -1; // Not all roots were tranfered successfully

	TopoDS_Shape shape = reader.OneShape();
	BRepMesh_IncrementalMesh(shape, 1, true);
	std::vector<TopoDS_Shape> subshapes = ExpandShapeHierarchy(shape);
	
	RetrieveFaces(shape);
	if (indexedVerticies.empty())
		return -2;

	RemoveDuplicateVerticesAndUpdateIndices();
	MarshalGeometryData(vertexBuffer, vertexEntryCount, indexbuffer, indexCount);
	ClearGeometryData();

	return returncode;
}

std::vector<TopoDS_Shape> ExpandShapeHierarchy(TopoDS_Shape& rootShape) {
	
	std::vector<TopoDS_Shape> subshapes;
	for (TopExp_Explorer subshape(rootShape, TopAbs_SOLID); subshape.More(); subshape.Next())
	{
		TopoDS_Shape shape = subshape.Current();
		subshapes.push_back(shape);
	}

	std::cout << "Number of subshapes: " << subshapes.size() << std::endl;
	return subshapes;
}

void RetrieveFaces(TopoDS_Shape &shape)
{
	

	for (TopExp_Explorer aExpFace(shape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
		TopAbs_Orientation faceOrientation = aFace.Orientation();

		TopLoc_Location aLocation;
		Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace, aLocation);

		if (!aTr.IsNull())
		{
			const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
			const Poly_Array1OfTriangle& triangles = aTr->Triangles();
			const TColgp_Array1OfPnt2d & uvNodes = aTr->UVNodes();

			TColgp_Array1OfPnt aPoints(1, aNodes.Length());
			for (Standard_Integer i = 1; i < aNodes.Length() + 1; i++)
				aPoints(i) = aNodes(i).Transformed(aLocation);

			Standard_Integer nnn = aTr->NbTriangles();
			Standard_Integer nt, n1, n2, n3;

			for (nt = 1; nt < nnn + 1; nt++)
			{
				triangles(nt).Get(n1, n2, n3);
				gp_Pnt aPnt1 = aPoints(n1);
				gp_Pnt aPnt2 = aPoints(n2);
				gp_Pnt aPnt3 = aPoints(n3);

				Vector3 p1, p2, p3;
				if (faceOrientation == TopAbs_Orientation::TopAbs_FORWARD)
				{
					p1.Set(aPnt1.X(), aPnt1.Y(), aPnt1.Z());
					p2.Set(aPnt2.X(), aPnt2.Y(), aPnt2.Z());
					p3.Set(aPnt3.X(), aPnt3.Y(), aPnt3.Z());
				}
				else
				{
					p1.Set(aPnt3.X(), aPnt3.Y(), aPnt3.Z());
					p2.Set(aPnt2.X(), aPnt2.Y(), aPnt2.Z());
					p3.Set(aPnt1.X(), aPnt1.Y(), aPnt1.Z());
				}

				indexedVerticies.push_back(IndexedVertex(p1, idxCounter++));
				indexedVerticies.push_back(IndexedVertex(p2, idxCounter++));
				indexedVerticies.push_back(IndexedVertex(p3, idxCounter++));
			}
		}
	}
}

void RemoveDuplicateVerticesAndUpdateIndices() {

	// Sort the vertex buffer first
	std::sort(indexedVerticies.begin(), indexedVerticies.end(), VertexLessThanComparator);
	indices.resize(indexedVerticies.size());

	vertices.push_back(indexedVerticies[0].vertex);
	indices[indexedVerticies[0].index] = 0;

	// Find remove duplicate vertices, but keep their index
	int vertexInstanceCount = 1;
	for (int i = 1; i < indexedVerticies.size(); i++) {
		if (indexedVerticies[i].vertex == indexedVerticies[i - vertexInstanceCount].vertex) { // Does the faces have the same vertex?
			indices[indexedVerticies[i].index] = (int32_t) vertices.size() - 1;
			vertexInstanceCount++;
		}

		else {
			vertices.push_back(indexedVerticies[i].vertex);
			indices[indexedVerticies[i].index] = (int32_t) vertices.size() - 1;
			vertexInstanceCount = 1; //Reset the instance count
		}
	}
}

void MarshalGeometryData(char** vertexBuffer, int32_t* vertexEntryCount, char** indexbuffer, int32_t* indexCount)
{
	// Allocate memory for a float array that can hold all our geometrical data.
	size_t requiredFloatElements = vertices.size() * 3;
	float* geometricalDataBuffer = new float[requiredFloatElements];

	int i = 0, j = 0; // i = number of vertices, j = number of floats
	for (; i < vertices.size(); i++, j += 3) {
		geometricalDataBuffer[j] = (float)vertices[i].x;
		geometricalDataBuffer[j + 1] = (float)vertices[i].y;
		geometricalDataBuffer[j + 2] = (float)vertices[i].z;
	}

	*vertexEntryCount = (int32_t)(requiredFloatElements);
	*vertexBuffer = (char*)CoTaskMemAlloc(requiredFloatElements * sizeof(float));
	std::memcpy(*vertexBuffer, &geometricalDataBuffer[0], requiredFloatElements * sizeof(float));

	*indexCount = (int32_t)indices.size();
	*indexbuffer = (char*)CoTaskMemAlloc(indices.size() * sizeof(int32_t));
	std::memcpy(*indexbuffer, &indices[0], indices.size() * sizeof(int32_t));
}


void ClearGeometryData()
{
	indexedVerticies.clear();
	indexedVerticies.shrink_to_fit();

	vertices.clear();
	vertices.shrink_to_fit();

	indices.clear();
	indices.shrink_to_fit();

	idxCounter = 0;
}


int testGet3DGeometry() {
	// Dummy data:
	int temp = 0;
	char a = 'a', b = 'b';
	char* filepath = &a;
	char **vertexbuffer = &filepath, **indexbuffer = &filepath;
	int32_t *vertexEntryCount = &temp, *indexCount = &temp;

	return ImportSTEPFile("D:\\Models\\STP\\Molstad\\Form_Seabed_Manta_node_shell.stp", vertexbuffer, vertexEntryCount, indexbuffer, indexCount);
}

int main() {
	int returncode = testGet3DGeometry();
	std::cout << "ImportSTEPFile executed with returncode " << returncode << std::endl;
}

// Obtain the results of translation in one OCCT shape
//BRepMesh_IncrementalMesh mesh = BRepMesh_IncrementalMesh(shape, 1);

