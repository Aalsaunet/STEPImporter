// STEPImport.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <STEPControl_Reader.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepTools.hxx>
#include <iostream>

int main() {

	STEPControl_Reader reader;
	reader.ReadFile("C:\\Repositories\\OCAF_experiments\\Data\\linkrods.step");

	// Loads file MyFile.stp
	Standard_Integer NbRoots = reader.NbRootsForTransfer();
	// gets the number of transferable roots
	std::cout << "Number of roots in STEP file : " << NbRoots << std::endl;

	Standard_Integer NbTrans = reader.TransferRoots();
	// Translates all transferable roots, and returns the number of //successful translations
	std::cout << "STEP roots transferred : " << NbTrans << std::endl;
	std::cout << "Number of resulting shapes is : " << reader.NbShapes() << std::endl;
	TopoDS_Shape result = reader.OneShape();
	
	// Obtain the results of translation in one OCCT shape


	return 0;
}

