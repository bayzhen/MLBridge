#pragma once

#include "Subsystems/Subsystem.h"
#include "MLBSetting.h"
#include "Delegates/Delegate.h"
#include "MLBSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrainingStart);

UCLASS()
class MLBRIDGE_API UMLBSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    static UMLBSubsystem* Get();

    // Add your custom functions here

    static UMLBSubsystem* self;
    UPROPERTY(BlueprintAssignable, Category = "MLBridge")
    FOnTrainingStart OnTrainingStart;

};
