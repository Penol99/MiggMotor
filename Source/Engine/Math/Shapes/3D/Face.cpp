#include "engine_pch.h"
#include "Face.h"

namespace mi
{
	Face::Face(const Vector3f& aVertex1, const Vector3f& aVertex2, const Vector3f& aVertex3)
		: myVertex1(aVertex1)
		, myVertex2(aVertex2)
		, myVertex3(aVertex3)
	{
	}

	Vector3f Face::GetNormal() const
	{
		return (myVertex2 - myVertex1).Cross(myVertex3 - myVertex1).GetNormalized();
	}

	Plane Face::ToPlane() const
	{
		return Plane(myVertex1, GetNormal());
	}

}
