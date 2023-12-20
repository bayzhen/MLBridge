#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
};
