#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/PlayerInput.h"
#include "MLBSetting.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogMLBridge, Verbose, All); \
DEFINE_LOG_CATEGORY(LogMLBridge);
void MLBLogInfo(FString Log) {
	UE_LOG(LogMLBridge, Log, TEXT("%s"), *Log);
}

USTRUCT(BlueprintType)
struct MLBRIDGE_API FMLBCommuniteUnit {
	GENERATED_BODY()
	FMLBCommuniteUnit() : Id(FMath::RandRange(0, 999999)), Working(true) {}
	int32 Id;
	bool Working;
};

USTRUCT(BlueprintType, meta = (HasNativeBreak = "/Script/MLBridge.MLBBPFunctionLibrary.BreakMLBCmdUnit", HasNativeMake = "/Script/MLBridge.MLBBPFunctionLibrary.MakeMLBCmdUnit"))
struct MLBRIDGE_API FMLBCmdUnit : public FMLBCommuniteUnit {
	GENERATED_BODY()
	FMLBCmdUnit() : Cmds(), ActionValues() {}
	FMLBCmdUnit(TMap<FString, FString>& InCmds, TArray<float>& InActionValues) :
		Cmds(InCmds),
		ActionValues(InActionValues) {}
	UPROPERTY(EditAnywhere, Category = "MLBridge")
	TMap<FString, FString> Cmds;
	UPROPERTY(EditAnywhere, Category = "MLBridge")
	TArray<float> ActionValues;
};

USTRUCT(BlueprintType, meta = (HasNativeBreak = "/Script/MLBridge.MLBBPFunctionLibrary.BreakMLBObsUnit", HasNativeMake = "/Script/MLBridge.MLBBPFunctionLibrary.MakeMLBObsUnit"))
struct MLBRIDGE_API FMLBObsUnit : public FMLBCommuniteUnit {
	GENERATED_BODY()
	FMLBObsUnit() :
		StateValues(),
		Reward(0),
		Terminated(false),
		Truncated(false) {}
	FMLBObsUnit(TArray<float>& InStateValues, float& InReward, bool& InTerminated, bool& InTruncated) :
		StateValues(InStateValues),
		Reward(InReward),
		Terminated(InTerminated),
		Truncated(InTruncated) {}
	UPROPERTY(EditAnywhere, Category = "MLBridge")
	TArray<float> StateValues;

	UPROPERTY(EditAnywhere, Category = "MLBridge")
	float Reward;

	UPROPERTY(EditAnywhere, Category = "MLBridge")
	bool Terminated;

	UPROPERTY(EditAnywhere, Category = "MLBridge")
	bool Truncated;
};

USTRUCT(BlueprintType)
struct MLBRIDGE_API FMLBInputKey {
	GENERATED_BODY()
	FMLBInputKey() : KeyName(""), Value(0) {}
	UPROPERTY(EditAnywhere, Category = "MLBridge")
	FName KeyName;

	UPROPERTY(EditAnywhere, Category = "MLBridge")
	float Value;
};

UENUM(BlueprintType)
enum class EConnectionMethod : uint8
{
	Socket UMETA(DisplayName = "Socket"),
	Http UMETA(DisplayName = "Http"),
	Https UMETA(DisplayName = "Https")
};

USTRUCT(BlueprintType)
struct MLBRIDGE_API FMLBConnectionSetting :public FTableRowBase
{
	GENERATED_BODY()
public:
	FMLBConnectionSetting() :ConnectionMethod(EConnectionMethod::Socket), SocketIP(""), SocketPort(0), HttpURL(""), HttpsURL("") {};

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "MLBridge")
	EConnectionMethod ConnectionMethod;

	UPROPERTY(EditAnywhere, Category = "MLBridge")
	FString SocketIP;

	UPROPERTY(EditAnywhere, Category = "MLBridge")
	int SocketPort;

	UPROPERTY(EditAnywhere, Category = "MLBridge")
	FString HttpURL;

	UPROPERTY(EditAnywhere, Category = "MLBridge")
	FString HttpsURL;
};

USTRUCT(BlueprintType)
struct MLBRIDGE_API FMLBTrainingSetting :public FTableRowBase
{
	GENERATED_BODY()
public:

	FMLBTrainingSetting() :MLBDeltaTime(0.f), InputKeys() {};
	UPROPERTY(EditAnywhere, Category = "MLBridge")
	float MLBDeltaTime;

	// 键盘鼠标所有输入
	UPROPERTY(EditAnywhere, Category = "MLBridge", Meta = (ToolTip = ""))
	TArray<FName> InputKeys;
};
