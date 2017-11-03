
#include "D3D11App.h"
#include "D3D11Math.h"
#include "D3D11Camera.h"

struct Vertex;

class App : public D3D11App
{
public:

	App(HINSTANCE hInstance, int Width, int Height);
	virtual ~App() override;

public:

	virtual bool Init() override;
	virtual void OnResize() override;
	virtual void UpdateScene(FLOAT DeltaTime);
	virtual void DrawScene() override;

private:

	void BuildModel();
	void BuildShader();
	void BuildVertexLayout();

private:

	ID3D11Buffer*								m_VertexBuffer;
	ID3D11Buffer*								m_IndexBuffer;

	ID3DX11Effect*								m_FX;
	ID3DX11EffectTechnique*						m_Tech;

	ID3D11InputLayout*							m_InputLayout;
	D3D11Camera*								m_Camera;

	ID3DX11EffectMatrixVariable*				m_EffectWorld;
	ID3DX11EffectMatrixVariable*				m_EffectView;
	ID3DX11EffectMatrixVariable*				m_EffectProjection;

private:

	XMMATRIX				m_World;

	Vertex*					m_Vertices;
	UINT*					m_Indices;
	XMFLOAT4*				m_Colors;
	UINT					m_VertexCount;
	UINT					m_FacetCount;
	UINT					m_ColorCount;

};

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT4 Color;
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE PrevInstance,
	PSTR CmdLine, INT ShowCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	App theApp(hInstance, 800, 800);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}


App::App(HINSTANCE hInstance, int Width, int Height) :
	D3D11App(hInstance), 
	m_VertexBuffer(nullptr),
	m_IndexBuffer(nullptr),
	m_FX(nullptr),
	m_Tech(nullptr),
	m_InputLayout(nullptr)
{
	m_ClientWidth = Width;
	m_ClientHeight = Height;
	m_MainWndCaption = L"DirectX11 App";
	m_Camera = new D3D11OrthographicCamera();
}

App::~App()
{
	ReleaseCOM(m_VertexBuffer);
	ReleaseCOM(m_IndexBuffer);
	ReleaseCOM(m_FX);
	ReleaseCOM(m_InputLayout);
	SafeDelete(m_Camera);
	SafeDeleteArray(m_Vertices);
	SafeDeleteArray(m_Indices);
	SafeDeleteArray(m_Colors);
}

bool App::Init()
{
	if (!D3D11App::Init())
		return false;

	//TODO : Add code to init camera and world matrix of your model here

	BuildModel();
	BuildShader();
	BuildVertexLayout();

	return true;
}

void App::OnResize()
{
	D3D11App::OnResize();
	
	
}

void App::UpdateScene(FLOAT DeltaTime)
{

}

void App::DrawScene()
{
	m_D3D11ImmediateContext->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const FLOAT*>(&Colors::Black));
	m_D3D11ImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_D3D11ImmediateContext->IASetInputLayout(m_InputLayout);
	m_D3D11ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT Stride = sizeof(Vertex);
	UINT Offset = 0;
	m_D3D11ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &Stride, &Offset);
	m_D3D11ImmediateContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_Camera->UpdateViewMatrix();
	m_Camera->UpdateViewMatrix();
	m_EffectWorld->SetMatrix(reinterpret_cast<FLOAT*>(&m_World));
	m_EffectView->SetMatrix(reinterpret_cast<FLOAT*>(&m_Camera->View()));
	m_EffectProjection->SetMatrix(reinterpret_cast<FLOAT*>(&m_Camera->Proj()));

	D3DX11_TECHNIQUE_DESC TechDesc;
	m_Tech->GetDesc(&TechDesc);
	for (UINT p = 0; p < TechDesc.Passes; ++p)
	{
		m_Tech->GetPassByIndex(p)->Apply(0, m_D3D11ImmediateContext);
		m_D3D11ImmediateContext->DrawIndexed(m_FacetCount * 3, 0, 0);
	}

	HR(m_SwapChain->Present(0, 0));
}


void App::BuildModel()
{
	
}

void App::BuildShader()
{
	DWORD ShaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
	ShaderFlags |= D3DCOMPILE_DEBUG;
	ShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* CompiledShader = 0;
	ID3D10Blob* CompilationMsgs = 0;
	D3DX11CompileFromFile(L"Shader.fx", 0, 0, 0, "fx_5_0", ShaderFlags,
		0, 0, &CompiledShader, &CompilationMsgs, 0);

	if (CompilationMsgs != nullptr)
	{
		MessageBoxA(0, reinterpret_cast<char*>(CompilationMsgs->GetBufferPointer()), 0, 0);
		ReleaseCOM(CompilationMsgs);
	}

	HR(D3DX11CreateEffectFromMemory(CompiledShader->GetBufferPointer(), CompiledShader->GetBufferSize(),
		0, m_D3D11Device, &m_FX));

	ReleaseCOM(CompiledShader);

	m_Tech = m_FX->GetTechniqueByName("Technique");

	ID3DX11EffectConstantBuffer* MatrixBuffer = m_FX->GetConstantBufferByName("ConstantBufferMatrix");

	m_EffectWorld = MatrixBuffer->GetMemberByName("World")->AsMatrix();
	m_EffectView = MatrixBuffer->GetMemberByName("View")->AsMatrix();
	m_EffectProjection = MatrixBuffer->GetMemberByName("Projection")->AsMatrix();
}

void App::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC Layouts[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT NumElements = ARRAYSIZE(Layouts);

	D3DX11_PASS_DESC PassDesc;
	m_Tech->GetPassByIndex(0)->GetDesc(&PassDesc);

	HR(m_D3D11Device->CreateInputLayout(Layouts, NumElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &m_InputLayout));
}