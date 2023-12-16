// Copyright Epic Games, Inc. All Rights Reserved.

#include "MLBridge.h"
#include "MLBManager.h"
#include "MLBSetting.h"
#define LOCTEXT_NAMESPACE "FMLBridgeModule"

/// <summary>
/// 要求：
///		模块启动后会持续检测AI主机是否在线。
///		一旦错误断线直接关闭游戏。
///		关于奖励的设计在Python那边。
///		每一次UE的log都会被保存起来用以查出问题。
/// </summary>
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
	MLBCommunicateThread->Thread->WaitForCompletion();
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMLBridgeModule, MLBridge)