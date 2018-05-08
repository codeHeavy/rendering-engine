#include "Camera.h"
#include <Windows.h>
#include <algorithm>

XMFLOAT3 Camera::GetPosition()
{
	return position;
}

XMFLOAT3 Camera::GetDirection()
{
	XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 0);
	auto rotQuaternion = XMQuaternionRotationRollPitchYaw(rotationX, rotationY, 0);
	dir = XMVector3Rotate(dir, rotQuaternion);
	XMStoreFloat3(&tdirection, dir);
	return tdirection;
}

void Camera::RotateX(float x)
{
	rotationX += x;
	//Clamp to [90, -90] degrees.
	//if (rotationX > XM_PI/2) rotationX = XM_PI / 2;
	//if (rotationX < -XM_PI/2) rotationX = -XM_PI / 2;
}

void Camera::RotateY(float y)
{
	rotationY += y;
	//Clamp to [90, -90] degrees.
	//if (rotationY > XM_PI / 2) rotationY = XM_PI / 2;
	//if (rotationY < -XM_PI / 2) rotationY = -XM_PI / 2;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	auto rotQuaternion = XMQuaternionRotationRollPitchYaw(rotationX, rotationY, 0);
	XMVECTOR pos = XMVectorSet(position.x, position.y, position.z, 0);
	XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	dir = XMVector3Rotate(dir, rotQuaternion);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::SetProjectionMatrix(float aspectRatio)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * XM_PI,
		aspectRatio,
		0.1f,
		1000.0f);
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P));
}

void Camera::RenderReflectionMatrix(float height)
{
	XMFLOAT3 up, reflPosition, lookAt;
	float radians;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	// For planar reflection invert the Y position of the camera.
	reflPosition.x = position.x;
	reflPosition.y = -position.y + (height * 2.0f);
	reflPosition.z = position.z;

	// Calculate the rotation in radians.
	radians = rotationY * 0.0174532925f;

	// Setup where the camera is looking.
	lookAt.x = sinf(radians) + position.x;
	lookAt.y = reflPosition.y;
	lookAt.z = cosf(radians) + position.z;

	// Create the view matrix from the three vectors.
	XMStoreFloat4x4(&reflectionMatrix,XMMatrixLookAtLH(XMLoadFloat3(&reflPosition), XMLoadFloat3(&lookAt), XMLoadFloat3(&up)));
	XMStoreFloat4x4(&reflectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&reflectionMatrix)));
	return;
}

XMFLOAT4X4 Camera::GetReflectionMatrix()
{
	return reflectionMatrix;
}

void Camera::Update(float deltaTime)
{
	float speed = 10.f;
	XMVECTOR pos = XMVectorSet(position.x, position.y, position.z, 0);
	XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 0);
	auto rotQuaternion = XMQuaternionRotationRollPitchYaw(rotationX, rotationY, 0);
	dir = XMVector3Rotate(dir, rotQuaternion);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0); // Y is up!

	if (GetAsyncKeyState('W') & 0x8000)
	{
		pos = pos + dir * speed * deltaTime;;
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		pos = pos - dir * speed * deltaTime;;
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		auto leftDir = XMVector3Cross(dir, up);
		pos = pos + leftDir * speed * deltaTime;;
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		auto rightDir = XMVector3Cross(-dir, up);
		pos = pos + rightDir * speed * deltaTime;;
	}

	/*if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		pos = pos + XMVectorSet(0, speed * deltaTime, 0, 0);
	}

	if (GetAsyncKeyState('X') & 0x8000)
	{
		pos = pos + XMVectorSet(0, -speed * deltaTime, 0, 0);
	}*/

	XMStoreFloat3(&position, pos);
}

Camera::Camera(float aspectRatio)
{
	position = XMFLOAT3(0.f, 3.f, -15.f);
	direction = XMFLOAT3(0.f, 0.f, 1.f);
	rotationX = rotationY = 0.f;

	XMVECTOR pos = XMVectorSet(0, 0, -25, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * XM_PI,		// Field of View Angle
		aspectRatio,		// Aspect ratio
		0.1f,						// Near clip plane distance
		300.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P));
}


Camera::~Camera()
{
}

XMFLOAT3 Camera::GetUp()
{
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	auto rotQuaternion = XMQuaternionRotationRollPitchYaw(rotationX, rotationY, 0);
	up = XMVector3Rotate(up, rotQuaternion);
	XMStoreFloat3(&currentUp, up);
	return currentUp;
}