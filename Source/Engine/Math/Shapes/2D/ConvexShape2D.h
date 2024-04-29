#pragma once
#include <vector>
#include "Vector2.h"

namespace mi
{

	class ConvexShape2D
	{
	public:

		explicit ConvexShape2D(const std::vector<Vector2f>& someVertices);
		explicit ConvexShape2D(const Vector2f& aPosition, const std::vector<Vector2f>& someVertices);
		~ConvexShape2D() = default;

		const std::vector<Vector2f>& GetVertices() const;

		const Vector2f& GetPosition() const;

		void SetPosition(const Vector2f& aPosition);

	private:


		Vector2f myPosition;
		std::vector<Vector2f> myVertices;

	};
}