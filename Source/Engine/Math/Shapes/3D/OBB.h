#pragma once
#include "Vector.h"
#include "Math/Matrix4x4.h"

namespace mi
{
	class OBB
	{
	public:

		OBB() = default;
		OBB(const Vector3f& aCenter, const Vector3f& aExtents);
		
		Vector3f GetCenter() const;
		void SetCenter(const Vector3f& aCenter);
		void SetExtents(const Vector3f& aExtents);
		void Rotate(const Matrix4x4f& aRotationMatrix);

		Matrix4x4f myMatrix;
		Vector3f myHalfExtents;
	};
}