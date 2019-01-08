#include "Counter.h"




void Counter::Start()
{
	mValue = 0;
}

int Counter::GetCount() const
{
	return mValue;
}

void Counter::Increment()
{
	++mValue;
}
