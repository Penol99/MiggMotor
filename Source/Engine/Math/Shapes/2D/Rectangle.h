#pragma once
#include "Vector.h"

namespace mi
{
	class Rectangle
	{
	public:
		Rectangle() = default;
		~Rectangle() = default;
		Rectangle(const Vector2i aPosition, const Vector2i aSize = {1, 1}, const Vector2f aScale = {1, 1}) 
			: myPosition(aPosition), mySize(aSize), myScale(aScale)
		{
		};
		Vector2i Position() const;
		Vector2i SetPosition(Vector2i aPosition);
		
		Vector2i Size() const;
		Vector2i SetSize(Vector2i aSize);
		
		Vector2f Scale() const;
		Vector2f SetScale(Vector2f aScaleVector);

		bool IsInside(Vector2f aPosition) const;

	private:
		friend class UI;
		Vector2i myPosition{};
		Vector2i mySize{};
		Vector2f myScale = {1.f, 1.f};
	};
}