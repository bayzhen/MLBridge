#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/PlayerInput.h"
#include "MLBSetting.generated.h"

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
	FMLBConnectionSetting() :ConnectionMethod(EConnectionMethod::Socket),SocketIP(""),SocketPort(0), HttpURL(""),HttpsURL("") {};

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

	FMLBTrainingSetting() :MLBDeltaTime(0.f) , InputKeys(){};
	UPROPERTY(EditAnywhere, Category = "MLBridge")
	float MLBDeltaTime;

	// 键盘鼠标所有输入
	UPROPERTY(EditAnywhere, Category = "MLBridge", Meta = (ToolTip = ""))
	TArray<FName> InputKeys;
};
