#pragma once
#include "RadioButton.h"

class RadioButtonController
{
public:
	RadioButtonController();
	~RadioButtonController();
public:
	void PushRadioButton(RadioButton* pRadio);
	void RemoveRadioButton(RadioButton* pRadio);
public:
	void SetRadioOthers();
protected:
	vector<RadioButton*> m_vecRadios;
};

