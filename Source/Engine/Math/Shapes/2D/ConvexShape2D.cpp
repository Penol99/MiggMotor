#include "engine_pch.h"
#include "ConvexShape2D.h"

namespace mi
{
	ConvexShape2D::ConvexShape2D(const std::vector<Vector2f>& someVertices)
		: myVertices(someVertices)
	{
	}

	ConvexShape2D::ConvexShape2D(const Vector2f& aPosition, const std::vector<Vector2f>& someVertices)
		: myPosition(aPosition)
		, myVertices(someVertices)
	{
	}

	const std::vector<Vector2f>& ConvexShape2D::GetVertices() const
	{
		return myVertices;
	}

	const Vector2f& ConvexShape2D::GetPosition() const
	{
		return myPosition;
	}

	void ConvexShape2D::SetPosition(const Vector2f& aPosition)
	{
		myPosition = aPosition;
	}

}

