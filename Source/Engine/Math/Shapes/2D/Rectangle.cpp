#include "engine_pch.h"
#include "Rectangle.h"
#include "Engine.h"


mi::Vector2i mi::Rectangle::Position() const
{
	return myPosition;
}

mi::Vector2i mi::Rectangle::SetPosition(Vector2i aPosition)
{
	myPosition = aPosition;
	return myPosition;
}

mi::Vector2i mi::Rectangle::Size() const
{
	return mySize;
}

mi::Vector2i mi::Rectangle::SetSize(Vector2i aSize)
{
	mySize = aSize;
	return mySize;
}

mi::Vector2f mi::Rectangle::Scale() const
{
	return myScale;
}

mi::Vector2f mi::Rectangle::SetScale(Vector2f aScaleVector)
{
	myScale = aScaleVector;
	return myScale;
}

bool mi::Rectangle::IsInside(Vector2f aPosition) const
{
	Vector2f pos = aPosition;
	return (pos.x >= myPosition.x - mySize.x / 2) &&
		(pos.x <= myPosition.x + mySize.x / 2) &&
		(pos.y >= myPosition.y - mySize.y / 2) &&
		(pos.y <= myPosition.y + mySize.y / 2);
}

