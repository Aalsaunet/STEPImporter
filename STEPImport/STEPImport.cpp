#include "stdafx.h"
#include "STEPImport.h"
#include "SimpleVectors.h"
#include <algorithm>

struct Face {
	Vector3 vertex;
	//Vector3 normal;
	Vector2 uv2;
	int32_t index;
	Face(Vector3 v, Vector2 uv2, int32_t i) : vertex(v), uv2(uv2), index(i) {};
	
	bool operator<(const Face& otherFace) const {
		if (this->vertex < otherFace.vertex)
			return true;
		return false;
	}
};

std::vector<Face> faces;

/* The main geometry structures */
std::vector<Vector3> vertices;
std::vector<Vector3> normals;
std::vector<Vector2> uv2s;
std::vector<int32_t> indices;
int32_t idxCounter = 0;

bool VertexLessThanComparator(const Face& a, const Face& b) {
	return (a.vertex < b.vertex);
}

//  Get3DGeometry gets called from outside the DLL
STEP_API int Get3DGeometry(const char* filepath, char** geoBuffer, char** indexbuffer, int32_t* vertexElements, int32_t* normalElements, int32_t* uv2Elements, int32_t* triangleElements) { 
	
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
	
	RetrieveFaces(shape);

	if (faces.empty())
		return -2;

	RemoveDuplicateVerticesAndUpdateIndices();

	CalculateSmoothedNormals();

	MarshalGeometryData(vertexElements, normalElements, uv2Elements, geoBuffer, triangleElements, indexbuffer);

	ClearGeometryData();

	return returncode;
}

void RetrieveFaces(TopoDS_Shape &shape)
{
	/*Standard_Real aDeflection = 0.1;*/
	BRepMesh_IncrementalMesh(shape, 1);
	//Standard_Integer aIndex = 1, nbNodes = 0;
	//TColgp_SequenceOfPnt aPoints, aPoints1;

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

				gp_Pnt2d uv1 = uvNodes(n1);
				gp_Pnt2d uv2 = uvNodes(n2);
				gp_Pnt2d uv3 = uvNodes(n3);

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

				faces.push_back(Face(p1, Vector2(uv1.X(), uv1.Y()), idxCounter++));
				faces.push_back(Face(p2, Vector2(uv2.X(), uv2.Y()), idxCounter++));
				faces.push_back(Face(p3, Vector2(uv3.X(), uv3.Y()), idxCounter++));
			}
		}
	}
}

void RemoveDuplicateVerticesAndUpdateIndices() {

	// Sort the vertex buffer first
	std::sort(faces.begin(), faces.end(), VertexLessThanComparator);
	indices.resize(faces.size());

	vertices.push_back(faces[0].vertex);
	//normals.push_back(faces[0].normal);
	uv2s.push_back(faces[0].uv2);
	indices[faces[0].index] = 0;

	// Find remove duplicate vertices, but keep their index
	int vertexInstanceCount = 1;
	for (int i = 1; i < faces.size(); i++) {
		if (faces[i].vertex == faces[i - vertexInstanceCount].vertex) { // Does the faces have the same vertex?
			indices[faces[i].index] = (int32_t) vertices.size() - 1;
			vertexInstanceCount++;
		}

		else {
			vertices.push_back(faces[i].vertex);
			//normals.push_back(faces[i].normal);
			uv2s.push_back(faces[i].uv2);
			indices[faces[i].index] = (int32_t) vertices.size() - 1;
			vertexInstanceCount = 1; //Reset the instance count
		}

	}
}

void CalculateSmoothedNormals() {
	normals.resize(vertices.size());
	std::fill(normals.begin(), normals.end(), Vector3(0, 0, 0)); // TODO initialize elements in normals to Vector3(0, 0, 0)?

	// Calculate face normals and add that normal to surface normals already relevant the vertex
	for (int i = 0; i < indices.size(); i += 3) {
		Vector3 p1 = vertices[indices[i]];
		Vector3 p2 = vertices[indices[i + 1]];
		Vector3 p3 = vertices[indices[i + 2]];

		Vector3 dir1 = p2 - p1;
		Vector3 dir2 = p3 - p1;
		Vector3 normal = dir1.CrossProduct(dir2);
		normal.Normalize();

		normals[indices[i]] += normal;
		normals[indices[i + 1]] += normal;
		normals[indices[i + 2]] += normal;
	}

	// Iterate through each of vertex normal and normalize
	for (int i = 0; i < normals.size(); i++)
		normals[i].Normalize();
}

void MarshalGeometryData(int32_t * vertexElements, int32_t * normalElements, int32_t * uv2Elements, char ** geometricalbuffer, int32_t * triangleElements, char ** indexbuffer)
{
	// Allocate memory for a float array that can hold all our geometrical data.
	size_t requiredFloatElements = (vertices.size() * 3) + (normals.size() * 3) + (uv2s.size() * 2);
	float* geometricalDataBuffer = new float[requiredFloatElements]; // Allocate on the heap and clean up on the C# side


	int32_t dataBufferIndex = 0;
	int32_t numberOfVertixElements = 0;
	for (Vector3 const& vertex : vertices) {
		geometricalDataBuffer[dataBufferIndex++] = (float)vertex.x;
		geometricalDataBuffer[dataBufferIndex++] = (float)vertex.y;
		geometricalDataBuffer[dataBufferIndex++] = (float)vertex.z;
		numberOfVertixElements += 3;
	}
	*vertexElements = numberOfVertixElements;

	int32_t numberOfNormalElements = 0;
	for (Vector3 const& normal : normals) {
		geometricalDataBuffer[dataBufferIndex++] = (float)normal.x;
		geometricalDataBuffer[dataBufferIndex++] = (float)normal.y;
		geometricalDataBuffer[dataBufferIndex++] = (float)normal.z;
		numberOfNormalElements += 3;
	}
	*normalElements = numberOfNormalElements;

	int32_t numberOfUV2Elements = 0;
	for (Vector2 const& uv2 : uv2s) {
		geometricalDataBuffer[dataBufferIndex++] = (float)uv2.x;
		geometricalDataBuffer[dataBufferIndex++] = (float)uv2.y;
		numberOfUV2Elements += 2;
	}
	*uv2Elements = numberOfUV2Elements;

	*geometricalbuffer = (char*)CoTaskMemAlloc(requiredFloatElements * sizeof(float));
	std::memcpy(*geometricalbuffer, geometricalDataBuffer, requiredFloatElements * sizeof(float));
	delete[] geometricalDataBuffer;


	*triangleElements = (int32_t)indices.size();
	*indexbuffer = (char*)CoTaskMemAlloc(indices.size() * sizeof(int32_t));
	std::memcpy(*indexbuffer, &indices[0], indices.size() * sizeof(int32_t));
}


void ClearGeometryData()
{
	faces.clear();
	faces.shrink_to_fit();

	vertices.clear();
	vertices.shrink_to_fit();

	normals.clear();
	normals.shrink_to_fit();

	uv2s.clear();
	uv2s.shrink_to_fit();

	indices.clear();
	indices.shrink_to_fit();

	idxCounter = 0;
}


int testGet3DGeometry() {
	// Dummy data:
	int temp = 0;
	char a = 'a', b = 'b';
	char* filepath = &a;
	char **geometricalbuffer = &filepath, **indexbuffer = &filepath;
	int32_t *vertexElements = &temp, *normalElements = &temp, *uv2Elements = &temp, *triangleElements = &temp;

	return Get3DGeometry("A:\\Dimension10\\repositories\\STEPImporter\\STEPImporter\\Data\\linkrods.step", geometricalbuffer, indexbuffer, vertexElements, normalElements, uv2Elements, triangleElements);
}

int main() {
	int returncode = testGet3DGeometry();
	std::cout << "Get3DGeometry executed with returncode " << returncode << std::endl;
}

// Obtain the results of translation in one OCCT shape
//BRepMesh_IncrementalMesh mesh = BRepMesh_IncrementalMesh(shape, 1);

