#include "MLBCommunicateThread.h"
#include "HAL/RunnableThread.h"
#include "SocketSubsystem.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

FMLBCommunicateThread* FMLBCommunicateThread::self = nullptr;

FMLBCommunicateThread::FMLBCommunicateThread(const FString& InIPAddress, int32 InPort)
	: IPAddress(InIPAddress), Port(InPort)
{
	bStopThread = false;
	Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValid;
	Addr->SetIp(*IPAddress, bIsValid);
	Addr->SetPort(Port);
	self = this;
	Thread =  FRunnableThread::Create(this, TEXT("MLBCommunicateThread"), 0, TPri_BelowNormal);
}

FMLBCommunicateThread::~FMLBCommunicateThread()
{
	delete Thread;
}

uint32 FMLBCommunicateThread::Run()
{
	Reconnect();
	while (!bStopThread)
	{
		if (Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
		{
			TArray<uint8> Data;
			Data.SetNumUninitialized(ReceiveBufferSize);
			int32 BytesRead =0 ;
			FString ReceivedData;
			while (Socket->Recv(Data.GetData(), Data.Num(), BytesRead))
			{
				Data.SetNum(BytesRead);
				Data.Add(0);
				const char* data = reinterpret_cast<char*>(Data.GetData());
				FString DataPart = StringCast<UTF8CHAR>(data).Get();
				if (DataPart.IsEmpty())
					break;
				else {
					ReceivedData += *DataPart;
				}
			}
			// 如果收到的数据不为空。
			if (!ReceivedData.IsEmpty()) {
				// 解析字符串为JsonObjectPtr
				auto JsonObjectPtr = FStringIntoJson(ReceivedData);
				// 准备即将压入队列的操作Map
				TMap<FString, FString> InCmds;
				InCmds.Add("operation", JsonObjectPtr->GetStringField("operation"));
				TArray<float> InActionValues;
				auto ValueArr = JsonObjectPtr->GetArrayField("state");
				for (auto Value : ValueArr) {
					float ActionValue = Value->AsNumber();
					InActionValues.Add(ActionValue);
				}
				FMLBCmdUnit CmdUnit(InCmds, InActionValues);
				// 将操作Map压入指令队列。
				CmdArr.Add(CmdUnit);
			}
			while (!ObsArr.IsEmpty()) {
				FMLBObsUnit ObsUnit = ObsArr[0];
				ObsArr.RemoveAt(0);
				TSharedPtr<FJsonObject> JsonObjectPtr = MakeShared<FJsonObject>();
				TArray<TSharedPtr<FJsonValue>> StateJsonValues;
				for (auto StateValue : ObsUnit.StateValues) {
					TSharedPtr<FJsonValueNumber> StateJsonValue = MakeShared<FJsonValueNumber>(StateValue);
					StateJsonValues.Add(StateJsonValue);
				}
				JsonObjectPtr->SetNumberField("reward", ObsUnit.Reward);
				JsonObjectPtr->SetArrayField("values", StateJsonValues);
				JsonObjectPtr->SetBoolField("terminated", ObsUnit.Terminated);
				JsonObjectPtr->SetBoolField("truncated", ObsUnit.Truncated);
				FString JsonString = JsonIntoFString(JsonObjectPtr);
				this->SendData(JsonString);
			}
		}
		FPlatformProcess::Sleep(0.01f);
	}
	UE_LOG(LogTemp, Log, TEXT("FMLBCommunicateThread Run Finished."));
	return 0;
}

void FMLBCommunicateThread::Stop()
{
	bStopThread = true;
	UE_LOG(LogTemp, Log, TEXT("FMLBCommunicateThread Stop."));
	this->Thread->WaitForCompletion();
}

void FMLBCommunicateThread::Exit()
{
	bStopThread = true;
	UE_LOG(LogTemp, Log, TEXT("FMLBCommunicateThread Exit."));
}


FMLBCommunicateThread* FMLBCommunicateThread::GetThread()
{
	return self;
}

bool FMLBCommunicateThread::SendData(const FString& Data)
{
	if (Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		int32 BytesSent;
		TArray<uint8> Payload;

		// 将FString转换为UTF-8格式的字符串
		FTCHARToUTF8 DataUTF8(*Data);

		// 将字符串附加到Payload中
		Payload.Append(reinterpret_cast<const uint8*>(DataUTF8.Get()), DataUTF8.Length());

		return Socket->Send(Payload.GetData(), Payload.Num(), BytesSent);
	}

	return false;
}


void FMLBCommunicateThread::Reconnect()
{
	Socket = TSharedPtr<FSocket>(ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("MLBCommunicateThread"), false));
	// During development, there can be situation: ML machine should response to the MLBCommunicateThread but actually it's already dead.
	// If the Socket is Blocking, it will be stuck here and not able to reconnect to ML machine.
	Socket->SetNonBlocking();
	if (!Socket->Connect(*Addr))
	{
		// 不打印了。
		// UE_LOG(LogTemp, Warning, TEXT("Failed to connect to %s:%d"), *IPAddress, Port);
		Socket.Reset();
		FPlatformProcess::Sleep(5.0f); // Wait for 5 seconds before trying to reconnect
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Connected to %s:%d"), *IPAddress, Port);
	}
}


TSharedPtr<FJsonObject> FMLBCommunicateThread::FStringIntoJson(FString& JsonString) {
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

FString FMLBCommunicateThread::JsonIntoFString(TSharedPtr<FJsonObject> JsonObject)
{
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
	return JsonString;
}
