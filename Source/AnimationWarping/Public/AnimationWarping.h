// Written by Berkay Tuna, August 2020

#pragma once

#include "Modules/ModuleManager.h"

class FAnimationWarpingModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
