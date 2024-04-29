#pragma once
#include "Vector.h"

namespace mi
{
	namespace Converter
	{
		template<typename T>
		Vector2<T> ToVector2XY(const Vector3<T>& aVector3)
		{
			return Vector2<T>(aVector3.x, aVector3.y);
		}

		template<typename T>
		Vector2<T> ToVector2XZ(const Vector3<T>& aVector3)
		{
			return Vector2<T>(aVector3.x, aVector3.z);
		}

		template<typename T>
		Vector2<T> ToVector2YZ(const Vector3<T>& aVector3)
		{
			return Vector2<T>(aVector3.y, aVector3.z);
		}

		template<typename T>
		Vector3<T> ToVector3XY(const Vector2<T>& aVector2, const T aZPos = 0.f)
		{
			return Vector3<T>(aVector2.x, aVector2.y, aZPos);
		}

		template<typename T>
		Vector3<T> ToVector3XZ(const Vector2<T>& aVector2, const T aYPos = 0.f)
		{
			return Vector3<T>(aVector2.x, aYPos, aVector2.y);
		}

		template<typename T>
		Vector3<T> ToVector3YZ(const Vector2<T>& aVector2, const T aXPos = 0.f)
		{
			return Vector3<T>(aXPos, aVector2.x, aVector2.y);
		}

		template<typename T>
		Vector3<T> ToVector3(const Vector4<T>& aVector4)
		{
			return Vector3<T>(aVector4.x, aVector4.y, aVector4.z);
		}
	}
	
}