// STEPImport.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//
//#include <qlist.h>
//#include <qmap.h>
//#include <qhash.h>
//#include <qpair.h>
//#include <qstring.h>
//#include <qset.h>
//#include <qtextstream.h>
//#include <qcontiguouscache.h>
//#include <qatomic.h>
//#include <qbasicatomic.h>
//
//#include <qdatastream.h>
//#include <qchar.h>
//
//
//#include <qpoint.h>
//#include <qalgorithms.h>
//
//#include <qcolor.h>
//#include <qnamespace.h>
//
//#include <qdebug.h>
//
//#include <qrgb.h>
//#include <qvector3d.h>
//#include <qvector2d.h>


int main() {

	STEPControl_Reader reader;
	reader.ReadFile("C:\\Repositories\\OCAF_experiments\\Data\\linkrods.step");

	// Loads file MyFile.stp
	Standard_Integer NbRoots = reader.NbRootsForTransfer();
	// gets the number of transferable roots
	std::cout << "Number of roots in STEP file : " << NbRoots << std::endl;

	// Translates all transferable roots, and returns the number of //successful translations
	Standard_Integer NbTrans = reader.TransferRoots();
	std::cout << "STEP roots transferred : " << NbTrans << std::endl;
	std::cout << "Number of resulting shapes is : " << reader.NbShapes() << std::endl;
	TopoDS_Shape result = reader.OneShape();
	
	// Obtain the results of translation in one OCCT shape
	//BRepMesh_IncrementalMesh mesh = BRepMesh_IncrementalMesh(result, 1);

	return 0;
}

void get3DGeometry(TopoDS_Shape shape) {

	std::vector<QVector3D> vertices;
	std::vector<QVector3D> normals;
	std::vector<QVector2D> uvs2;
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


				QVector3D p1, p2, p3;
				if (faceOrientation == TopAbs_Orientation::TopAbs_FORWARD)
				{
					p1 = QVector3D(aPnt1.X(), aPnt1.Y(), aPnt1.Z());
					p2 = QVector3D(aPnt2.X(), aPnt2.Y(), aPnt2.Z());
					p3 = QVector3D(aPnt3.X(), aPnt3.Y(), aPnt3.Z());
				}
				else
				{
					p1 = QVector3D(aPnt3.X(), aPnt3.Y(), aPnt3.Z());
					p2 = QVector3D(aPnt2.X(), aPnt2.Y(), aPnt2.Z());
					p3 = QVector3D(aPnt1.X(), aPnt1.Y(), aPnt1.Z());
				}


				vertices.push_back(p1);
				vertices.push_back(p2);
				vertices.push_back(p3);


				QVector3D dir1 = p2 - p1;
				QVector3D dir2 = p3 - p1;
				QVector3D normal = QVector3D::crossProduct(dir1, dir2);


				normals.push_back(normal);
				normals.push_back(normal);
				normals.push_back(normal);


				uvs2.push_back(QVector2D(uv1.X(), uv1.Y()));
				uvs2.push_back(QVector2D(uv2.X(), uv2.Y()));
				uvs2.push_back(QVector2D(uv3.X(), uv3.Y()));


				indices.push_back(idxCounter++);
				indices.push_back(idxCounter++);
				indices.push_back(idxCounter++);
			}
		}
	}
}

