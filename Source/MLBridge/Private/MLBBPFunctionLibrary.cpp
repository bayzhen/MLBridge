#include "MLBBPFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


UWorld* UMLBBPFunctionLibrary::GetRunningWorld()
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

UPlayerInput* UMLBBPFunctionLibrary::GetPlayerInput()
{
	TObjectPtr<UPlayerInput>& PlayerInput = UGameplayStatics::GetPlayerController(this->GetRunningWorld(), 0)->PlayerInput;
	return PlayerInput.Get();
}

void UMLBBPFunctionLibrary::SimulateKeyPressAndRelase(FName KeyName, float Duration)
{
	FInputKeyParams InputKeyParams(FKey(KeyName), EInputEvent::IE_Pressed, 1.0);
	this->GetPlayerInput()->InputKey(InputKeyParams);
	FTimerHandle TimerHandle;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "SimulateKeyRelease", KeyName);
	this->GetRunningWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, Duration, false);
}

void UMLBBPFunctionLibrary::SimulateKeyRelease(FName KeyName)
{
	FInputKeyParams InputKeyParams(FKey(KeyName), EInputEvent::IE_Released, 1.0);
	this->GetPlayerInput()->InputKey(InputKeyParams);
}
