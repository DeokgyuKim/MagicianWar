#pragma once
#include "framework.h"

class DDSTexture
{
public:
	DDSTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, string strName, wstring wstrFilePath);
	~DDSTexture() {}
public:
	void	PreRender(ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap);
public:
	string								m_strName;
	wstring								m_wstrFilePath;
	ComPtr<ID3D12Resource>				m_ptrResource;
	ComPtr<ID3D12Resource>				m_ptrUploadHeap;
};

