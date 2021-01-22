#include "Vector3.h"

Vector3::Vector3(): Vector3(0, 0, 0)
{
}

Vector3::Vector3(const glm::vec3& vec): Vector3(vec.x, vec.y, vec.z)
{
}

Vector3::Vector3(const float _x, const float _y, const float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

Vector3 Vector3::operator+(const Vector3& other) const
{
	return {x + other.x, y + other.y, z + other.z};
}

Vector3 Vector3::operator-(const Vector3& other) const
{
	return {x - other.x, y - other.y, z - other.z};
}

Vector3 Vector3::operator-() const
{
	return {-x, -y, -z};
}

Vector3 Vector3::operator*(float val) const
{
	return {x * val, y * val, z * val};
}

float& Vector3::operator[](size_t idx)
{
	return v[idx];
}

const float& Vector3::operator[](size_t idx) const
{
	return v[idx];
}

Vector3& Vector3::operator+=(const Vector3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vector3 Vector3::operator/(const Vector3& other) const
{
	Vector3 v = *this;
	v.x /= other.x;
	v.y /= other.y;
	v.z /= other.z;
	return v;
}

float Vector3::dot(const Vector3& other) const
{
	return other.x * x + other.y * y + other.z * z;
}

Vector3 Vector3::cross(const Vector3& other) const
{
	return {y * other.z - z * y, z * other.x - x * other.z, x * other.y - y * other.x};
}

float Vector3::magnitude() const
{
	return sqrtf(x * x + y * y + z * z);
}

Vector3 Vector3::normalized() const
{
	return *this * (1.0f / magnitude());
}

float Vector3::squareDist(const Vector3& v2) const
{
	return (x - v2.x) * (x - v2.x) + (y - v2.y) * (y - v2.y) + (z - v2.z) * (z - v2.z);
}
