#pragma once

#include "CoreMinimal.h"

#include "HAL/Runnable.h"
#include "Containers/Queue.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "MLBSubsystem.h"

class MLBRIDGE_API FMLBCommunicateThread : public FRunnable
{
public:

	// Initializes the thread with the provided IP address and port number.
	// It sets up the internet address and creates the thread with a below normal priority.
	// The Finished flag is set to false, meaning the thread has not finished its task yet.
	FMLBCommunicateThread(const FString& IPAddress, int32 Port);
	virtual ~FMLBCommunicateThread();


	// Main loop of the FMLBCommunicateThread class. It manages the connection state, handles receiving and processing data,
	// and sends outgoing data as needed.
	// It continuously checks whether it needs to stop the thread and, if not, whether the socket is connected.
	// If connected, it sets the socket to non-blocking mode and receives data in chunks, concatenating received data parts into a complete message.
	// Then, it processes the incoming data and sends any queued outgoing messages.
	// The loop sleeps for 0.01 seconds between iterations to prevent excessive resource usage.
	// When the loop ends, it sets the Finished flag to true and logs a message indicating that the Run() method is finished.
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	// Action is a part of the command received from ML machine.
	static FMLBCommunicateThread* GetThread();

public:
	TArray<FMLBCmdUnit> CmdArr;
	TArray<FMLBObsUnit> ObsArr;
	static FMLBCommunicateThread* self;
	void Reconnect();	
	TSharedPtr<FJsonObject> FStringIntoJson(FString& JsonString);
	FString JsonIntoFString(TSharedPtr<FJsonObject> JsonObject);

private:
	FThreadSafeBool bStopThread;
	FRunnableThread* Thread;
	FString IPAddress;
	int32 Port;
	TSharedPtr<FSocket> Socket;
	TSharedPtr<FInternetAddr> Addr;
	bool SendData(const FString& Data);
	const int32 ReceiveBufferSize = 1024;
};
