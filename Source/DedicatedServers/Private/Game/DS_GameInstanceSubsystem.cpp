// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DS_GameInstanceSubsystem.h"

#include "DedicatedServers/DedicatedServers.h"

void UDS_GameInstanceSubsystem::InitGameLift(const FServerParameters& ServerParameters)
{
	if (bGameLiftInitialized)
	{
		return;
	}

#if WITH_GAMELIFT
	
	UE_LOG(LogDedicatedServers, Log, TEXT("Initializing the GameLift Server"));

	//Getting the module first.
	FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	//Define the server parameters for a GameLift Anywhere fleet. These are not needed for a GameLift managed EC2 fleet.
	// 이때 fleet은 이 서버 프로그램을 가동하는 hosting resources를 나타낸다. AWS EC2 fleet이 될 수도 있고, 직접 서버를 가동하면 개인 하드웨어가 될 수도 있다.
	//InitSDK establishes a local connection with GameLift's agent to enable further communication.
	//Use InitSDK(serverParameters) for a GameLift Anywhere fleet. 
	//Use InitSDK() for a GameLift managed EC2 fleet.
	GameLiftSdkModule->InitSDK(ServerParameters);

	// ProcessParameters.OnStartGameSession : GameLift의 요청에 의해 실제 게임 서버의 준비가 완료될 때
	auto OnGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		FString GameSessionId = FString(gameSession.GetGameSessionId());
		UE_LOG(LogDedicatedServers, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
		GameLiftSdkModule->ActivateGameSession();
	};
	ProcessParameters.OnStartGameSession.BindLambda(OnGameSession);

	// ProcessParameters.OnTerminate : 이 게임 서버의 호스팅 인스턴스가 종료되기 전에 추가 작업을 수행하기 위해 호출된다.
	auto OnProcessTerminate = [=]()
    {
    	UE_LOG(LogDedicatedServers, Log, TEXT("Game Server process is terminating."));
    	GameLiftSdkModule->ProcessEnding();
    };
    ProcessParameters.OnTerminate.BindLambda(OnProcessTerminate);

	//GameLift invokes this callback approximately every 60 seconds.
	// false를 반환하면 서버가 종료된다.
	auto OnHealthCheck = []() 
    {
    	UE_LOG(LogDedicatedServers, Log, TEXT("Performing Health Check"));
    	return true;
    };
    ProcessParameters.OnHealthCheck.BindLambda(OnHealthCheck);

	// 플레이어가 연결할 Port 설정
	// Command Line Arguments로 넘길 경우 해당 Port를 사용하고, 아니라면 UE의 기본 포트인 7777 사용 
	int32 Port = FURL::UrlConfig.DefaultPort;
	ParseCommandLinePort(Port);
	ProcessParameters.port = Port;

	// GameSession Log를 저장할 위치 저장
	TArray<FString> LogFiles;
	LogFiles.Add(TEXT("FPSTemplate/Saved/Logs/FPSTemplate.log"));
	ProcessParameters.logParameters = LogFiles;

	//The game server calls ProcessReady() to tell GameLift it's ready to host game sessions.
	UE_LOG(LogDedicatedServers, Log, TEXT("Calling Process Ready."));
	GameLiftSdkModule->ProcessReady(ProcessParameters);
	
#endif

	bGameLiftInitialized = true;
}

void UDS_GameInstanceSubsystem::ParseCommandLinePort(int32& OutPort)
{
	// Command Line에서 Token과 switch로 나눈다.
	TArray<FString> CommandLineTokens;
	TArray<FString> CommandLineSwitches;
	FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

	// port=???? 꼴의 Switch를 찾아 OutPort를 해당 Value로 설정한다.
	for (const FString& Switch : CommandLineSwitches)
	{
		FString Key;
		FString Value;
		if (Switch.Split("=", &Key, &Value))
		{
			if (Key.Equals(TEXT("port"), ESearchCase::IgnoreCase) && Value.IsNumeric())
			{
				LexFromString(OutPort, *Value);
				return;
			}
		}
	} 
}
