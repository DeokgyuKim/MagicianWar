#include "AnimationFSM.h"
#include "InterfaceCommand.h"
#include "AnimationCommand.h"

AnimationFSM* AnimationFSM::m_pInstance = nullptr;

AnimationFSM::AnimationFSM()
{
}

AnimationFSM::~AnimationFSM()
{
	Release();
}

void AnimationFSM::Release()
{
	for (auto& command : m_FSM)
	{
		delete command;
		command = nullptr;
	}

	m_FSM.clear();
}

void AnimationFSM::PushAnimation(int _event, InterfaceAnimation* _IAni)
{
	InterfaceCommand* command = new AnimationCommand(_IAni);
	command->SetState(_event);
	m_FSM.push_back(command);
}

void AnimationFSM::PopAnimation(int _event)
{
	if (m_FSM.size() < 2) return;

	m_FSM.remove_if([&](InterfaceCommand* command) {
		if (command->GetState() == _event)
			return true;
		return false;
	});
}

void AnimationFSM::Execute()
{
	m_FSM.back()->Execute();
}


