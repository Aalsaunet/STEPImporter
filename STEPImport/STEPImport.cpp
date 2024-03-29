#include "stdafx.h"
#include "STEPImport.h"

/* The main geometry structures */
TopoDS_Shape rootShape;
std::vector<TopoDS_Shape> subshapes;

bool VertexLessThanComparator(const IndexedVertex& a, const IndexedVertex& b) {
	return (a.vertex < b.vertex);
}

STEP_API int ImportSTEPFile(const char* filepath, int32_t* numberOfSubShapes) {
	
	STEPControl_Reader reader;
	reader.ReadFile(filepath);
	
	Standard_Integer NbRoots = reader.NbRootsForTransfer(); // Gets the number of transferable roots
	Standard_Integer NbTrans = reader.TransferRoots();		// Translates all transferable roots, and returns the number of successful translations

	rootShape = reader.OneShape();
	BRepMesh_IncrementalMesh(rootShape, 1, true); //Mesh the root shape

	FindAllSubShapes(numberOfSubShapes);
	return (NbRoots - NbTrans); //Returns number of failed translations
}

void FindAllSubShapes(int32_t* numberOfSubShapes) {
	
	for (TopExp_Explorer subshape(rootShape, TopAbs_SOLID); subshape.More(); subshape.Next())
		subshapes.push_back(subshape.Current());

	std::cout << "Number of subshapes: " << subshapes.size() << std::endl;
	*numberOfSubShapes = subshapes.size();
}

STEP_API int ProcessSubShape(int32_t subshapeIndex, char** vertexBuffer, int32_t* vertexEntryCount, char** indexbuffer, int32_t* indexCount) {
	
	TopoDS_Shape& shape = subshapes[subshapeIndex];
	std::vector<IndexedVertex> indexedVerticies;
	std::vector<Vector3> vertices;
	std::vector<int32_t> indices;
	
	RetrieveFaces(shape, indexedVerticies);
	if (indexedVerticies.empty())
		return -2;

	RemoveDuplicateVerticesAndUpdateIndices(indexedVerticies, vertices, indices);
	MarshalGeometryData(vertices, indices, vertexBuffer, vertexEntryCount, indexbuffer, indexCount);
	
	indexedVerticies.clear();
	vertices.clear();
	indices.clear();

	return 0;
}

void RetrieveFaces(TopoDS_Shape &shape, std::vector<IndexedVertex>& indexedVerticies)
{
	int32_t idxCounter = 0;
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

void RemoveDuplicateVerticesAndUpdateIndices(std::vector<IndexedVertex>& indexedVerticies, std::vector<Vector3>& vertices, std::vector<int32_t>& indices) {

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

void MarshalGeometryData(std::vector<Vector3>& vertices, std::vector<int32_t>& indices, char** vertexBuffer, int32_t* vertexEntryCount, char** indexbuffer, int32_t* indexCount)
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

STEP_API int ClearStepModelData() {
	
	//delete &rootShape;
	subshapes.clear(); //TODO iterate and delete every subshape?
	return 0;
}


void TestStepImporter() {
	// Dummy data:
	int temp = 0;
	char a = 'a', b = 'b';
	char* filepath = &a;
	char **vertexbuffer = &filepath, **indexbuffer = &filepath;
	int32_t *vertexEntryCount = &temp, *indexCount = &temp;
	int32_t numberOfSubShapes = 0;
	
	int returncode = ImportSTEPFile("A:\\Dimension10\\repositories\\STEPImporter\\STEPImporter\\Data\\linkrods.step", &numberOfSubShapes);
	if (returncode != 0)
		std::cout << "Error: " << returncode << " translation(s) failed" << std::endl;
	
	std::cout << "ImportSTEPFile executed with returncode " << returncode << std::endl;
	std::cout << "Number of identified subshapes: " << numberOfSubShapes << std::endl;
	
	//ImportSTEPFile("D:\\Models\\STP\\Molstad\\Form_Seabed_Manta_node_shell.stp", vertexbuffer, vertexEntryCount, indexbuffer, indexCount)
	return ;
}

int main() {
	TestStepImporter();
}

