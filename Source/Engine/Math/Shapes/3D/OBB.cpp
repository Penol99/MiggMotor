#include "engine_pch.h"
#include "OBB.h"

namespace mi
{
	OBB::OBB(const Vector3f& aCenter, const Vector3f& aExtents)
		: myHalfExtents(aExtents / 2.f)
	{
		myMatrix.SetPosition(aCenter);
	}

	Vector3f OBB::GetCenter() const
	{
		return myMatrix.GetPosition();
	}

	void OBB::SetCenter(const Vector3f& aCenter)
	{
		myMatrix.SetPosition(aCenter);
	}

	void OBB::SetExtents(const Vector3f& aExtents)
	{
		myHalfExtents = aExtents / 2.f;
	}

	void OBB::Rotate(const Matrix4x4f& aRotationMatrix)
	{
		myMatrix.RotateAroundSelf(aRotationMatrix);
	}

}
