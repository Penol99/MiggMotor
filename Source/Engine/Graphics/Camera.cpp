#include "engine_pch.h"
#include "Camera.h"
#include <PropertyDefines.h>
#include <Engine.h>

namespace mi
{

	Camera::Camera()
		:myPosition(XMFLOAT3(0.0f, 0.0f, 0.0f)),
		myRotation(XMFLOAT3(0.0f, 0.0f, 0.0f))

	{
		myPositionVector = XMLoadFloat3(&myPosition);
		myRotationVector = XMLoadFloat3(&myPosition);
		UpdateViewMatrix();
	}

	Camera::~Camera()
	{
	}

	void Camera::SetProjectionValues(float aFovInDegrees, float anAspectRatio, float aNearZ, float aFarZ)
	{
		float fovRadians = (aFovInDegrees / 360.f) * XM_2PI;
		myProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, anAspectRatio, aNearZ, aFarZ);
		myNearZ = aNearZ;
		myFarZ = aFarZ;
	}

	void Camera::SetOrthographicProjection(float aWidth, float aHeight, float aNearZ, float aFarZ)
	{

		myProjectionMatrix = XMMatrixOrthographicLH(aWidth, aHeight, aNearZ, aFarZ);
		myNearZ = aNearZ;
		myFarZ = aFarZ;
	}


	const XMMATRIX& Camera::GetViewMatrix() const
	{
		return myViewMatrix;
	}

	const XMMATRIX& Camera::GetProjectionMatrix() const
	{
		return myProjectionMatrix;
	}

	const XMVECTOR& Camera::GetPositionXMVector() const
	{
		return myPositionVector;
	}

	const XMFLOAT3& Camera::GetPositionXMFloat3() const
	{
		return myPosition;
	}

	const mi::Vector3f Camera::GetPosition() const
	{
		const mi::Vector3f pos = { myPosition.x ,myPosition.y ,myPosition.z };

		return pos;
	}

	const XMVECTOR& Camera::GetRotationXMVector() const
	{
		return myRotationVector;
	}

	const XMFLOAT3& Camera::GetRotationXMFloat3() const
	{
		return myRotation;
	}

	const mi::Vector3f Camera::GetRotation() const
	{
		const mi::Vector3f rot = { myRotation.x ,myRotation.y ,myRotation.z };

		return rot;
	}

	void Camera::SetXMVectorPosition(const XMVECTOR& aPos)
	{
		XMStoreFloat3(&myPosition, aPos);
		myPositionVector = aPos;
		UpdateViewMatrix();
	}

	void Camera::SetPosition(float x, float y, float z)
	{
		myPosition = XMFLOAT3(x, y, z);
		myPositionVector = XMLoadFloat3(&myPosition);
		UpdateViewMatrix();
	}

	void Camera::SetPosition(mi::Vector3f aVector3f)
	{
		myPosition = XMFLOAT3(aVector3f.x, aVector3f.y, aVector3f.z);
		myPositionVector = XMLoadFloat3(&myPosition);
		UpdateViewMatrix();
	}

	void Camera::AdjustXMVectorPosition(const XMVECTOR& aPos)
	{
		myPositionVector = DirectX::XMVectorAdd(myPositionVector, aPos);
		//myPositionVector += aPos;
		XMStoreFloat3(&myPosition, myPositionVector);
		UpdateViewMatrix();
	}

	void Camera::AdjustPosition(float x, float y, float z)
	{
		myPosition.x += x;
		myPosition.y += y;
		myPosition.z += z;
		myPositionVector = XMLoadFloat3(&myPosition);
		UpdateViewMatrix();
	}

	void Camera::AdjustPosition(mi::Vector3f aVector3f)
	{
		myPosition.x += aVector3f.x;
		myPosition.y += aVector3f.y;
		myPosition.z += aVector3f.z;
		myPositionVector = XMLoadFloat3(&myPosition);
		UpdateViewMatrix();
	}

	void Camera::SetXMVectorRotation(const XMVECTOR& aRot)
	{
		XMStoreFloat3(&myRotation, aRot);
		myRotationVector = aRot;
		UpdateViewMatrix();
	}

	void Camera::SetRotation(float x, float y, float z)
	{
		myRotation = XMFLOAT3(x, y, z);
		myRotationVector = XMLoadFloat3(&myRotation);
		UpdateViewMatrix();
	}

	void Camera::SetRotation(Vector3f aVector3f)
	{
		myRotation = XMFLOAT3(aVector3f.x, aVector3f.y, aVector3f.z);
		myRotationVector = XMLoadFloat3(&myRotation);
		UpdateViewMatrix();
	}

	void Camera::AdjustXMVectorRotation(const DirectX::XMVECTOR& aRot)
	{
		
		myPositionVector = DirectX::XMVectorAdd(myPositionVector, aRot);
		XMStoreFloat3(&myRotation, myRotationVector);
		UpdateViewMatrix();
	}

	void Camera::AdjustRotation(float x, float y, float z)
	{
		myRotation.x += x;
		myRotation.y += y;
		myRotation.z += z;
		myRotationVector = XMLoadFloat3(&myRotation);
		UpdateViewMatrix();
	}

	void Camera::AdjustRotation(Vector3f aVector3f)
	{
		myRotation.x += aVector3f.x;
		myRotation.y += aVector3f.y;
		myRotation.z += aVector3f.z;
		myRotationVector = XMLoadFloat3(&myRotation);
		UpdateViewMatrix();
	}

	void Camera::SetLookAtPos(XMFLOAT3 aLookAtPos)
	{
		if (myPosition.x == aLookAtPos.x && myPosition.y == aLookAtPos.y && myPosition.z == aLookAtPos.z)
		{
			return;
		}

		aLookAtPos.x = myPosition.x - aLookAtPos.x;
		aLookAtPos.y = myPosition.y - aLookAtPos.y;
		aLookAtPos.z = myPosition.z - aLookAtPos.z;

		double pitch = 0.f;
		double yaw = 0.f;
		if (aLookAtPos.y != 0.f)
		{
			const double distance = sqrt(aLookAtPos.x * aLookAtPos.x + aLookAtPos.z * aLookAtPos.z);
			pitch = atan(aLookAtPos.y / distance);
		}
		if (aLookAtPos.x != 0.f)
		{
			yaw = atan(aLookAtPos.x / aLookAtPos.z);
		}
		if (aLookAtPos.z > 0)
		{
			yaw += XM_PI;
		}
		SetRotation((float)pitch, (float)yaw, 0.0f);
	}

	const XMVECTOR& Camera::GetForwardXMVector() const
	{
		return myXMVectorForward;
	}

	const XMVECTOR& Camera::GetRightXMVector() const
	{
		return myXMVectorRight;
	}

	const XMVECTOR& Camera::GetLeftXMVector() const
	{
		return myXMVectorLeft;
	}

	const XMVECTOR& Camera::GetBackXMVector() const
	{
		return myXMVectorBack;
	}

	const Vector3f Camera::GetForward() const
	{
		XMFLOAT3 vec;
		XMStoreFloat3(&vec, myXMVectorForward);
		return { vec.x,vec.y,vec.z };
	}

	const Vector3f Camera::GetRight() const
	{
		XMFLOAT3 vec;
		XMStoreFloat3(&vec, myXMVectorRight);
		return { vec.x,vec.y,vec.z };
	}

	const Vector3f Camera::GetLeft() const
	{
		XMFLOAT3 vec;
		XMStoreFloat3(&vec, myXMVectorLeft);
		return { vec.x,vec.y,vec.z };
	}

	const Vector3f Camera::GetBack() const
	{
		XMFLOAT3 vec;
		XMStoreFloat3(&vec, myXMVectorBack);
		return { vec.x,vec.y,vec.z };
	}

	float Camera::GetNearZ() const
	{
		return myNearZ;
	}

	float Camera::GetFarZ() const
	{
		return myFarZ;
	}

	Ray Camera::ScreenPointToRay(const Vector2f aPoint) const
	{
		float x = (2.0f * aPoint.x) / Engine::GetInstance()->GetRenderResolution().x - 1.0f;
		float y = 1.0f - (2.0f * aPoint.y) / Engine::GetInstance()->GetRenderResolution().y;

		DirectX::XMMATRIX invViewProjMatrix = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixMultiply(myViewMatrix, myProjectionMatrix));

		DirectX::XMVECTOR origin = DirectX::XMVectorSet(x, y, 0.0f, 1.0f);
		DirectX::XMVECTOR destination = DirectX::XMVectorSet(x, y, 1.0f, 1.0f);

		// Transform to world space
		origin = DirectX::XMVector3TransformCoord(origin, invViewProjMatrix);
		destination = DirectX::XMVector3TransformCoord(destination, invViewProjMatrix);

		// Create and return the ray
		return Ray(origin, DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(destination, origin)));
	}

	void Camera::SetAsActiveCamera()
	{
		Engine::GetInstance()->SetActiveCamera(this);
	}

	void Camera::UpdateViewMatrix()
	{
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(myRotation.x, myRotation.y, myRotation.z);
		XMVECTOR camTarget = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotationMatrix);
		camTarget = XMVectorAdd(camTarget, myPositionVector);
		//camTarget += myPositionVector;
		myXMVectorUp = XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);
		myViewMatrix = XMMatrixLookAtLH(myPositionVector, camTarget, myXMVectorUp);

		// Set the Camera-to-World matrix

		XMMATRIX vectorRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, myRotation.y, 0.0f);
		myXMVectorForward = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vectorRotationMatrix);
		myXMVectorBack = XMVector3TransformCoord(DEFAULT_BACK_VECTOR, vectorRotationMatrix);
		myXMVectorRight = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vectorRotationMatrix);
		myXMVectorLeft = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vectorRotationMatrix);

	}
}
