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

template <typename T> bool IsInBounds(const T& value, const T& low, const T& high) {
	return (value > low) && (value < high);
}

//bool Vector3::operator==(const Vector3& v) const {
//	if (IsInBounds<Standard_Real>(v.x, this->x - BOUNDS_OFFSET, this->x + BOUNDS_OFFSET)
//		&& IsInBounds<Standard_Real>(v.y, this->y - BOUNDS_OFFSET, this->y + BOUNDS_OFFSET)
//		&& IsInBounds<Standard_Real>(v.z, this->z - BOUNDS_OFFSET, this->z + BOUNDS_OFFSET))
//		return true;
//	return false;
//}

bool Vector3::operator==(const Vector3& v) const {
	if (this->x == v.x && this->y == v.y && this->z == v.z)
		return true;
	return false;
}

bool Vector3::operator<(const Vector3& v) const {
	if (this->x < v.x)
		return true;
	else if (this->x > v.x)
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

Vector3& Vector3::operator+=(const Vector3& v) {
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	return *this;
}

Vector3 Vector3::CrossProduct(const Vector3& b) const {
	// Given vector a and b, their cross product is: (a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x)
	return Vector3(this->y * b.z - this->z * b.y, this->z * b.x - this->x * b.z, this->x * b.y - this->y * b.x);
}

void Vector3::Normalize() {
	Standard_Real vectorMagnitude = sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
	if (vectorMagnitude != 0) {
		this->x /= vectorMagnitude;
		this->y /= vectorMagnitude;
		this->z /= vectorMagnitude;
	}
}

Vector3 Vector3::ReturnNormalizedInstance() {
	Standard_Real vectorMagnitude = sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
	return Vector3(this->x / vectorMagnitude, this->y / vectorMagnitude, this->z / vectorMagnitude);
}