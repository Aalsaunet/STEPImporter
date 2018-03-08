#pragma once
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
