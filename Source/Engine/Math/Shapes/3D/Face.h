#pragma once
#include "Vector3.h"
#include "Plane.h"

namespace mi
{
	class Face
	{
	public:

		Face() = default;
		Face(const Vector3f& aVertex1, const Vector3f& aVertex2, const Vector3f& aVertex3);
		~Face() = default;

		Vector3f GetNormal() const;

		Plane ToPlane() const;

	private:

		Vector3f myVertex1, myVertex2, myVertex3;
	};
}