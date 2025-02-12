﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameSessions/GameSessionsManager.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/API/APIData.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HTTP/HTTPRequestTypes.h"

void UGameSessionsManager::JoinGameSession()
{
	BroadcastJoinGameSessionMessage.Broadcast(TEXT("Searching for Game Session..."), false);

	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::FindOrCreateGameSession_Response);
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::FindOrCreateGameSession);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->ProcessRequest();
}

void UGameSessionsManager::FindOrCreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		BroadcastJoinGameSessionMessage.Broadcast(HTTPStatusMessage::SomethingWentWrong, true);
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			BroadcastJoinGameSessionMessage.Broadcast(HTTPStatusMessage::SomethingWentWrong, true);
		}

		FDSGameSession GameSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &GameSession);

		const FString GameSessionId = GameSession.GameSessionId;
		const FString GameSessionStatus = GameSession.Status;
		HandleGameSessionStatus(GameSessionId, GameSessionStatus);
	}
}

void UGameSessionsManager::CreatePlayerSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		BroadcastJoinGameSessionMessage.Broadcast(HTTPStatusMessage::SomethingWentWrong, true);
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			BroadcastJoinGameSessionMessage.Broadcast(HTTPStatusMessage::SomethingWentWrong, true);
		}

		FDSPlayerSession PlayerSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &PlayerSession);
		PlayerSession.Dump();

		APlayerController* OwningPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (IsValid(OwningPlayerController))
		{
			OwningPlayerController->SetInputMode(FInputModeGameOnly());
			OwningPlayerController->SetShowMouseCursor(false);
		}

		const FString IpAndPort = PlayerSession.IpAddress + TEXT(":") + FString::FromInt(PlayerSession.Port);
		const FName Address(*IpAndPort);
		UGameplayStatics::OpenLevel(this, Address);
	}
}

FString UGameSessionsManager::GetUniquePlayerId() const
{
	const APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		const APlayerState* LocalPlayerState = LocalPlayerController->GetPlayerState<APlayerState>();
		if (IsValid(LocalPlayerState) && LocalPlayerState->GetUniqueId().IsValid())
		{
			return TEXT("Player_") + FString::FromInt(LocalPlayerState->GetUniqueID());
		}
	}
	return FString();
}

void UGameSessionsManager::HandleGameSessionStatus(const FString& GameSessionId, const FString& GameSessionStatus)
{
	if (GameSessionStatus.Equals(TEXT("ACTIVE")))
	{
		BroadcastJoinGameSessionMessage.Broadcast(TEXT("Found active Game Session. Create a Player Session..."), false);
		TryCreatePlayerSession(GetUniquePlayerId(), GameSessionId);
	}
	else if (GameSessionStatus.Equals(TEXT("ACTIVATING")))
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(CreateSessionTimerHandle, FTimerDelegate::CreateUObject(this, &UGameSessionsManager::JoinGameSession), 0.5f, false);
		}
	}
	else
	{
		BroadcastJoinGameSessionMessage.Broadcast(HTTPStatusMessage::SomethingWentWrong, true);
	}
}

void UGameSessionsManager::TryCreatePlayerSession(const FString& PlayerId, const FString& GameSessionId)
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::CreatePlayerSession_Response);
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::CreatePlayerSession);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	const TMap<FString, FString> Params =
	{
		{ TEXT("playerId"), PlayerId },
		{ TEXT("gameSessionId"), GameSessionId }
	};
	const FString Content = SerializeJsonContent(Params);
	
	Request->SetContentAsString(Content);
	Request->ProcessRequest();	
}
