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
	void EnqueueCommand(const FString& Command);
	bool DequeueCommand(FString& OutCommand);
	void SendObs(const FString& Data);
	static FMLBCommunicateThread* GetThread();

public:
	FThreadSafeBool bStopThread;
	FRunnableThread* Thread;
	FString IPAddress;
	int32 Port;

	TQueue<FString> CommandQueue;
	TQueue<FString> ObsQueue;

	TSharedPtr<FSocket> Socket;
	TSharedPtr<FInternetAddr> Addr;

	void Reconnect();
	bool ProcessIncomingData(FString& InData);
	void PushTMapIntoObsQueue(TMap<FString, float>& ObsTMap);
	void PushFStringIntoObsQueue(FString ObsFString);

	bool SendData(const FString& Data);
	bool Finished;
	const int32 ReceiveBufferSize = 1024;
	static FMLBCommunicateThread* self;
};
