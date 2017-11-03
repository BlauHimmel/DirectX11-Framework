#include "D3D11Camera.h"

D3D11Camera::D3D11Camera(): 
	m_Position(0.0f, 0.0f, 0.0f),
	m_Right(1.0f, 0.0f, 0.0f),
	m_Up(0.0f, 1.0f, 0.0f),
	m_Look(0.0f, 0.0f, 1.0f)
{
	
}

XMVECTOR D3D11Camera::GetPositionXM()const
{
	return XMLoadFloat3(&m_Position);
}

XMFLOAT3 D3D11Camera::GetPosition()const
{
	return m_Position;
}

void D3D11Camera::SetPosition(float X, float Y, float Z)
{
	m_Position = XMFLOAT3(X, Y, Z);
}

void D3D11Camera::SetPosition(const XMFLOAT3& Position)
{
	m_Position = Position;
}

XMVECTOR D3D11Camera::GetRightXM() const
{
	return XMLoadFloat3(&m_Right);
}

XMFLOAT3 D3D11Camera::GetRight() const
{
	return m_Right;
}

XMVECTOR D3D11Camera::GetUpXM() const
{
	return XMLoadFloat3(&m_Up);
}

XMFLOAT3 D3D11Camera::GetUp() const
{
	return m_Up;
}

XMVECTOR D3D11Camera::GetLookXM() const
{
	return XMLoadFloat3(&m_Look);
}

XMFLOAT3 D3D11Camera::GetLook() const
{
	return m_Look;
}

float D3D11Camera::GetNearZ() const
{
	return m_NearZ;
}

float D3D11Camera::GetFarZ() const
{
	return m_FarZ;
}

float D3D11Camera::GetAspect() const
{
	return m_Aspect;
}

void D3D11Camera::LookAt(FXMVECTOR Position, FXMVECTOR Target, FXMVECTOR WorldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(Target, Position));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(WorldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_Position, Position);
	XMStoreFloat3(&m_Look, L);
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
}

void D3D11Camera::LookAt(const XMFLOAT3& Position, const XMFLOAT3& Target, const XMFLOAT3& WorldUp)
{
	XMVECTOR P = XMLoadFloat3(&Position);
	XMVECTOR T = XMLoadFloat3(&Target);
	XMVECTOR U = XMLoadFloat3(&WorldUp);

	LookAt(P, T, U);
}

XMMATRIX D3D11Camera::View()const
{
	return XMLoadFloat4x4(&m_View);
}

XMMATRIX D3D11Camera::Proj()const
{
	return XMLoadFloat4x4(&m_Proj);
}

XMMATRIX D3D11Camera::ViewProj()const
{
	return XMMatrixMultiply(View(), Proj());
}

void D3D11Camera::Strafe(float Distance)
{
	XMVECTOR S = XMVectorReplicate(Distance);
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR P = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(S, R, P));
}

void D3D11Camera::Walk(float Distance)
{
	XMVECTOR S = XMVectorReplicate(Distance);
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR P = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(S, L, P));
}

void D3D11Camera::Pitch(float Angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), Angle);

	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void D3D11Camera::RotateY(float Angle)
{
	XMMATRIX R = XMMatrixRotationY(Angle);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void D3D11Camera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR P = XMLoadFloat3(&m_Position);
	
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	R = XMVector3Cross(U, L);

	float X = -XMVectorGetX(XMVector3Dot(P, R));
	float Y = -XMVectorGetX(XMVector3Dot(P, U));
	float Z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);

	m_View(0, 0) = m_Right.x;
	m_View(1, 0) = m_Right.y;
	m_View(2, 0) = m_Right.z;
	m_View(3, 0) = X;

	m_View(0, 1) = m_Up.x;
	m_View(1, 1) = m_Up.y;
	m_View(2, 1) = m_Up.z;
	m_View(3, 1) = Y;

	m_View(0, 2) = m_Look.x;
	m_View(1, 2) = m_Look.y;
	m_View(2, 2) = m_Look.z;
	m_View(3, 2) = Z;

	m_View(0, 3) = 0.0f;
	m_View(1, 3) = 0.0f;
	m_View(2, 3) = 0.0f;
	m_View(3, 3) = 1.0f;
}

D3D11OrthographicCamera::D3D11OrthographicCamera() :
	D3D11Camera()
{
	SetLens(800.0f, 600.0f, 1.0f, 0.1f, 1000.0f);
}

void D3D11OrthographicCamera::SetLens(int Width, int Height, float Size, float NearZ, float FarZ)
{
	m_ClientWidth = static_cast<float>(Width);
	m_ClientHeight = static_cast<float>(Height);
	m_Size = Size;
	m_NearZ = NearZ;
	m_FarZ = FarZ;
	m_ViewWidth = m_ClientWidth / m_Size;
	m_ViewHeight = m_ClientHeight / m_Size;
	m_Aspect = m_ClientWidth / m_ClientHeight;

	XMMATRIX P = XMMatrixOrthographicLH(m_ViewWidth, m_ViewHeight, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_Proj, P);
}

float D3D11OrthographicCamera::GetWidth() const
{
	return m_ClientWidth;
}

float D3D11OrthographicCamera::GetHeigth() const
{
	return m_ClientHeight;
}

float D3D11OrthographicCamera::GetViewWidth() const
{
	return m_ViewWidth;
}

float D3D11OrthographicCamera::GetViewHeigth() const
{
	return m_ViewHeight;
}

D3D11_CAMERA_TYPE D3D11OrthographicCamera::GetCameraType() const
{
	return D3D11_CAMERA_TYPE::D3D11_CAMERA_TYPE_ORTHOGRAPHICS;
}

D3D11PerspectiveCamera::D3D11PerspectiveCamera() : 
	D3D11Camera()
{
	SetLens(XM_PIDIV4, 800.0f / 600.0f, 0.1f, 1000.0f);
}

void D3D11PerspectiveCamera::SetLens(float FovY, float Aspect, float NearZ, float FarZ)
{
	m_FovY = FovY;
	m_Aspect = Aspect;
	m_NearZ = NearZ;
	m_FarZ = FarZ;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_Proj, P);
}

float D3D11PerspectiveCamera::GetFovY() const
{
	return m_FovY;
}

float D3D11PerspectiveCamera::GetFovX() const
{
	float HalfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f*atan(HalfWidth / m_NearZ);
}

float D3D11PerspectiveCamera::GetNearWindowWidth() const
{
	return m_Aspect * m_NearWindowHeight;
}

float D3D11PerspectiveCamera::GetNearWindowHeight() const
{
	return m_NearWindowHeight;
}

float D3D11PerspectiveCamera::GetFarWindowWidth() const
{
	return m_Aspect * m_FarWindowHeight;
}

float D3D11PerspectiveCamera::GetFarWindowHeight() const
{
	return m_FarWindowHeight;
}

D3D11_CAMERA_TYPE D3D11PerspectiveCamera::GetCameraType() const
{
	return D3D11_CAMERA_TYPE::D3D11_CAMERA_TYPE_PERSPECTIVE;
}
