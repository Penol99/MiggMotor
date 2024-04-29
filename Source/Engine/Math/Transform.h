#pragma once
#include "Vector3.h"
#include "Matrix.h"

namespace mi
{

	struct TransformPureData
	{
		int objectId;
		Vector3f position;
		Vector3f rotation;
		Vector3f scale;
	};

	typedef __m128 VectorRegister; // SIMD XMM[0-7] structure. We use it for vector registers.

	class Transform
	{

	public:
		Vector3f myPosition = Vector3f::Zero;
		Vector3f myRotation = Vector3f::Zero;
		Vector3f myScale = Vector3f::One;

	private:

		// Don't touch - should only be used in ModelComponent and SkinnedModelComponent
		DirectX::XMMATRIX myWorldTransform;

	public:

		Transform() = default;
		Transform(Vector3f somePosition, Rotator someRotation = Rotator::Zero, Vector3f someScale = Vector3f::One);
		Transform(Vector3f somePosition, Quaternionf someRotation, Vector3f someScale = Vector3f::One);

		const DirectX::XMMATRIX& GetWorldTransform() const;
		void Update(const Transform* aParentTransform);

		Quatf GetQuaternion() const { return Quatf(GetMatrix()); }
		Vector3f GetSize() const { return myScale; }

		Vector3f GetForward() const;
		Vector3f GetBack() const;
		Vector3f GetRight() const;
		Vector3f GetLeft() const;
		Vector3f GetUp() const;

		void SetPosition(Vector3f somePosition);
		void SetRotation(Rotator someRotation);
		void SetScale(Vector3f someScale);

		void AddRotation(Rotator someRotation);

		void Deserialize(const TransformPureData& aPureData);

		Matrix4x4f GetMatrix(bool bNoScale = false) const;
		DirectX::XMMATRIX GetLocalMatrixDirectX() const;
		VectorRegister VectorTransformVector(const VectorRegister& VecP, const void* MatrixM) const;
		Vector4f TransformVector4(const Vector4f& someVector, Matrix4x4f& Matrix) const;
		Vector3f TransformPosition(Vector3f& someVector) const;
		Vector3f InverseTransformPosition(Vector3f& someVector) const;
	};
}