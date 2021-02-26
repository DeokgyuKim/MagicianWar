#include "DDSTexture.h"
#include "Core.h"

DDSTexture::DDSTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, string strName, wstring wstrFilePath)
{
	Core::GetInstance()->CmdLstReset();

	//Create Resource
	m_strName = strName;
	m_wstrFilePath = wstrFilePath;
	CreateDDSTextureFromFile12(device, cmdLst, wstrFilePath.c_str(), m_ptrResource, m_ptrUploadHeap);
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	//Create SRV Heap
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 2;///////////////////////
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_ptrSRVHeap));

	//Create Shader Resource View
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_ptrSRVHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_ptrResource.Get()->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = m_ptrResource.Get()->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	device->CreateShaderResourceView(m_ptrResource.Get(), &srvDesc, hDescriptor);
}

void DDSTexture::PreRender(ID3D12GraphicsCommandList* cmdLst)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_ptrSRVHeap.Get() };
	cmdLst->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(m_ptrSRVHeap->GetGPUDescriptorHandleForHeapStart());

	cmdLst->SetGraphicsRootDescriptorTable(1, tex);
}
