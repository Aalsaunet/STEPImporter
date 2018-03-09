#pragma once
#include "stdafx.h"
#include "SimpleVectors.h"

void Vector3::Set(Standard_Real x_new, Standard_Real y_new, Standard_Real z_new) {
	x = x_new;
	y = y_new;
	z = z_new;
}

Vector3 Vector3::operator-(const Vector3& v) {
	Vector3 result;
	result.x = this->x - v.x;
	result.y = this->y - v.y;
	result.z = this->z - v.z;
	return result;
}

bool Vector3::operator==(const Vector3& v) const {
	if (this->x == v.x && this->y == v.y && this->z == v.z)
		return true;
	return false;
}

bool Vector3::operator<(const Vector3& v) const {
	if (this->x < v.x)
		return true;
	else if (this->x > v.y)
		return false;
	else if (this->y < v.y)
		return true;
	else if (this->y > v.y)
		return false;
	else if (this->z < v.z)
		return true;
	else
		return false;
}

Vector3 Vector3::CrossProduct(Vector3 b) {
	// Given vector a and b, their cross product is: (a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x)
	Vector3 result;
	result.Set(this->y * b.z - this->z * b.y, this->z * b.x - this->x * b.z, this->x * b.y - this->y * b.x);
	return result;
}

//static int InsertIfUnique(std::vector<Vector3> vertexbuffer, Vector3 vertex) {
//
//	// Search for the vertex using binary search;
//	int startIndex = 0;
//	int stopIndex = vertexbuffer.size();
//
//	while ((stopIndex - startIndex) > 1) {
//		int middleIndex = (startIndex + stopIndex) / 2;
//		Vector3 bufferedVertex = vertexbuffer[middleIndex];
//
//		if (vertex == bufferedVertex)
//			return middleIndex;
//		else if (vertex > bufferedVertex)
//			startIndex = middleIndex;
//		else
//			stopIndex = middleIndex;
//	}
//	
//	// If we get here, the vertex isn't present in the buffer
//	//vertexbuffer.in
//
//	return 0;
//}