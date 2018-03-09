#include "stdafx.h"
#include "STEPImport.h"
#include "SimpleVectors.h"

// Main geometry structures
std::vector<Vector3> vertices;
std::vector<Vector3> normals;
std::vector<Vector2> uvs2;
std::vector<int32_t> indices;
int32_t idxCounter = 0;

//  Get3DGeometry gets called from outside the DLL
STEP_API int Get3DGeometry(char* filepath, char** geoBuffer, char** indexbuffer, int32_t* vertexElements, int32_t* normalElements, int32_t* uv2Elements, int32_t* triangleElements) { 

	int returncode = 0;
	STEPControl_Reader reader;
	reader.ReadFile(filepath);
	
	Standard_Integer NbRoots = reader.NbRootsForTransfer(); // Gets the number of transferable roots
	Standard_Integer NbTrans = reader.TransferRoots();		// Translates all transferable roots, and returns the number of successful translations

	if (NbTrans != NbRoots) 
		returncode = -1; // Not all roots were tranfered succesfully

	TopoDS_Shape shape = reader.OneShape();
	
	GenerateFaces(shape);

	calculateNormals();

	MarshalGeometryData(vertexElements, normalElements, uv2Elements, geoBuffer, triangleElements, indexbuffer);

	return returncode;
}

void GenerateFaces(TopoDS_Shape &shape)
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

				// Check if a duplicate vertex already exists
				//int index = InsertIfUnique(vertices, p1); // Return the index of the vertex in the buffer, or a duplicate of it.
				vertices.push_back(p1);
				vertices.push_back(p2);
				vertices.push_back(p3);

				uvs2.push_back(Vector2(uv1.X(), uv1.Y()));
				uvs2.push_back(Vector2(uv2.X(), uv2.Y()));
				uvs2.push_back(Vector2(uv3.X(), uv3.Y()));


				indices.push_back(idxCounter++);
				indices.push_back(idxCounter++);
				indices.push_back(idxCounter++);
			}
		}
	}
}

void calculateNormals() {
	for (int i = 0; i < vertices.size(); i += 3) {

		Vector3 dir1 = vertices[i + 1] - vertices[i];
		Vector3 dir2 = vertices[i + 2] - vertices[i];
		Vector3 normal = dir1.CrossProduct(dir2);
		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);
	}
}

void MarshalGeometryData(int32_t * vertexElements, int32_t * normalElements, int32_t * uv2Elements, char ** geometricalbuffer, int32_t * triangleElements, char ** indexbuffer)
{
	// Allocate memory for a float array that can hold all our geometrical data.
	size_t requiredFloatElements = (vertices.size() * 3) + (normals.size() * 3) + (uvs2.size() * 2);
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
	for (Vector2 const& uv2 : uvs2) {
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

int testGet3DGeometry() {
	// Dummy data:
	int temp = 0;
	char a = 'a', b = 'b';
	char* filepath = &a;
	char **geometricalbuffer = &filepath, **indexbuffer = &filepath;
	int32_t *vertexElements = &temp, *normalElements = &temp, *uv2Elements = &temp, *triangleElements = &temp;

	return Get3DGeometry(filepath, geometricalbuffer, indexbuffer, vertexElements, normalElements, uv2Elements, triangleElements);
}

int main() {
	int returncode = testGet3DGeometry();
	std::cout << "Get3DGeometry executed with returncode " << returncode << std::endl;
}

//std::cout << "Number of resulting shapes is : " << reader.NbShapes() << std::endl;

// Obtain the results of translation in one OCCT shape
//BRepMesh_IncrementalMesh mesh = BRepMesh_IncrementalMesh(shape, 1);

