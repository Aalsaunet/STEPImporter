// STEPImport.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define STEP_API extern "C" __declspec(dllexport)

struct Vector2 {
	Standard_Real x, y;
	Vector2(Standard_Real x, Standard_Real y) : x(x), y(y) {};
};

struct Vector3 {
	Standard_Real x, y, z;
	void Set(Standard_Real x_new, Standard_Real y_new, Standard_Real z_new) {
		x = x_new;
		y = y_new;
		z = z_new;
	}

	Vector3 operator-(const Vector3& v) {
		Vector3 result;
		result.x = this->x - v.x;
		result.y = this->y - v.y;
		result.z = this->z - v.z;
		return result;
	}

	static Vector3 CrossProduct(Vector3 a, Vector3 b) {
		// Given vector a and b, their cross product is: (a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x)
		Vector3 result;
		result.Set(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		return result;
	}
};


STEP_API void Get3DGeometry(Standard_CString filepath, char* outAddr) {

	STEPControl_Reader reader;
	//reader.ReadFile("C:\\Repositories\\OCAF_experiments\\Data\\linkrods.step");
	reader.ReadFile(filepath);

	// Loads file MyFile.stp
	Standard_Integer NbRoots = reader.NbRootsForTransfer();
	// gets the number of transferable roots
	std::cout << "Number of roots in STEP file : " << NbRoots << std::endl;

	// Translates all transferable roots, and returns the number of //successful translations
	Standard_Integer NbTrans = reader.TransferRoots();
	std::cout << "STEP roots transferred : " << NbTrans << std::endl;
	std::cout << "Number of resulting shapes is : " << reader.NbShapes() << std::endl;
	TopoDS_Shape shape = reader.OneShape();

	// Obtain the results of translation in one OCCT shape
	//BRepMesh_IncrementalMesh mesh = BRepMesh_IncrementalMesh(shape, 1);

	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> uvs2;
	std::vector<unsigned int> indices;
	unsigned int idxCounter = 0;

	Standard_Real aDeflection = 0.1;
	BRepMesh_IncrementalMesh(shape, 1);
	Standard_Integer aIndex = 1, nbNodes = 0;
	TColgp_SequenceOfPnt aPoints, aPoints1;
	
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


				vertices.push_back(p1);
				vertices.push_back(p2);
				vertices.push_back(p3);


				Vector3 dir1 = p2 - p1;
				Vector3 dir2 = p3 - p1;
				Vector3 normal = Vector3::CrossProduct(dir1, dir2);


				normals.push_back(normal);
				normals.push_back(normal);
				normals.push_back(normal);


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

int main() {

	return 0;
}

