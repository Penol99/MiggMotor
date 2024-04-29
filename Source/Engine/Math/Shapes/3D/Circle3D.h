#pragma once
#include "Matrix4x4.h"

namespace mi
{
	class Circle3D
	{
	public:

		Circle3D() = default;
		Circle3D(const Vector3f& aCenter, const float aRadius);
		~Circle3D() = default;

		const Vector3f& GetCenter() const;
		float GetRadius() const;
		const Matrix4x4f& GetMatrix() const;

		void SetCenter(const Vector3f& aCenter);
		void SetRadius(const float aRadius);

	private:

		float myRadius;
		Matrix4x4f myMatrix;
	};

}