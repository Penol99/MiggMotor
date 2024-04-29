#pragma once
#include "Vector2.h"

namespace mi
{
	template<typename T>
	class Triangle
	{
	public:

		Triangle() = default;
		Triangle(const Vector2<T>& aVertex1, const Vector2<T>& aVertex2, const Vector2<T>& aVertex3);
		~Triangle() = default;

		Vector2<T>& Vertex1();
		Vector2<T>& Vertex2();
		Vector2<T>& Vertex3();

		const Vector2<T>& Vertex1() const;
		const Vector2<T>& Vertex2() const;
		const Vector2<T>& Vertex3() const;

		Vector2<T> GetMiddle() const;

	private:

		Vector2<T> myVertex1, myVertex2, myVertex3;
	};

	using Trianglef = Triangle<float>;

	template<typename T>
	inline Triangle<T>::Triangle(const Vector2<T>& aVertex1, const Vector2<T>& aVertex2, const Vector2<T>& aVertex3)
		: myVertex1(aVertex1)
		, myVertex2(aVertex2)
		, myVertex3(aVertex3)
	{
	}

	template<typename T>
	inline Vector2<T>& Triangle<T>::Vertex1()
	{
		return myVertex1;
	}

	template<typename T>
	inline Vector2<T>& Triangle<T>::Vertex2()
	{
		return myVertex2;
	}

	template<typename T>
	inline Vector2<T>& Triangle<T>::Vertex3()
	{
		return myVertex3;
	}

	template<typename T>
	inline const Vector2<T>& Triangle<T>::Vertex1() const
	{
		return myVertex1;
	}

	template<typename T>
	inline const Vector2<T>& Triangle<T>::Vertex2() const
	{
		return myVertex2;
	}

	template<typename T>
	inline const Vector2<T>& Triangle<T>::Vertex3() const
	{
		return myVertex3;
	}

	template<typename T>
	inline Vector2<T> Triangle<T>::GetMiddle() const
	{
		return (myVertex1 + myVertex2 + myVertex3) / ((T)3);
	}
}