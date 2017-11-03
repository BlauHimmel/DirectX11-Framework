#pragma once

#ifndef __D3D11_CAMERA_H__
#define __D3D11_CAMERA_H__

#include <Windows.h>
#include <xnamath.h>
#include <cmath>

enum class D3D11_CAMERA_TYPE
{
	D3D11_CAMERA_TYPE_ORTHOGRAPHICS,
	D3D11_CAMERA_TYPE_PERSPECTIVE
};

class D3D11Camera
{

public:

	D3D11Camera();

public:

	XMVECTOR GetPositionXM() const;
	XMFLOAT3 GetPosition() const;
	void SetPosition(float X, float Y, float Z);
	void SetPosition(const XMFLOAT3& Position);

	XMVECTOR GetRightXM() const;
	XMFLOAT3 GetRight() const;
	XMVECTOR GetUpXM() const;
	XMFLOAT3 GetUp() const;
	XMVECTOR GetLookXM() const;
	XMFLOAT3 GetLook() const;

	float GetAspect() const;

	float GetNearZ() const;
	float GetFarZ() const;

public:

	virtual D3D11_CAMERA_TYPE GetCameraType() const = 0;

public:

	void LookAt(FXMVECTOR Position, FXMVECTOR Target, FXMVECTOR WorldUp);
	void LookAt(const XMFLOAT3& Position, const XMFLOAT3& Target, const XMFLOAT3& WorldUp);

public:

	XMMATRIX View() const;
	XMMATRIX Proj() const;
	XMMATRIX ViewProj() const;

public:

	void Strafe(float Distance);
	void Walk(float Distance);

	void Pitch(float Angle);
	void RotateY(float Angle);

public:

	void UpdateViewMatrix();

protected:

	XMFLOAT3 m_Position;
	XMFLOAT3 m_Right;
	XMFLOAT3 m_Up;
	XMFLOAT3 m_Look;

	float m_NearZ;
	float m_FarZ;
	float m_Aspect;

	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;
};

class D3D11OrthographicCamera : public D3D11Camera
{

public:

	D3D11OrthographicCamera();

public:

	void SetLens(int Width, int Height, float Size, float NearZ, float FarZ);

public:

	float GetWidth() const;
	float GetHeigth() const;
	float GetViewWidth() const;
	float GetViewHeigth() const;

public:

	virtual D3D11_CAMERA_TYPE GetCameraType() const override;

protected:

	float m_Size;
	float m_ClientWidth;
	float m_ClientHeight;
	float m_ViewWidth;
	float m_ViewHeight;

};

class D3D11PerspectiveCamera : public D3D11Camera
{

public:

	D3D11PerspectiveCamera();

public:
	
	void SetLens(float FovY, float Aspect, float NearZ, float FarZ);

public:

	float GetFovY() const;
	float GetFovX() const;

	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

public:

	virtual D3D11_CAMERA_TYPE GetCameraType() const override;

protected:

	float m_FovY;
	float m_NearWindowHeight;
	float m_FarWindowHeight;

};

#endif