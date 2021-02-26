#pragma once
#include "framework.h"

class DDSTexture
{
public:
	DDSTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst,string strName, wstring wstrFilePath);
	~DDSTexture() {}
public:
	void	PreRender(ID3D12GraphicsCommandList* cmdLst);
public:
	string								m_strName;
	wstring								m_wstrFilePath;
	ComPtr<ID3D12Resource>				m_ptrResource;
	ComPtr<ID3D12Resource>				m_ptrUploadHeap;
	ComPtr<ID3D12DescriptorHeap>		m_ptrSRVHeap;
};

