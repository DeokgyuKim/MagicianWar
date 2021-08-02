#include "RadioButtonController.h"

RadioButtonController::RadioButtonController()
{
}

RadioButtonController::~RadioButtonController()
{
}

void RadioButtonController::PushRadioButton(RadioButton* pRadio)
{
	m_vecRadios.push_back(pRadio);
}

void RadioButtonController::RemoveRadioButton(RadioButton* pRadio)
{
	auto iter = find(m_vecRadios.begin(), m_vecRadios.end(), pRadio);
	if (iter == m_vecRadios.end())
		return;
	m_vecRadios.erase(iter);
}

void RadioButtonController::SetRadioOthers()
{
	for (int i = 0; i < m_vecRadios.size(); ++i)
	{
		for (int j = 0; j < m_vecRadios.size(); ++j)
		{
			if (i == j)
				continue;
			m_vecRadios[i]->SetOthers(m_vecRadios[j]);
		}
	}
}
