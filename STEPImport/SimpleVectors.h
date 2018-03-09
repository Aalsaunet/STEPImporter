#pragma once
#include "stdafx.h"

struct Vector2 {
	Standard_Real x, y;
	Vector2(Standard_Real x, Standard_Real y) : x(x), y(y) {};
};

struct Vector3 {
	Standard_Real x, y, z;
	void Set(Standard_Real x_new, Standard_Real y_new, Standard_Real z_new);
	Vector3 operator-(const Vector3& v);
	bool operator==(const Vector3& v);
	bool operator<(const Vector3& v);	
	Vector3 CrossProduct(Vector3 b);
};

bool VectorComparator(Vector3 a, Vector3 b);

//static int InsertIfUnique(std::vector<Vector3> vertexbuffer, Vector3 vertex);
