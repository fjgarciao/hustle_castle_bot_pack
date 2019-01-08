#pragma once

#include "pch.h"
#include "Logic_sm.h"
#include "Applet.h"
#include "LogicFunctors.h"
#include "Arena.h"



class Logic : public LogicContext<Logic>, public AppletWrapper, public LogicFunctors, public Arena
{
public:
	Logic(Applet &app)
		: AppletWrapper(app, *this), Arena(app)
	{}

	bool IsRunned() const;

	void End();

private:
	bool mRunned = true;

};

