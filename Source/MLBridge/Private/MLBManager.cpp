#include "MLBManager.h"
#include "TimerManager.h"
#include "MLBSetting.h"

#include "GameFramework/Character.h"
#include "GameFramework/WorldSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameViewportClient.h"
#include "MLBSubsystem.h"
#include "UnrealClient.h"


// When the beginplay is triggerd, the manager should be told that it should not start training at once.
// Because there can be objects that haven't been loaded. And the hero can be null.
// The manager haven't collected the obs so it is not necessary to return obs.
// The manager will keep checking if the MLBCommunicationThread has received new actions.
// There will be some parameters for training in the DT_MLBTrainingSettings datatable.
// The parameters won't change in the training process but they can be different in different trainig envs.
// So the manager should know the name of env it is in.
void AMLBManager::BeginPlay()
{
	Super::BeginPlay();
	IsTraining = false;
	MLBCommunicateThread = FMLBCommunicateThread::GetThread();
	UDataTable* MyDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/MLBridge/DT_MLBTrainingSettings.DT_MLBTrainingSettings'"));
	MLBTrainingSetting = MyDataTable->FindRow<FMLBTrainingSetting>(MLBTrainingSettingRowName, FString(TEXT("")));
}

// After reporting to the ML machine, the agent shall wait until the ML machine tells it the action to execute.
// So, the game should stop.
// The only way to stop the game is to stop ticking.
// It will be difficult to execute other code when tick is unabled. The blueprint execution will stop if the tick is unabled.
// There must be a loop in the manager's tick function. And the loop can only be written in cpp.
// Creating a widget can handle this. But nobody will click on the screen if the game is running without a windows.
// I believe there must be some delegates, which will listen to the actions received and trigger the actions.
// In conclusion, there must be a loop function inside mlbmanager's tick.
// it will keep checking whether there are actions and execute them if the actions exist.
// The deltaseconds can be set. The frequency of ticking can be set. This is more convenient.

void AMLBManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// return if it is not training.
	if (!IsTraining) return;
	// The time Tick started.
	FDateTime TickGameTime = FDateTime::Now();
	// Check Whether there is command received.
	if (this->AllowTickingWhenTraining) {
		if (MLBCommunicateThread->CmdArr.Num() > 0) {
			ExecuteCmdUnit(MLBCommunicateThread->CmdArr[0]);
			MLBCommunicateThread->CmdArr.RemoveAt(0);
			FMLBObsUnit Obs = CollectObsUnit();
			MLBCommunicateThread->ObsArr.Add(Obs);
		}
	}
	else {
		bool HasExecuteCommand = false;
		while (!HasExecuteCommand && IsTraining) {
			FDateTime CurGameTime = FDateTime::Now();
			if ((CurGameTime - TickGameTime).GetTotalSeconds() > 30) {
				MLBLogInfo(FString("Waiting over 30s. Stop training."));
				IsTraining = false;
			}
			if (MLBCommunicateThread->CmdArr.Num() > 0) {
				ExecuteCmdUnit(MLBCommunicateThread->CmdArr[0]);
				MLBCommunicateThread->CmdArr.RemoveAt(0);
				FMLBObsUnit Obs = CollectObsUnit();
				MLBCommunicateThread->ObsArr.Add(Obs);
				HasExecuteCommand = true;
			}
		}
	}
}

void AMLBManager::SocketReconnect()
{
	if (this->MLBCommunicateThread) {
		UE_LOG(LogTemp, Log, TEXT("MLBCommunicateThread->Reconnect()"));
		this->MLBCommunicateThread->Reconnect();
	}
}

void AMLBManager::StartTraining()
{
	SocketReconnect();
	IsTraining = true;
}

void AMLBManager::EndTraining()
{
	IsTraining = false;
}

