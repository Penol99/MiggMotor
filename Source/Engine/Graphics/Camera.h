#pragma once
#include <DirectXMath.h>
#include <Ray.h>
#include <Vector.h>
#include <Component.h>

using namespace DirectX;

namespace mi
{

	struct CameraPureData
	{
		int objectId;
		float farPlane;
		float nearPlanes;
		float fov;
		Vector3f position;
		Vector3f rotation;
	};

	const CameraPureData myDefaultData =
	{
		-1,
		10001,
		0.1f,
		90.f,
		Vector3f::Zero,
		Vector3f::Zero
	};


	class Camera
	{
	public:
		Camera();
		~Camera();

		Camera& operator=(const Camera& other) {
			if (this != &other)
			{
				myPositionVector = other.myPositionVector;
				myRotationVector = other.myRotationVector;
				myPosition = other.myPosition;
				myRotation = other.myRotation;
				myViewMatrix = other.myViewMatrix;
				myProjectionMatrix = other.myProjectionMatrix;


				myXMVectorForward = other.myXMVectorForward;
				myXMVectorRight = other.myXMVectorRight;
				myXMVectorLeft = other.myXMVectorLeft;
				myXMVectorBack = other.myXMVectorBack;

				myNearZ = other.myNearZ;
				myFarZ = other.myFarZ;

			}

			return *this;
		}

		void SetProjectionValues(float aFovInDegrees, float anAspectRatio, float aNearZ, float aFarZ);
		void SetOrthographicProjection(float aWidth, float aHeight, float aNearZ, float aFarZ);
		const XMMATRIX& GetViewMatrix() const;
		const XMMATRIX& GetProjectionMatrix() const;

		const XMVECTOR& GetPositionXMVector() const;
		const XMFLOAT3& GetPositionXMFloat3() const;
		const mi::Vector3f GetPosition() const;


		const XMVECTOR& GetRotationXMVector() const;
		const XMFLOAT3& GetRotationXMFloat3() const;
		const mi::Vector3f GetRotation() const;

		void SetXMVectorPosition(const XMVECTOR& aPos);
		void SetPosition(float x, float y, float z);
		void SetPosition(mi::Vector3f aVector3f);

		void AdjustXMVectorPosition(const XMVECTOR& aPos);
		void AdjustPosition(float x, float y, float z);
		void AdjustPosition(mi::Vector3f aVector3f);

		void SetXMVectorRotation(const XMVECTOR& aRot);
		void SetRotation(float x, float y, float z);
		void SetRotation(mi::Vector3f aVector3f);

		void AdjustXMVectorRotation(const XMVECTOR& aRot);
		void AdjustRotation(float x, float y, float z);
		void AdjustRotation(mi::Vector3f aVector3f);

		void SetLookAtPos(XMFLOAT3 aLookAtPos);
		const XMVECTOR& GetForwardXMVector() const;
		const XMVECTOR& GetRightXMVector() const;
		const XMVECTOR& GetLeftXMVector() const;
		const XMVECTOR& GetBackXMVector() const;


		const Vector3f GetForward() const;
		const Vector3f GetRight() const;
		const Vector3f GetLeft() const;
		const Vector3f GetBack() const;

		float GetNearZ() const;
		float GetFarZ() const;

		Ray ScreenPointToRay(const Vector2f aPoint) const;

		void SetAsActiveCamera();
	private:
		void UpdateViewMatrix();
		XMVECTOR myPositionVector;
		XMVECTOR myRotationVector;
		XMFLOAT3 myPosition;
		XMFLOAT3 myRotation;
		XMMATRIX myViewMatrix;
		XMMATRIX myProjectionMatrix;

		const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		const XMVECTOR DEFAULT_BACK_VECTOR = XMVectorSet(0.f, 0.f, -1.f, 0.f);
		const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.f, 0.f, 0.f, 0.f);

		XMVECTOR myXMVectorForward;
		XMVECTOR myXMVectorRight;
		XMVECTOR myXMVectorLeft;
		XMVECTOR myXMVectorBack;
		XMVECTOR myXMVectorUp;

		float myNearZ;
		float myFarZ;

		CameraPureData myData;
		friend class Graphics;
	};
}