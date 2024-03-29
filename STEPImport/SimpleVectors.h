#pragma once
#include "stdafx.h"

const Standard_Real BOUNDS_OFFSET = 0.01;

struct Vector2 {
	Standard_Real x, y;
	Vector2(Standard_Real x, Standard_Real y) : x(x), y(y) {};
};

struct Vector3 {
	Standard_Real x, y, z;
	Vector3(){};
	Vector3(Standard_Real x, Standard_Real y, Standard_Real z) : x(x), y(y), z(z) {};
	void Set(Standard_Real x_new, Standard_Real y_new, Standard_Real z_new);
	Vector3 operator-(const Vector3& v);
	bool operator==(const Vector3& v) const;
	bool operator<(const Vector3& v) const;	
	Vector3& operator+=(const Vector3& v);
	Vector3 CrossProduct(const Vector3& b) const;
	void Normalize();
	Vector3 ReturnNormalizedInstance();
};

