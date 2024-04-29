#pragma once
#include <cmath>
#include <iostream>
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include "FMathConstants.h"


namespace mi
{
	template <class T>
	class Vector2
	{
	public:
		union
		{
			T x;
			T X;
			T myX;
		};
		union
		{
			T y;
			T Y;
			T myY;
		};

		Vector2<T>();
		~Vector2<T>() = default;
		Vector2<T>(const T& aX, const T& aY);
		Vector2<T>(const Vector2<T>& aVector) = default;
		Vector2<T>(const T& aScalar);

		template <class U>
		explicit operator Vector2<U>() { return { static_cast<U>(myX), static_cast<U>(myY) }; }

		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;

		template <class OtherType>
		Vector2<T>& operator=(const Vector2<OtherType>& aVector) { X = (T)aVector.X; Y = (T)aVector.Y; return *this; }

		T LengthSqr() const;
		T Length() const;

		Vector2<T> GetNormalized() const;
		Vector2<T>& Normalize();
		Vector2<T> Normal() const;

		T Cross(const Vector2& aVector) const;

		T Dot(const Vector2<T>& aVector) const;

		static T CalculateAngleInDegrees(Vector2<T> aVector1, Vector2<T> aVector2);
		static Vector2<T> CreateFromRadians(const T aRadians);

		void Set(const T& aX, const T& aY);
		
		T HorizontalAR();
		T VerticalAR();

		//Vector2 operator/(const T aType) const;

		template <typename TO> Vector2<TO> GetAs() const;

		template <class U>
		friend std::ostream& operator<<(std::ostream& os, const Vector2<U>& aVector);

		static Vector2<T> Abs(const Vector2<T>& aVector);
		static T Distance(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
		static Vector2<T> Lerp(const Vector2<T>& aStart, const Vector2<T>& aEnd, const float aPercent);
		static Vector2<T> NLerp(const Vector2<T>& aStart, const Vector2<T>& aEnd, const float aPercent);
		static Vector2<T> ScalarAr1ToAR2(const T aSourceAspectRatio, const T aDestinationAspectRatio);
	};

	typedef Vector2<float> Vector2f;
	typedef Vector2<unsigned int>  Vector2ui;
	typedef Vector2<int>  Vector2i;

	template <class T> Vector2<T> operator-(const Vector2<T>& aVector);
	template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar);
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar);
	//template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const int& aScalar);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> bool operator==(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> bool operator<(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> bool operator>(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> bool operator<=(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> bool operator>=(const Vector2<T>& aVector0, const Vector2<T>& aVector1);

	template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar);
	template <class T> std::istream& operator>>(std::istream& in, Vector2<T>& aVec);


#pragma region MemberDefinitions

	template<class T>
	std::istream& operator>>(std::istream& in, Vector2<T>& aVec)
	{
		in >> aVec.X;
		in >> aVec.Y;
		return in;
	}

	template<class T>
	inline Vector2<T>::Vector2() : Vector2(0, 0)
	{}

	template<class T>
	inline Vector2<T>::Vector2(const T& aX, const T& aY) : X(aX), Y(aY)
	{}

	template <class T>
	Vector2<T>::Vector2(const T& aScalar) : X(aScalar), Y(aScalar)
	{}

	template<class T>
	inline T Vector2<T>::LengthSqr() const
	{
		return (X * X) + (Y * Y);
	}

	template<class T>
	inline T Vector2<T>::Length() const
	{
		return sqrt((X * X) + (Y * Y));
	}

	template<class T>
	inline Vector2<T> Vector2<T>::GetNormalized() const
	{
		const T magnitude = (X * X) + (Y * Y);

		if (magnitude == 0)
		{
			return *this;
		}

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		return Vector2<T>(X * inversedMagnitude, Y * inversedMagnitude);
	}

	template<class T>
	inline Vector2<T>& Vector2<T>::Normalize()
	{
		const T magnitude = (X * X) + (Y * Y);

		if (magnitude == 0)
		{
			return *this;
		}

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		X *= inversedMagnitude;
		Y *= inversedMagnitude;
		return *this;
	}

	// Returns a copy of the non-normalized normal.
	template<class T> inline Vector2<T> Vector2<T>::Normal() const
	{
		return Vector2<T>(Y, -X);
	}

	template<typename T>
	T Vector2<T>::Cross(const Vector2& aVector) const
	{
		return (myX * aVector.myY) - (myY * aVector.myX);
	}

	template<class T>
	inline T Vector2<T>::Dot(const Vector2<T>& aVector) const
	{
		return (X * aVector.X) + (Y * aVector.Y);
	}

	template<class T>
	inline T Vector2<T>::CalculateAngleInDegrees(Vector2<T> aVector1, Vector2<T> aVector2)
	{
		aVector1.Normalize();
		aVector2.Normalize();

		const float dotProduct = aVector1.Dot(aVector2);
		const float crossProduct = aVector2.Cross(aVector1);

		const float angleRadians = acos(dotProduct);
		float angleDegrees = angleRadians * FMath::RadToDeg;

		if (crossProduct < 0)
			angleDegrees = -angleDegrees;  // Flip the sign for clockwise rotations

		return angleDegrees;
	}

	template<class T>
	inline Vector2<T> Vector2<T>::CreateFromRadians(const T aRadians)
	{
		return Vector2<T>(std::cos(aRadians), std::sin(aRadians));
	}

	template<class T>
	inline void Vector2<T>::Set(const T& aX, const T& aY)
	{
		X = aX;
		Y = aY;
	}

	template<class T>
	inline T Vector2<T>::HorizontalAR()
	{
		return T(x/y);
	}

	template<class T>
	inline T Vector2<T>::VerticalAR()
	{
		return T(y/x);
	}

	template<class T>
	template<typename TO>
	inline Vector2<TO> Vector2<T>::GetAs() const
	{
		return {TO(myX), TO(myY)};
	}

#pragma endregion MemberDefinitions

#pragma region OperatorDefinitions

	template<class T>
	Vector2<T> operator-(const Vector2<T>& aVector)
	{
		return Vector2<T>(-aVector.x, -aVector.y);
	}

	template <class T>
	Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.X + aVector1.X, aVector0.Y + aVector1.Y);
	}

	template<class T>
	Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.X - aVector1.X, aVector0.Y - aVector1.Y);
	}

	template<class T>
	Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.X * aVector1.X, aVector0.Y * aVector1.Y);
	}

	template <class T>
	Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.X * aScalar, aVector.Y * aScalar);
	}

	template <class T>
	Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		return aVector * aScalar;
	}

	template <class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		//return aVector * (1 / aScalar);
		return { aVector.X /aScalar, aVector.Y / aScalar };
	}

	//template <class T>
	//Vector2<T> operator/(const Vector2<T>& aVector, const int& aScalar)
	//{
	//	return { aVector.X / static_cast<T>(aScalar), aVector.Y / static_cast<T>(aScalar) };
	//}

	template<class T>
	Vector2<T> operator/(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>{ aVector0.X / aVector1.X, aVector0.Y / aVector1.Y };
	}

	template<class T>
	bool operator==(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return aVector0.X == aVector1.X && aVector0.Y == aVector1.Y;
	}

	template<class T>
	bool operator<(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return aVector0.x < aVector1.x && aVector0.y < aVector1.y;
	}

	template<class T>
	bool operator>(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return aVector0.x > aVector1.x && aVector0.y > aVector1.y;
	}

	template<class T>
	bool operator<=(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return aVector0.x <= aVector1.x && aVector0.y <= aVector1.y;
	}

	template<class T>
	bool operator>=(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return aVector0.x >= aVector1.x && aVector0.y >= aVector1.y;
	}

	template <class T>
	void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.X += aVector1.X;
		aVector0.Y += aVector1.Y;
	}

	template <class T>
	void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.X -= aVector1.X;
		aVector0.Y -= aVector1.Y;
	}

	template <class T>
	void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.X *= aScalar;
		aVector.Y *= aScalar;
	}

	template <class T>
	void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		const T inv = (1 / aScalar);
		aVector.X *= inv;
		aVector.Y *= inv;
	}
	template <class T>
	std::ostream& operator<<(std::ostream& os, const Vector2<T>& aVector)
	{
		return os << "{ X: " << aVector.X << " Y: " << aVector.Y << " }";
	}
#pragma endregion OperatorDefinitions

#pragma region Static Functions

	template<class T>
	inline Vector2<T> Vector2<T>::Abs(const Vector2<T>& aVector)
	{
		return { std::abs(aVector.X), std::abs(aVector.Y) };
	}

	template <class T>
	inline T Vector2<T>::Distance(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		const Vector2<T> direction = aVector1 - aVector0;
		return direction.Length();
	}
	template<class T>
	inline Vector2<T> Vector2<T>::Lerp(const Vector2<T>& aStart, const Vector2<T>& aEnd, const float aPercent)
	{
		return (aStart + aPercent * (aEnd - aStart));
	}
	template<class T>
	inline Vector2<T> Vector2<T>::NLerp(const Vector2<T>& aStart, const Vector2<T>& aEnd, const float aPercent)
	{
		return Lerp(aStart, aEnd, aPercent).Normalize();
	}

	/// <summary>
	/// Returns a Vector2 that can be multiplied with a size vector in a source aspect ratio to adjust it to keep pixels square when rendered in a destination aspect ratio.
	/// Aspect ratios are horisonatal ie. width/height.
	/// </summary>
	template<class T>
	inline Vector2<T> Vector2<T>::ScalarAr1ToAR2(const T aSourceAspectRatio, const T aDestinationAspectRatio)
	{
		return { aDestinationAspectRatio / aSourceAspectRatio, T(1) };
	}

#pragma endregion Static Functions
} // namespace Tga
