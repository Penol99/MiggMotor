#pragma once
#include "vector2.h"

namespace mi
{
	template<typename T>
	struct QuadraticFunction
	{
		T a = 0, b = 0, c = 0;

		T operator()(const T aValue) const;

		Vector2<T> GetVertex() const;
		T GetFirstRootX() const;
		T GetSecondRootX() const;

		T GetFirstX(const T aY) const;
		T GetSecondX(const T aY) const;

		std::pair<T, T> SolveQuadratic(const T aCPrime) const;

	private:

		template<typename Operator>
		T SolveQuadratic(const T aY, Operator anOperator) const;
	};

	using QuadraticFunctionf = QuadraticFunction<float>;

	template<typename T>
	inline T QuadraticFunction<T>::operator()(const T anX) const
	{
		return a * anX * anX + b * anX + c;
	}

	template<typename T>
	inline Vector2<T> QuadraticFunction<T>::GetVertex() const
	{
		const T xValue = -b / (2 * a);
		return Vector2<T>(xValue, (*this)(xValue));
	}

	template<typename T>
	inline T QuadraticFunction<T>::GetFirstRootX() const
	{
		return (-b - std::sqrt(b * b) - 4 * a * c) / (2 * a);
	}

	template<typename T>
	inline T QuadraticFunction<T>::GetSecondRootX() const
	{
		return (-b + std::sqrt(b * b - 4 * a * c)) / (2 * a);
	}

	template<typename T>
	inline T QuadraticFunction<T>::GetFirstX(const T aY) const
	{
		return SolveQuadratic(aY, std::minus<T>());
	}

	template<typename T>
	inline T QuadraticFunction<T>::GetSecondX(const T aY) const
	{
		/*T discriminant = b * b - 4 * a * aY;

		if (discriminant < 0)
		{
			return std::numeric_limits<T>::quiet_NaN();
		}

		T sqrtDiscriminant = std::sqrt(discriminant);*/
		T root = anOperator(-b, b) / (2 * a);

		return root - c;
		//return SolveQuadratic(aY, std::plus<T>());
	}

	template<typename T>
	inline std::pair<T, T> QuadraticFunction<T>::SolveQuadratic(const T aCPrime) const
	{
		T discriminant = b * b - 4 * a * (c - aCPrime);

		if (discriminant < 0)
		{
			// No real roots
			return { std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN() };
		}

		T sqrtDiscriminant = std::sqrt(discriminant);
		T root1 = (-b + sqrtDiscriminant) / (2 * a);
		T root2 = (-b - sqrtDiscriminant) / (2 * a);

		return { root1, root2 };
	}

	

	template<typename T>
	template<typename Operator>
	inline T QuadraticFunction<T>::SolveQuadratic(const T aY, Operator anOperator) const
	{
		T discriminant = b * b - 4 * a * 0;

		if (discriminant < 0) 
		{
			return std::numeric_limits<T>::quiet_NaN();
		}

		T sqrtDiscriminant = std::sqrt(discriminant);
		T root = anOperator(-b, sqrtDiscriminant) / (2 * a);

		return root - c;
	}

}