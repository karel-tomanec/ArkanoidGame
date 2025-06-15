#pragma once

#include <algorithm>
#include <optional>

struct HitInfo
{
	Vector2 intersection;
	Vector2 normal;
	float t;
};

static constexpr float kEpsilon = 1e-18f;

// Source: Real-Time Collision Detection by Christer Ericson
// Intersect ray R(t) = p + t*d against AABB a. When intersecting,
// return intersection distance tmin and point q of intersection
inline bool intersectRayAABB(const Vector2& p, const Vector2& d, const AABB& a, float& tMin, Vector2& q)
{
	tMin = 0.f; // set to -FLT_MAX to get first hit on line
	float tMax = FLT_MAX; // set to max distance ray can travel (for segment)

	// For all three slabs
	for (uint8_t i = 0; i < 2; i++)
	{
		if (std::abs(d[i]) < kEpsilon)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (p[i] < a.min[i] || p[i] > a.max[i])
				return false;
		}
		else
		{
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.f / d[i];
			float t1 = (a.min[i] - p[i]) * ood;
			float t2 = (a.max[i] - p[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2)
				std::swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			tMin = std::max(t1, tMin);
			tMax = std::min(t2, tMax);
			// Exit with no collision as soon as slab intersection becomes empty
			if (tMin > tMax)
				return false;
		}
	}

	if (tMin < kEpsilon) // prevent touching artifacts
		return false;

	// Ray intersects all 2 slabs. Return point (q) and intersection t value (tmin)
	q = p + d * tMin;
	return true;
}

// Computes closest points C1 and C2 of S1(s) = P1 + s * (Q1 - P1) and
// S2(t)=P2+t*(Q2-P2), returning s and t. Function result is squared
// distance between between S1(s) and S2(t)
inline float closestPtSegmentSegment(const Vector2& p1, const Vector2& q1, const Vector2& p2, const Vector2& q2, float& s, float& t, Vector2& c1, Vector2& c2)
{
	Vector2 d1 = q1 - p1;
	Vector2 d2 = q2 - p2;
	Vector2 r = p1 - p2;
	float a = dot(d1, d1);
	float e = dot(d2, d2);
	float f = dot(d2, r);

	if (a <= kEpsilon && e <= kEpsilon)
	{
		s = t = 0.0f;
		c1 = p1;
		c2 = p2;
		return dot(c1 - c2, c1 - c2);
	}
	if (a <= kEpsilon)
	{
		s = 0.0f;
		t = f / e;
		t = std::clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = dot(d1, r);
		if (e <= kEpsilon)
		{
			t = 0.0f;
			s = std::clamp(-c / a, 0.0f, 1.0f);
		}
		else
		{
			float b = dot(d1, d2);
			float denom = a * e - b * b;

			if (denom != 0.0f)
			{
				s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}
			else
				s = 0.0f;

			t = (b * s + f) / e;

			if (t < 0.0f)
			{
				t = 0.0f;
				s = std::clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f)
			{
				t = 1.0f;
				s = std::clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}
	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;
	return dot(c1 - c2, c1 - c2);
}

inline bool intersectSegmentSphere(const Segment& seg, const Sphere& s, float& t)
{
	Vector2 d = seg.b - seg.a;
	Vector2 m = seg.a - s.center;
	float b = dot(m, d);
	float c = dot(m, m) - s.radius * s.radius;
	if (c > 0.0f && b > 0.0f)
		return false;
	const float discr = b * b - c;
	if (discr < 0.0f)
		return false;
	t = -b - sqrtf(discr);
	t = std::max(t, 0.0f);
	return true;
}

inline bool intersectSegmentCapsule(const Segment& seg, const Capsule& cap, float& t, Vector2& outPoint, Vector2& outNormal)
{
	float s, tc;
	Vector2 c1, c2;

	float distSq = closestPtSegmentSegment(seg.a, seg.b, cap.a, cap.b, s, tc, c1, c2);

	if (distSq <= cap.radius * cap.radius)
	{

		Sphere s;
		s.center = c2;
		s.radius = cap.radius;

		if (!intersectSegmentSphere(seg, s, t))
			return false;

		outPoint = seg.a + (seg.b - seg.a) * t;
		outNormal = normalize(outPoint - s.center);

		return true;
	}
	return false;
}

// Source: Real-Time Collision Detection by Christer Ericson
inline std::optional<HitInfo> intersectMovingSphereAABB(const Sphere& s, const Vector2& d, const AABB& b)
{
	AABB e = b;
	e.min.x -= s.radius;
	e.min.y -= s.radius;
	e.max.x += s.radius;
	e.max.y += s.radius;

	Vector2 p;
	float t;
	if (!intersectRayAABB(s.center, d, e, t, p) || t > 1.f)
		return std::nullopt;

	int u = 0, v = 0;
	if (p.x < b.min.x) u |= 1;
	if (p.x > b.max.x) v |= 1;
	if (p.y < b.min.y) u |= 2;
	if (p.y > b.max.y) v |= 2;

	int m = u + v;
	Segment seg{ s.center, s.center + d };

	if (m == 3) // Vertex region
	{
		float tMin = FLT_MAX;
		Vector2 bestNormal;
		Vector2 bestPoint;

		Capsule edge1{ b.corner(v), b.corner(v ^ 1), s.radius };
		Capsule edge2{ b.corner(v), b.corner(v ^ 2), s.radius };

		float tempT;
		Vector2 p1, n1, p2, n2;

		if (intersectSegmentCapsule(seg, edge1, tempT, p1, n1) && tempT < tMin)
		{
			tMin = tempT;
			bestPoint = p1;
			bestNormal = n1;
		}
		if (intersectSegmentCapsule(seg, edge2, tempT, p2, n2) && tempT < tMin)
		{
			tMin = tempT;
			bestPoint = p2;
			bestNormal = n2;
		}

		if (tMin == FLT_MAX)
			return std::nullopt;

		t = tMin;

		return std::make_optional<HitInfo>(bestPoint, bestNormal, t);
	}

	if ((m & (m - 1)) == 0) // Face region
	{
		// Intersection point on expanded AABB is valid
		Vector2 normal;
		switch (m)
		{
			case 1: normal = Vector2{ -1, 0 }; break; // Left
			case 2: normal = Vector2{ 0, -1 }; break; // Bottom
			case 4: normal = Vector2{ 1, 0 }; break;  // Right
			case 8: normal = Vector2{ 0, 1 }; break;  // Top
		}
		return std::make_optional<HitInfo>(p, normal, t);
	}

	Vector2 pt, n;
	if (intersectSegmentCapsule(seg, Capsule{ b.corner(u ^ 3), b.corner(v), s.radius }, t, pt, n))
	{
		return std::make_optional<HitInfo>(pt, n, t);
	}
	return std::nullopt;
}