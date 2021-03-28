#pragma once

#include "InterfaceCommand.h"

class InterfaceAnimation;
class AnimationCommand : public InterfaceCommand
{
public:
	explicit AnimationCommand(InterfaceAnimation* user);
	virtual ~AnimationCommand(){}
	AnimationCommand(const AnimationCommand& rhs) = delete;
public:
	virtual void Execute() override;
private:
	void Idle();
	void Move_Forward();
	void Move_Back();
	void Move_Left();
	void Move_Right();
	void Attack();
	void Jump();


};

