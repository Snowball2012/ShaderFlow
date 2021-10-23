// Copyright(c) 2021 Sergey Kulikov

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FShaderFlowModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
