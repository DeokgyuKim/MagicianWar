#include "Com_Network.h"
#include "Network.h"
Com_Network::Com_Network(int tag, int netEvent)
	:m_iTag(tag), m_NetEvent(netEvent)
{

}

Com_Network::~Com_Network()
{
}

void Com_Network::LateUpdate(const float& fTimeDelta)
{
}


void Com_Network::CallEvent(int _event ,int val1, int val2, int val3)
{
	if (_event == NULL)
	{
		if (val3 != NULL)
			Network::GetInstance()->CallEvent(m_NetEvent, 3, val1, val2, val3);
		else if (val2 != NULL)
			Network::GetInstance()->CallEvent(m_NetEvent, 2, val1, val2);
		else if (val1 != NULL)
			Network::GetInstance()->CallEvent(m_NetEvent, 1, val1);
		else
			Network::GetInstance()->CallEvent(m_NetEvent, 0);
	}
	else
	{
		if (val3 != NULL)
			Network::GetInstance()->CallEvent(_event, 3, val1, val2, val3);
		else if (val2 != NULL)
			Network::GetInstance()->CallEvent(_event, 2, val1, val2);
		else if (val1 != NULL)
			Network::GetInstance()->CallEvent(_event, 1, val1);
		else
			Network::GetInstance()->CallEvent(_event, 0);
	}

}
