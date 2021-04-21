#pragma once
#include "framework.h"

class DDSTexture
{
public:
	DDSTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, 
		string strName, wstring wstrFilePath, bool Texture3D = false, int RootParameterIdx = 4);
	~DDSTexture() {}
public:
	void	PreRender(ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap);
	void	PreRender(ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, int Idx);
public:
	int									m_RootParameterIdx;
	string								m_strName;
	wstring								m_wstrFilePath;
	ComPtr<ID3D12Resource>				m_ptrResource;
	ComPtr<ID3D12Resource>				m_ptrUploadHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_hDescriptor;
};

