#pragma once
#include "framework.h"

class Core;
class Shader;
class Object;
class DDSTexture;
class Camera;
class TextureMgr;
class RenderTarget;
class RenderTargetMgr;
class CLight;
class Skybox;
class InstanceInfo;
class LightGeo;

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
	void	DrawObject(InstanceInfo* _inst, list<Object*> _Objects);

	void	Render_Albedo(const float& fTimeDelta);
	void	Render_Shadow(const float& fTimeDelta);
public:
	void	PushObject(RENDER_TYPE eType, Object* pObject);
public:
	ID3D12DescriptorHeap*		GetHeap() { return m_ptrDescriptorHeap.Get(); }
	ID3D12RootSignature*		GetRootSignature() { return m_ptrRootSignature.Get(); }
	void						SetCamera(Camera* camera) { m_pCamera = camera; }
	void						CreateConstantBufferView(D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc);
	D3D12_CPU_DESCRIPTOR_HANDLE	CreateShaderResourceView(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc);
	D3D12_CPU_DESCRIPTOR_HANDLE	CreateUnorderedAccessView(ID3D12Resource* resource, D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc);
private:
	void	BuildRootSignature();
	void	BuildDescrpitorHeap();
	void	BuildShader();
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

	void	DebugKeyInput();
	string	DebugInput = "Post";
private:
	Core*								m_pCore = NULL;
	ID3D12Device*						m_pDevice = NULL;
	ID3D12GraphicsCommandList*			m_pCmdLst = NULL;
	unordered_map<RENDER_TYPE, Shader*>	m_mapShaders;
	unordered_map<RENDER_TYPE, Shader*> m_mapShadersForShadow;
	ComPtr<ID3D12RootSignature>			m_ptrRootSignature;
	ComPtr<ID3D12DescriptorHeap>		m_ptrDescriptorHeap;

	list<Object*>						m_lstObjects[RENDER_TYPE::RENDER_END];
	map<string, UINT>					m_InstanceCheck;
	map<string, UINT>					m_InstanceCheckForShadow;

	int									m_iCountView = 0;

	Camera*								m_pCamera;
	CLight*								m_pLight;

	TextureMgr*							m_pTextureMgr;
	RenderTargetMgr*					m_pRTMgr;
	CRITICAL_SECTION					m_Crt;
	
	LightGeo*							m_pBlendGeo;
};

