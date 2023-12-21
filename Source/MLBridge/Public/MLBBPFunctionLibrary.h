#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MLBSetting.h"
#include "MLBBPFunctionLibrary.generated.h"


UCLASS()
class MLBRIDGE_API UMLBBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable, Category = "MLBridge")
	//static bool IsEven(int32 Number);

	//UFUNCTION(BlueprintCallable, Category = "MLBridge")
	//static float CalculateTriangleArea(float Base, float Height);

	UFUNCTION(BlueprintCallable)
	UWorld* GetRunningWorld();

	UFUNCTION(BlueprintCallable)
	UPlayerInput* GetPlayerInput();

	UFUNCTION(BlueprintCallable)
	void SimulateKeyPressAndRelase(FName KeyName, float Duration);

	UFUNCTION(BlueprintCallable)
	void SimulateKeyRelease(FName KeyName);

	UFUNCTION(BlueprintPure)
	static FMLBObsUnit MakeMLBObsUnit(TArray<float> InStateValues, float InReward, bool InTerminated, bool InTruncated) {
		return FMLBObsUnit(InStateValues, InReward, InTerminated, InTruncated);
	}

	UFUNCTION(BlueprintPure)
	static void BreakMLBObsUnit(const FMLBObsUnit& MLBObsUnit, TArray<float>& InStateValues, float& InReward, bool& InTerminated, bool& InTruncated) {
		InStateValues = MLBObsUnit.StateValues;
		InReward = MLBObsUnit.Reward; 
		InTerminated = MLBObsUnit.Terminated; 
		InTruncated = MLBObsUnit.Truncated;
	}

	UFUNCTION(BlueprintPure)
	static FMLBCmdUnit MakeMLBCmdUnit(TMap<FString, FString> InCmds, TArray<float> InActionValues) {
		return FMLBCmdUnit(InCmds, InActionValues);
	}

	UFUNCTION(BlueprintPure)
	static void BreakMLBCmdUnit(const FMLBCmdUnit& MLBCmdUnit, TMap<FString, FString>& InCmds, TArray<float>& InActionValues) {
		InCmds = MLBCmdUnit.Cmds; 
		InActionValues = MLBCmdUnit.ActionValues;
	}
};
