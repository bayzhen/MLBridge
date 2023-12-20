// Copyright Epic Games, Inc. All Rights Reserved.

#include "MLBridge.h"
#include "MLBManager.h"
#include "MLBSetting.h"
#define LOCTEXT_NAMESPACE "FMLBridgeModule"

void FMLBridgeModule::StartupModule()
{
	UDataTable* MyDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/MLBridge/DT_MLBConnectionSettings.DT_MLBConnectionSettings'"));
	if (MyDataTable) {
		FMLBConnectionSetting* RowData = MyDataTable->FindRow<FMLBConnectionSetting>("1", FString(TEXT("LookupRow")));
		SocketIP = RowData->SocketIP;
		SocketPort = RowData->SocketPort;
		MLBCommunicateThread = TSharedPtr<FMLBCommunicateThread>(new FMLBCommunicateThread(SocketIP, SocketPort));
	}
}

void FMLBridgeModule::ShutdownModule()
{
	MLBCommunicateThread->Stop();
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMLBridgeModule, MLBridge)