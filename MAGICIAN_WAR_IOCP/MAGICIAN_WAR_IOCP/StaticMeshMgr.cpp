#include "StaticMeshMgr.h"

StaticMeshMgr* StaticMeshMgr::m_pInstance = NULL;

void StaticMeshMgr::Release()
{
	if (!m_mmapInfo.empty())
	{
		for (auto iter = m_mmapInfo.begin(); iter != m_mmapInfo.end(); ++iter)
			delete (*iter).first;
		m_mmapInfo.clear();
	}
}

HRESULT StaticMeshMgr::LoadMeshInfo(string strFilePath)
{
	if (!m_mmapInfo.empty())
	{
		for (auto iter = m_mmapInfo.begin(); iter != m_mmapInfo.end(); ++iter)
			delete (*iter).first;
		m_mmapInfo.clear();
	}

	ifstream readFile;
	readFile.open(strFilePath, ios_base::in);

	if (!readFile.is_open())
		return E_FAIL;

	while (!readFile.eof())
	{
		string* FileName = new string;
		getline(readFile, *FileName);

		if (*FileName == "")
			break;

		string str;
		TransformStruct st;

		getline(readFile, str);
		st.xmfPosition.x = stof(str);
		getline(readFile, str);
		st.xmfPosition.y = stof(str);
		getline(readFile, str);
		st.xmfPosition.z = stof(str);

		getline(readFile, str);
		st.xmfRotate.x = stof(str);
		getline(readFile, str);
		st.xmfRotate.y = stof(str);
		getline(readFile, str);
		st.xmfRotate.z = stof(str);

		getline(readFile, str);
		st.xmfScale.x = stof(str);
		getline(readFile, str);
		st.xmfScale.y = stof(str);
		getline(readFile, str);
		st.xmfScale.z = stof(str);

		m_mmapInfo.emplace(FileName, st);
	}

	readFile.close();

	cout << "LoadInfo : " << strFilePath << endl;
	for (auto iter = m_mmapInfo.begin(); iter != m_mmapInfo.end(); ++iter)
		cout << "\t>> " << *(*iter).first << endl;
	return S_OK;
}
