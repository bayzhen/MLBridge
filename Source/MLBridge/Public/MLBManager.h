#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "MLBSetting.h"
#include "MLBCommunicateThread.h"
#include "MLBManager.generated.h"

// The ML Bridge Manager connects, registers, and tunes the game for training, 
// sends information to the server, executes instructions, 
// and offers controlled acceleration to improve training efficiency while avoiding errors.

UCLASS()
class MLBRIDGE_API AMLBManager : public APawn
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
public:
	// return the ObsMap
	UFUNCTION(BlueprintImplementableEvent, Category = "MLBridge")
	FMLBObsUnit CollectObsUnit();
	// Any logic about SAR should be done in BP.
	UFUNCTION(BlueprintImplementableEvent, Category = "MLBridge")
	void ExecuteCmdUnit(const FMLBCmdUnit Cmd);
public:
	UFUNCTION(BlueprintCallable, Category = "MLBridge")
	void SocketReconnect();

	UFUNCTION(BlueprintCallable, Category = "MLBridge")
	void StartTraining();

	UFUNCTION(BlueprintCallable, Category = "MLBridge")
	void EndTraining();
public:
	FSocket* Socket;
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MLBridge")
	bool IsTraining;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MLBridge")
	FName MLBTrainingSettingRowName;
	FMLBCommunicateThread* MLBCommunicateThread;
	FMLBTrainingSetting* MLBTrainingSetting;
	TMap<FString, float> ObsMap;
};
