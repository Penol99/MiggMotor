#pragma once
#include "Vector2.h"

namespace mi
{

	class Ray2D
	{
	public:

		Ray2D();
		Ray2D(const Vector2f& anOrigin, const Vector2f& aDirection);

		const Vector2f& GetOrigin() const;
		void SetOrigin(const Vector2f& anOrigin);

		const Vector2f& GetDirection() const;
		void SetDirection(const Vector2f& aDirection);

		Vector2f GetPointWithScalar(const float aScalar) const;
		Vector2f GetPointWithXPos(const float aXPos) const;
		Vector2f GetPointWithYPos(const float aYPos) const;


	private:
		Vector2f myOrigin;
		Vector2f myDirection;
	};
}