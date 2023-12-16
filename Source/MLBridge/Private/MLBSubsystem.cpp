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

UWorld* UMLBSubsystem::GetRunningWorld()
{
	UWorld* World = nullptr;

	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (const FWorldContext& Context : WorldContexts)
	{
		if ((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game))
		{
			World = Context.World();
			break;
		}
	}

	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMLBInput::GetPlayerController() World False"));
		return nullptr;
	}
	return World;
}

UPlayerInput* UMLBSubsystem::GetPlayerInput()
{
	TObjectPtr<UPlayerInput>& PlayerInput = UGameplayStatics::GetPlayerController(this->GetRunningWorld(), 0)->PlayerInput;
	return PlayerInput.Get();
}

void UMLBSubsystem::SimulateKeyPressAndRelase(FName KeyName, float Duration)
{
	FInputKeyParams InputKeyParams(FKey(KeyName), EInputEvent::IE_Pressed, 1.0);
	this->GetPlayerInput()->InputKey(InputKeyParams);
	FTimerHandle TimerHandle;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "ReleaseKey", KeyName);
	this->GetRunningWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, Duration, false);
}

void UMLBSubsystem::SimulateKeyRelease(FName KeyName)
{
	FInputKeyParams InputKeyParams(FKey(KeyName), EInputEvent::IE_Released, 1.0);
	this->GetPlayerInput()->InputKey(InputKeyParams);
}
