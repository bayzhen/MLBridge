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
	NeedToReturnObs = false;
	TimeRecord = FDateTime::Now();
	Count = 1;
	MLBCommunicateThread = FMLBCommunicateThread::GetThread();
	UDataTable* MyDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/MLBridge/DT_MLBTrainingSettings.DT_MLBTrainingSettings'"));
	MLBTrainingSetting = MyDataTable->FindRow<FMLBTrainingSetting>("1", FString(TEXT("LookupRow")));
}

// Here comes a problem. After reporting to the ML machine, the agent shall wait until the ML machine tells it the action to execute.
// So, the game should stop.
// The only way to stop the game is to stop ticking.
// It will be difficult to execute other code when tick is unabled. The blueprint execution will stop if the tick is unabled.
// There must be a loop in the manager's tick function. And the loop can only be written in cpp.
// Creating a widget can handle this. But nobody will click on the screen if the game is running without a windows.
// I believe there must be some delegates, which will listen to the actions received and trigger the actions.
// In conclusion, there must be a loop function inside mlbmanager's tick.
// it will keep checking whether there are actions and execute them if the actions exist.

void AMLBManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	while (IsTraining) {
		if (!MLBCommunicateThread->CommandQueue.IsEmpty()) {
			FString Command;
			MLBCommunicateThread->CommandQueue.Dequeue(Command);
			JsonObjectPtr = ParseFStringIntoJson(Command);
			FString Operation;
			if (JsonObjectPtr) {
				Operation = JsonObjectPtr->GetStringField("operation");
			}
			if (Operation.Equals("reset")) {
				ResetEnv();
				break;
			}
			if (Operation.Equals("step")) {
				TArray<TSharedPtr<FJsonValue>> Actions = JsonObjectPtr->GetArrayField("actions");
				TArray<float> InputValues;
				for (auto& Action : Actions) {
					InputValues.Add(Action->AsNumber());
				}
				PressKeys(InputValues);
				NeedToReturnObs = true;
				break;
			}
		}
		if (NeedToReturnObs) {
			MLBCommunicateThread->ObsQueue.Enqueue(GetObsJsonString());
			NeedToReturnObs = false;
		}
	}
	ValueBeforeStep = GetCurrentValue();
}

void AMLBManager::StepEnv(TArray<float>& actions) {
	PressKeys(actions);
	RegisterReturnObs();
	if (IsTerminated() || IsTruncated()) {
		IsTraining = false;
	}
}

void AMLBManager::HttpSend()
{
	/*auto Url = Settings->HttpURL;
	auto ContentString = GetObsJsonString();
	FHttpModule* HttpModule = FModuleManager::LoadModulePtr<FHttpModule>("HTTP");
	if (HttpModule)
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &AMLBManager::HttpReceive);
		HttpRequest->SetURL(Url);
		HttpRequest->SetVerb("POST");
		HttpRequest->SetHeader("User-Agent", "MyCustomClient/1.0");
		HttpRequest->SetHeader("Content-Type", "application/json");
		HttpRequest->SetContentAsString(ContentString);
		HttpRequest->ProcessRequest();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load HTTP module"));
	}*/
}

void AMLBManager::HttpReceive(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	// »Ö¸´ÓÎÏ·
	GetWorld()->GetWorldSettings()->SetTimeDilation(1);
	TArray<float> InputValues;
	if (bSuccess && Response.IsValid())
	{
		FString ContentString = Response->GetContentAsString();

		// Create a JSON Reader
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ContentString);

		// Parse the JSON data
		TSharedPtr<FJsonObject> JsonObject;
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			// JSON object has been successfully retrieved and parsed, now you can access its properties
			// For example, to get a string property:
			FString SomeStringProperty;
			TArray<TSharedPtr<FJsonValue>>  ActionValueArray = JsonObject->GetArrayField(TEXT("action"));
			if (ActionValueArray.Num() == 0) {
				UE_LOG(LogTemp, Warning, TEXT("MLB Action num == 0. Incorrect obs."));
			}

			for (auto JsonValuePtr : ActionValueArray) {
				InputValues.Add(JsonValuePtr.Get()->AsNumber());
			}
		}
		else
		{
			// Failed to parse the JSON data
			UE_LOG(LogTemp, Warning, TEXT("MLB Failed to parse JSON data"));
		}
	}
	else
	{
		// Request failed or response is invalid
		UE_LOG(LogTemp, Warning, TEXT("MLB Request failed or response is invalid"));
	}
	PressKeys(InputValues);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer<AMLBManager>(TimerHandle, this, &AMLBManager::HttpSend, MLBTrainingSetting->MLBDeltaTime, false);
}

void AMLBManager::PressKeys(TArray<float> InputValues)
{
	if (MLBTrainingSetting->InputKeys.Num() != InputValues.Num()) {
		UE_LOG(LogTemp, Warning, TEXT("MLB action num not right."));
		return;
	}
	else {
		for (int i = 0; i < MLBTrainingSetting->InputKeys.Num(); i++) {
			FName KeyName = MLBTrainingSetting->InputKeys[i];
			double KeyValue = InputValues[i];
			if (KeyName == "MouseX") {
				MoveMouse(FVector2D(KeyValue * 500, 0));
				continue;
			}
			if (KeyName == "MouseY") {
				MoveMouse(FVector2D(0, KeyValue * 500));
				continue;
			}
			if (KeyValue > 0) {
				FKey Key(KeyName);
				PressKey(Key);
			}
			else {
				FKey Key(KeyName);
				ReleaseKey(Key);
			}
		}
	}
}

void AMLBManager::PressKey(FKey Key)
{
	if (Key.GetFName() == "SpaceBar") {
		return;
	}
	auto PlayerInput = GetPlayerInput();
	FInputKeyParams InputKeyParams(Key, EInputEvent::IE_Pressed, 1.0);
	PlayerInput.Get()->InputKey(InputKeyParams);
}

void AMLBManager::ReleaseKey(FKey Key)
{
	auto PlayerInput = GetPlayerInput();
	FInputKeyParams InputKeyParams(Key, EInputEvent::IE_Released, 1.0);
	PlayerInput.Get()->InputKey(InputKeyParams);

}

void AMLBManager::MoveMouse(FVector2D Delta)
{

	auto PlayerInput = GetPlayerInput();
	FInputKeyParams InputKeyParams(EKeys::Mouse2D, EInputEvent::IE_Axis, FVector(Delta.X, Delta.Y, 0.0));
	PlayerInput.Get()->InputKey(InputKeyParams);
}

APlayerController* AMLBManager::GetPlayerController()
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
	return UGameplayStatics::GetPlayerController(World, 0);
}

TObjectPtr<UPlayerInput> AMLBManager::GetPlayerInput()
{
	return GetPlayerController()->PlayerInput;
}

FString AMLBManager::GetObsJsonString()
{
	auto StateArray = MakeObsArray();
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> JsonValues;

	for (float Value : StateArray)
	{
		JsonValues.Add(MakeShareable(new FJsonValueNumber(Value)));
	}

	JsonObject->SetArrayField("state", JsonValues);
	ValueDifferenceReward = GetCurrentValue() - ValueBeforeStep;
	JsonObject->SetNumberField("reward", (ValueDifferenceReward + RewardAccumulation));
	RewardAccumulation = 0;
	JsonObject->SetBoolField("terminated", IsTerminated());
	JsonObject->SetBoolField("truncated", IsTruncated());
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
	return JsonString;
}

//bool AMLBManager::CanStartTraining_Implementation()
//{
//	return false;
//}
//bool AMLBManager::ResetEnv_Implementation()
//{
//	return false;
//}
//
//bool AMLBManager::IsTruncated_Implementation()
//{
//	return false;
//}
//
//bool AMLBManager::IsTerminated_Implementation()
//{
//	return false;
//}
//
//float AMLBManager::GetCurrentValue_Implementation()
//{
//	return 0.0;
//}
//
//TArray<float> AMLBManager::GetObsArray_Implementation()
//{
//	return TArray<float>();
//}


void AMLBManager::AddReward(float Reward)
{
	this->RewardAccumulation += Reward;
}

void AMLBManager::RegisterReturnObs()
{
	NeedToReturnObs = true;
}

void AMLBManager::SocketReconnect()
{
	if (MLBCommunicateThread&& MLBCommunicateThread->Socket) {
		MLBCommunicateThread->Socket->Close();
	}
	MLBCommunicateThread->Reconnect();
}


TSharedPtr<FJsonObject> AMLBManager::ParseFStringIntoJson(FString& JsonString) {
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		return JsonObject;
	}
	else
	{
		// Failed to deserialize JSON FString
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON FString"));
		return nullptr;
	}
}