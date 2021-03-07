#pragma once
#include "framework.h"

class Core;
class Shader;
class Object;
class DDSTexture;
class Camera;
class TextureMgr;

class Renderer
{
private:
	Renderer() {}
	Renderer(const Renderer& rhs) {}
	~Renderer() {}
public:
	static Renderer*	GetInstance();
	static void			DestroyInstance();
private:
	static Renderer*	m_pInstance;
public:
	void	InitRenderer(Core* pCore, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdLst);
	void	Render(const float& fTimeDelta);
public:
	void	PushObject(RENDER_TYPE eType, Object* pObject);
public:
	ID3D12DescriptorHeap*	GetHeap() { return m_ptrDescriptorHeap.Get(); }
	void					SetCamera(Camera* camera) { m_pCamera = camera; }
	void					CreateConstantBufferView(D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc);
private:
	void	BuildRootSignature();
	void	BuildDescrpitorHeap();
	void	BuildShader();
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
private:
	Core*								m_pCore = NULL;
	ID3D12Device*						m_pDevice = NULL;
	ID3D12GraphicsCommandList*			m_pCmdLst = NULL;
	unordered_map<RENDER_TYPE, Shader*>	m_mapShaders;
	ComPtr<ID3D12RootSignature>			m_ptrRootSignature;
	ComPtr<ID3D12DescriptorHeap>		m_ptrDescriptorHeap;

	list<Object*>						m_lstObjects[RENDER_TYPE::RENDER_END];

	int									m_iCountView = 0;

	Camera*								m_pCamera;

	TextureMgr*							m_pTextureMgr;
};

