#pragma once
#include "framework.h"

class Core;
class Shader;
class Object;
class DDSTexture;
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
private:
	void	BuildRootSignature();
	void	BuildShader();
	void	BuildTextures();
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
private:
	Core*								m_pCore = NULL;
	ID3D12Device*						m_pDevice = NULL;
	ID3D12GraphicsCommandList*			m_pCmdLst = NULL;
	unordered_map<RENDER_TYPE, Shader*>	m_mapShaders;
	ComPtr<ID3D12RootSignature>			m_ptrRootSignature;

	list<Object*>						m_lstObjects[RENDER_TYPE::RENDER_END];
public:
	DDSTexture* m_pTexture;
};

