#pragma once

#include "CoreMinimal.h"
#include "MLBCommunicateThread.h"
#include "Modules/ModuleManager.h"

class MLBRIDGE_API FMLBridgeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FMLBCommunicateThread> MLBCommunicateThread;
	FString SocketIP;
	int SocketPort ;
};
