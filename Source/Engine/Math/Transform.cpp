#include "engine_pch.h"
#include "Transform.h"

namespace mi
{

	// These should really be in the Vector class itself.
#define SHUFFLEMASK(A0,A1,B2,B3) ( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) )
#define VectorLoadAligned(Ptr) _mm_load_ps((const float*) (Ptr))
#define VectorStoreAligned(Vec, Ptr) _mm_store_ps((float*) (Ptr), Vec)
#define VectorReplicate( Vec, ElementIndex )	_mm_shuffle_ps( Vec, Vec, SHUFFLEMASK(ElementIndex,ElementIndex,ElementIndex,ElementIndex) )


	Transform::Transform(Vector3f somePosition, Rotator someRotation, Vector3f someScale) : myPosition(somePosition),
		myRotation(someRotation), myScale(someScale)
	{
	}

	Transform::Transform(Vector3f somePosition, Quaternionf someRotation, Vector3f someScale) : myPosition(somePosition),
		myRotation(someRotation.GetEulerAnglesDegrees()), myScale(someScale)
	{
	}

	void Transform::Update(const Transform* aParentTransform)
	{

		myWorldTransform = GetLocalMatrixDirectX();
		if (aParentTransform)
		{
			myWorldTransform *= aParentTransform->myWorldTransform;
		}
	}

	Vector3f Transform::GetForward() const
	{
		DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(myRotation.x, myRotation.y, myRotation.z);
		DirectX::XMVECTOR camTarget = DirectX::XMVector3TransformCoord({ 0.f,0.f,1.f }, camRotationMatrix);
		DirectX::XMFLOAT3 vec;
		DirectX::XMStoreFloat3(&vec, camTarget);

		return { vec.x,vec.y,vec.z };
	}

	const DirectX::XMMATRIX& Transform::GetWorldTransform() const
	{
		return myWorldTransform;
	}

	Vector3f Transform::GetBack() const
	{
		DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(myRotation.x, myRotation.y, myRotation.z);
		DirectX::XMVECTOR camTarget = DirectX::XMVector3TransformCoord({ 0.f,0.f,-1.f }, camRotationMatrix);
		DirectX::XMFLOAT3 vec;
		DirectX::XMStoreFloat3(&vec, camTarget);

		return { vec.x,vec.y,vec.z };
	}

	Vector3f Transform::GetRight() const
	{
		DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(myRotation.x, myRotation.y, myRotation.z);
		DirectX::XMVECTOR camTarget = DirectX::XMVector3TransformCoord({ 1.f,0.f,0.f }, camRotationMatrix);
		DirectX::XMFLOAT3 vec;
		DirectX::XMStoreFloat3(&vec, camTarget);

		return { vec.x,vec.y,vec.z };
	}

	Vector3f Transform::GetLeft() const
	{
		DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(myRotation.x, myRotation.y, myRotation.z);
		DirectX::XMVECTOR camTarget = DirectX::XMVector3TransformCoord({ -1.f,0.f,0.f }, camRotationMatrix);
		DirectX::XMFLOAT3 vec;
		DirectX::XMStoreFloat3(&vec, camTarget);

		return { vec.x,vec.y,vec.z };
	}

	Vector3f Transform::GetUp() const
	{
		DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(myRotation.x, myRotation.y, myRotation.z);
		DirectX::XMVECTOR camTarget = DirectX::XMVector3TransformCoord({ 0.f,1.f,0.f }, camRotationMatrix);
		DirectX::XMFLOAT3 vec;
		DirectX::XMStoreFloat3(&vec, camTarget);

		return { vec.x,vec.y,vec.z };
	}

	void Transform::SetPosition(Vector3f somePosition)
	{
		myPosition = somePosition;
	}

	void Transform::SetRotation(Rotator someRotation)
	{
		myRotation = someRotation;
	}

	void Transform::SetScale(Vector3f someScale)
	{
		myScale = someScale;
	}

	void Transform::AddRotation(Rotator someRotation)
	{
		SetRotation(myRotation + someRotation);
	}


	void Transform::Deserialize(const TransformPureData& aPureData)
	{

		myPosition = aPureData.position;
		myRotation = aPureData.rotation * FMath::DegToRad;

		myRotation.x = (-myRotation.x) - 360.f * FMath::DegToRad;
		myRotation.y += 180.f * FMath::DegToRad;
		myRotation.z = (-myRotation.z) - 360.f * FMath::DegToRad;

		myScale = aPureData.scale;

		/*myPosition = aPureData.position;
		myRotation = aPureData.rotation;

		myRotation.x = (-myRotation.x) - 360.f;
		myRotation.y += 180.f;
		myRotation.z = (-myRotation.z) - 360.f;

		myScale = aPureData.scale;*/

	}


	Matrix4x4f Transform::GetMatrix(bool bNoScale) const
	{
		Matrix4x4f result = Matrix4x4f::CreateIdentityMatrix();

		if (bNoScale)
			result *= Matrix4x4f::CreateScaleMatrix({ 1, 1, 1 });
		else
			result *= Matrix4x4f::CreateScaleMatrix(myScale);

		result *= Matrix4x4f::CreateRollPitchYawMatrix(myRotation);
		result *= Matrix4x4f::CreateTranslationMatrix(myPosition);

		return result;
	}

	DirectX::XMMATRIX Transform::GetLocalMatrixDirectX() const
	{
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(
			myScale.x,
			myScale.y,
			myScale.z
		);

		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
			myRotation.x,
			myRotation.y,
			myRotation.z
		);

		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(
			myPosition.x,
			myPosition.y,
			myPosition.z
		);

		return scaleMatrix * rotationMatrix * translationMatrix;
	}

	VectorRegister Transform::VectorTransformVector(const VectorRegister& VecP, const void* MatrixM) const
	{
		const VectorRegister* M = (const VectorRegister*)MatrixM;
		VectorRegister VTempX, VTempY, VTempZ, VTempW;

		VTempX = VectorReplicate(VecP, 0);
		VTempY = VectorReplicate(VecP, 1);
		VTempZ = VectorReplicate(VecP, 2);
		VTempW = VectorReplicate(VecP, 3);

		VTempX = _mm_mul_ps(VTempX, M[0]);
		VTempY = _mm_mul_ps(VTempY, M[1]);
		VTempZ = _mm_mul_ps(VTempZ, M[2]);
		VTempW = _mm_mul_ps(VTempW, M[3]);

		VTempX = _mm_add_ps(VTempX, VTempY);
		VTempZ = _mm_add_ps(VTempZ, VTempW);
		VTempX = _mm_add_ps(VTempX, VTempZ);

		return VTempX;
	}

	Vector4f Transform::TransformVector4(const Vector4f& someVector, Matrix4x4f& Matrix) const
	{
		Vector4f Result;
		VectorRegister VecP = VectorLoadAligned(&someVector);
		VectorRegister VecR = VectorTransformVector(VecP, &Matrix);
		VectorStoreAligned(VecR, &Result);
		return Result;
	}

	Vector3f Transform::TransformPosition(Vector3f& someVector) const
	{
		Matrix4x4f Matrix = GetMatrix();
		return TransformVector4(Vector4f(someVector, 1.0f), Matrix).ToVector3();
	}

	Vector3f Transform::InverseTransformPosition(Vector3f& someVector) const
	{
		Matrix4x4f Matrix = GetMatrix(true).GetInverse();
		return TransformVector4(Vector4f(someVector, 1.0f), Matrix).ToVector3();
	}

#undef SHUFFLEMASK
#undef VectorLoadAligned
#undef VectorStoreAligned
#undef VectorReplicate
}

