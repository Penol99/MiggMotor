#pragma once
#include "Ray.h"
#include "LineSegment.h"
#include "Line.h"
#include "Math/Shapes/3D/AABB.h"

#include "AABB2D.h"
#include "LineSegment2D.h"
#include "Line2D.h"
#include "VectorConverter.h"

namespace mi
{
	namespace Converter
	{
		inline Ray ToRay(const LineSegment& aLineSegment)
		{
			return Ray(aLineSegment.Start(), aLineSegment.End() - aLineSegment.Start());
		}

		inline LineSegment ToLineSegment(const Ray& aRay, const float aLength)
		{
			return LineSegment(aRay.GetOrigin(), aRay.GetOrigin() + aRay.GetDirection() * aLength);
		}

		inline LineSegment ToLineSegmentXZ(const LineSegment2D& aLineSegment, const float aYPos = 0.f)
		{
			return LineSegment(ToVector3XZ(aLineSegment.Start(), aYPos), ToVector3XZ(aLineSegment.End(), aYPos));
		}

		inline Line ToLine(const LineSegment& aLineSegment)
		{
			return Line(aLineSegment.Start(), aLineSegment.End() - aLineSegment.Start());
		}

		inline Line ToLine(const Ray& aRay)
		{
			return Line(aRay.GetOrigin(), aRay.GetDirection());
		}

		inline AABB ToAABB(const LineSegment& aLineSegment)
		{
			return AABB::CreateFromPoints({ aLineSegment.Start(), aLineSegment.End() });
		}

		inline AABB ToAABBXY(const AABB2D& anAABB2D, const float aZPos = 0.f)
		{
			return AABB(ToVector3XY(anAABB2D.GetMin(), aZPos), ToVector3XY(anAABB2D.GetMax(), aZPos));
		}

		inline AABB ToAABBXZ(const AABB2D& anAABB2D, const float aYPos = 0.f)
		{
			return AABB(ToVector3XZ(anAABB2D.GetMin(), aYPos), ToVector3XZ(anAABB2D.GetMax(), aYPos));
		}

		inline AABB ToAABBYZ(const AABB2D& anAABB2D, const float aXPos = 0.f)
		{
			return AABB(ToVector3YZ(anAABB2D.GetMin(), aXPos), ToVector3YZ(anAABB2D.GetMax(), aXPos));
		}

		inline AABB2D ToAABB2D(const LineSegment2D& aLineSegment2D)
		{
			return AABB2D::CreateFromPoints({ aLineSegment2D.Start(), aLineSegment2D.End() });
		}

		inline AABB2D ToAABB2DXY(const AABB& anAABB)
		{
			return AABB2D(ToVector2XY(anAABB.GetMin()), ToVector2XY(anAABB.GetMax()));
		}

		inline AABB2D ToAABB2DXZ(const AABB& anAABB)
		{
			return AABB2D(ToVector2XZ(anAABB.GetMin()), ToVector2XZ(anAABB.GetMax()));
		}

		inline AABB2D ToAABB2DYZ(const AABB& anAABB)
		{
			return AABB2D(ToVector2YZ(anAABB.GetMin()), ToVector2YZ(anAABB.GetMax()));
		}

		inline Line2D ToLine2D(const LineSegment2D& aLineSegment1)
		{
			return Line2D(aLineSegment1.Start(), aLineSegment1.GetDirection());
		}
	}
}