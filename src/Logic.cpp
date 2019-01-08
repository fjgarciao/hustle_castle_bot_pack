#include "Logic.h"



void Logic::End()
{
	mRunned = false;
}

bool Logic::IsRunned() const
{
	return mRunned;
}
