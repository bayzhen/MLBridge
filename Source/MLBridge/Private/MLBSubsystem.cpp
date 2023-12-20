#include "MLBSubsystem.h"
#include "Kismet/GameplayStatics.h"

UMLBSubsystem* UMLBSubsystem::self = nullptr;

void UMLBSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // Your subsystem initialization logic here
    self = this;
}

void UMLBSubsystem::Deinitialize()
{
    // Your subsystem deinitialization logic here
    Super::Deinitialize();
}

UMLBSubsystem* UMLBSubsystem::Get()
{
    return self;
}
