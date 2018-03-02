//#include "stdafx.h"
//#include <STEPControl_Reader.hxx>
//#include <TopoDS_Shape.hxx>
//#include <BRepTools.hxx>
//#include <iostream>
//
//int main() {
//	
//	STEPControl_Reader reader;
//	reader.ReadFile("C:\\Repositories\\OCAF_experiments\\Data\\screw.step");
//
//	// Loads file MyFile.stp
//	Standard_Integer NbRoots = reader.NbRootsForTransfer();
//	// gets the number of transferable roots
//	std::cout << "Number of roots in STEP file : " << NbRoots << std::endl;
//
//	return 0;
//}
//
//
////Standard_Integer main()
////{
////	STEPControl_Reader reader;
////	reader.ReadFile(; MyFile.stp;);
////	// Loads file MyFile.stp
////	Standard_Integer NbRoots = reader.NbRootsForTransfer();
////	// gets the number of transferable roots
////	cout; Number of roots in STEP file : ; NbRootsendl;
////	Standard_Integer NbTrans = reader.TransferRoots();
////	// translates all transferable roots, and returns the number of //successful translations
////	cout; STEP roots transferred : ; NbTransendl;
////	cout; Number of resulting shapes is : ; reader.NbShapes()endl;
////	TopoDS_Shape result = reader.OneShape();
//	// obtain the results of translation in one OCCT shape