#pragma once
#include "framework.h"

class Shader
{
public:
	Shader() {}
	~Shader() {}
public:
	HRESULT BuildShadersAndInputLayout(const TCHAR* vsName, const char* vsFunc, const TCHAR* psName,
										const char* psFunc, vector<D3D12_INPUT_ELEMENT_DESC> InputLayout);
	HRESULT BuildPipelineState(ID3D12Device* device, ID3D12RootSignature* RootSignature, int numRt, 
		bool ClockWise = true, bool DepthStencil = true, bool CullNone = false, int depthidx = 99);
	HRESULT BuildPipelineStateBlendOp(ID3D12Device* device, ID3D12RootSignature* RootSignature, int numRt,
		bool ClockWise = true, bool DepthStencil = true, bool CullNone = false, int depthidx = 99);

public:
	void	PreRender(ID3D12GraphicsCommandList* cmdLst);
private:
	ComPtr<ID3DBlob>					m_vsByteCode;
	ComPtr<ID3DBlob>					m_psByteCode;
	ComPtr<ID3D12PipelineState>			m_PipelineState;
	vector<D3D12_INPUT_ELEMENT_DESC>	m_InputLayout;

	const char* m_vsFunc = NULL;
	const char* m_psFunc = NULL;
};

