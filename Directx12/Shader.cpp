#include "Shader.h"

HRESULT Shader::BuildShadersAndInputLayout(const TCHAR* vsName, const char* vsFunc, const TCHAR* psName,
											const char* psFunc,	vector<D3D12_INPUT_ELEMENT_DESC> InputLayout)
{
	HRESULT hr = S_OK;
	
	m_vsFunc = vsFunc;
	m_psFunc = psFunc;

	if (vsFunc != NULL)
		m_vsByteCode = d3dUtil::CompileShader(vsName, nullptr, vsFunc, "vs_5_1");
	if (psFunc != NULL)
		m_psByteCode = d3dUtil::CompileShader(psName, nullptr, psFunc, "ps_5_1");

	m_InputLayout = InputLayout;
	return S_OK;
}

HRESULT Shader::BuildPipelineState(ID3D12Device* device, ID3D12RootSignature* RootSignature, int numRt,
	bool ClockWise, bool DepthStencil, bool CullNone, int depthidx)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
	psoDesc.pRootSignature = RootSignature;
	if (m_vsFunc != NULL)
	{
		psoDesc.VS =
		{
			reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
			m_vsByteCode->GetBufferSize()
		};
	}
	if (m_psFunc != NULL)
	{
		psoDesc.PS =
		{
			reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
			m_psByteCode->GetBufferSize()
		};
	}

	if (ClockWise && !CullNone)
	{
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	}
	else
	{
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
		psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthClipEnable = TRUE;
		psoDesc.RasterizerState.MultisampleEnable = FALSE;
		psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
		psoDesc.RasterizerState.ForcedSampleCount = 0;
		psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	if(CullNone)
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.BlendState.AlphaToCoverageEnable = true;
	psoDesc.BlendState.IndependentBlendEnable = false;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
	psoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	if (DepthStencil)
	{
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		psoDesc.DepthStencilState.StencilEnable = FALSE;
	}
	else
	{
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
	}

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = numRt;
	for (int i = 0; i < numRt; ++i)
	{
		psoDesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (depthidx <= i)
			psoDesc.RTVFormats[i] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	psoDesc.SampleDesc.Count = 1;

	HRESULT a = (device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

	return S_OK;
}

HRESULT Shader::BuildPipelineStateBlendOp(ID3D12Device* device, ID3D12RootSignature* RootSignature, int numRt, bool ClockWise, bool DepthStencil, bool CullNone, int depthidx)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
	psoDesc.pRootSignature = RootSignature;
	if (m_vsFunc != NULL)
	{
		psoDesc.VS =
		{
			reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
			m_vsByteCode->GetBufferSize()
		};
	}
	if (m_psFunc != NULL)
	{
		psoDesc.PS =
		{
			reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
			m_psByteCode->GetBufferSize()
		};
	}

	if (ClockWise && !CullNone)
	{
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	}
	else
	{
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
		psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthClipEnable = TRUE;
		psoDesc.RasterizerState.MultisampleEnable = FALSE;
		psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
		psoDesc.RasterizerState.ForcedSampleCount = 0;
		psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	if (CullNone)
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.BlendState.AlphaToCoverageEnable = false;
	psoDesc.BlendState.IndependentBlendEnable = true;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
	psoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	psoDesc.BlendState.RenderTarget[1].BlendEnable = false;
	psoDesc.BlendState.RenderTarget[1].LogicOpEnable = false;
	psoDesc.BlendState.RenderTarget[1].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[1].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[1].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[1].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[1].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[1].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[1].LogicOp = D3D12_LOGIC_OP_NOOP;
	psoDesc.BlendState.RenderTarget[1].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	if (DepthStencil)
	{
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		psoDesc.DepthStencilState.StencilEnable = FALSE;
	}
	else
	{
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
	}

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = numRt;
	for (int i = 0; i < numRt; ++i)
	{
		psoDesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (depthidx <= i)
			psoDesc.RTVFormats[i] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	psoDesc.SampleDesc.Count = 1;

	HRESULT a = (device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

	return S_OK;

}

void Shader::PreRender(ID3D12GraphicsCommandList* cmdLst)
{
	cmdLst->SetPipelineState(m_PipelineState.Get());
}
