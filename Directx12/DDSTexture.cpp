#include "DDSTexture.h"
#include "Core.h"

DDSTexture::DDSTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, string strName, wstring wstrFilePath)
{
	Core::GetInstance()->CmdLstReset();

	//Create Resource
	m_strName = strName;
	m_wstrFilePath = wstrFilePath;
	CreateDDSTextureFromFile12(device, cmdLst, wstrFilePath.c_str(), m_ptrResource, m_ptrUploadHeap);
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();


	//Create Shader Resource View
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(heap->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_ptrResource.Get()->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = m_ptrResource.Get()->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	device->CreateShaderResourceView(m_ptrResource.Get(), &srvDesc, hDescriptor);
}

void DDSTexture::PreRender(ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(heap->GetGPUDescriptorHandleForHeapStart());

	cmdLst->SetGraphicsRootDescriptorTable(3, tex);
}
