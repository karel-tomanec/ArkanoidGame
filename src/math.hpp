#pragma once
#include <cmath>
#include <numbers>
#include <string>
#include <format>

constexpr float pi = std::numbers::pi_v<float>;

struct Vector2
{
	float x, y;

	constexpr Vector2() : x(0.0f), y(0.0f) {}
	constexpr Vector2(float value) : x(value), y(value) {}
	constexpr Vector2(float x, float y) : x(x), y(y) {}

	constexpr float& operator[](int i)
	{
		return (i == 0) ? x : y;
	}

	constexpr const float& operator[](int i) const
	{
		return (i == 0) ? x : y;
	}

	constexpr Vector2& operator*=(float s)
	{
		x *= s; y *= s; return *this;
	}

	constexpr Vector2& operator/=(float s)
	{
		s = 1.0f / s;
		x *= s; y *= s; return *this;
	}

	constexpr Vector2& operator+=(const Vector2& v)
	{
		x += v.x; y += v.y; return *this;
	}

	constexpr Vector2& operator-=(const Vector2& v)
	{
		x -= v.x; y -= v.y; return *this;
	}

	std::string toString() const
	{
		return std::format("Vector2({}, {})", x, y);
	}
};

constexpr Vector2 operator*(const Vector2& v, float s)
{
	return { v.x * s, v.y * s };
}

constexpr Vector2 operator*(float s, const Vector2& v)
{
	return v * s;
}

constexpr Vector2 operator*(const Vector2& a, const Vector2& b)
{
	return { a.x * b.x, a.y * b.y };
}

constexpr Vector2 operator/(const Vector2& v, float s)
{
	return { v.x / s, v.y / s };
}

constexpr Vector2 operator-(const Vector2& v)
{
	return { -v.x, -v.y };
}

constexpr Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return { a.x + b.x, a.y + b.y };
}

constexpr Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return { a.x - b.x, a.y - b.y };
}

constexpr float dot(const Vector2& a, const Vector2& b)
{
	return a.x * b.x + a.y * b.y;
}

constexpr float lengthSquared(const Vector2& v)
{
	return dot(v, v);
}

inline float length(const Vector2& v)
{
	return std::sqrt(lengthSquared(v));
}

inline Vector2 normalize(const Vector2& v)
{
	return v / length(v);
}

inline Vector2 reflect(const Vector2& v, const Vector2& n)
{
	return v - 2.f * dot(v, n) * n;
}

struct Segment
{
	Vector2 a;
	Vector2 b;
};

struct Sphere
{
	Vector2 center;
	float radius;
};

struct Capsule
{
	Vector2 a;
	Vector2 b;
	float radius;
};

struct AABB
{
	Vector2 min;
	Vector2 max;

	Vector2 corner(int index) const
	{
		return {
			(index & 1) ? max.x : min.x,
			(index & 2) ? max.y : min.y
		};
	}
};

inline Vector2 randomPointInCircle(float radius, const Vector2& rnd)
{
	float angle = rnd.x * 2.f * pi;
	float r = std::sqrt(rnd.y) * radius;

	return { r * std::cos(angle), r * std::sin(angle) };
}

inline Vector2 randomPointInRectangle(const Vector2& size, const Vector2& rnd)
{
	return { rnd.x * size.x - size.x * 0.5f, rnd.y * size.y - size.y * 0.5f };
}