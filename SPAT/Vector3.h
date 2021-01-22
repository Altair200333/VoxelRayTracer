#pragma once
#include <array>
#include <glm/vec3.hpp>

class Vector3 final
{
public:
    union
    {
        struct
        {
            float x, y, z;
        };
        std::array<float, 3> v;
    };

    Vector3();

    Vector3(const glm::vec3& vec);
	
    Vector3(float val):Vector3(val, val, val){}

    Vector3(const float _x, const float _y, const float _z);

    Vector3 operator+(const Vector3& other) const;

    Vector3 operator-(const Vector3& other) const;

    Vector3 operator-() const;

    Vector3 operator*(float val) const;

    float& operator[](size_t idx);
    const float& operator[](size_t idx) const;

    Vector3& operator+=(const Vector3& other);

    Vector3 operator/(const Vector3& other) const;

    [[nodiscard]] float dot(const Vector3& other) const;

    [[nodiscard]] Vector3 cross(const Vector3& other) const;

    [[nodiscard]] float magnitude() const;

    [[nodiscard]] Vector3 normalized() const;

    [[nodiscard]] float squareDist(const Vector3& v2) const;

    [[nodiscard]] glm::vec3 toGlm() const
    {
        return { x, y, z };
    }
};
