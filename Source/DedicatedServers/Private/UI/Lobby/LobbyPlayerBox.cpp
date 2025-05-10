// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LobbyPlayerBox.h"

#include "Components/ScrollBox.h"
#include "Game/DSGameState.h"
#include "Lobby/LobbyState.h"
#include "UI/Lobby/PlayerLabel.h"

void ULobbyPlayerBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ADSGameState* DSGameState = GetWorld()->GetGameState<ADSGameState>();
	if (!IsValid(DSGameState))
	{
		return;
	}

	
	if (IsValid(DSGameState->LobbyState))
	{
		OnLobbyStateInitialized(DSGameState->LobbyState);
	}
	else
	{
		DSGameState->OnLobbyStateInitialized.AddDynamic(this, &ULobbyPlayerBox::OnLobbyStateInitialized);
	}
}

void ULobbyPlayerBox::UpdatePlayerInfo(ALobbyState* LobbyState)
{
	ScrollBox_PlayerInfo->ClearChildren();

	for (const FLobbyPlayerInfo& PlayerInfo : LobbyState->GetPlayers())
	{
		CreateAndAddPlayerLabel(PlayerInfo);
	}
}

void ULobbyPlayerBox::OnLobbyStateInitialized(ALobbyState* LobbyState)
{
	if (!IsValid(LobbyState))
	{
		return;
	}

	LobbyState->OnPlayerInfoAdded.AddDynamic(this, &ULobbyPlayerBox::CreateAndAddPlayerLabel);
	LobbyState->OnPlayerInfoRemoved.AddDynamic(this, &ULobbyPlayerBox::OnPlayerRemoved);
	UpdatePlayerInfo(LobbyState);
}

void ULobbyPlayerBox::CreateAndAddPlayerLabel(const FLobbyPlayerInfo& PlayerInfo)
{
	if (FindPlayerLabel(PlayerInfo.Username))
	{
		// 이미 존재하면 early return
		return;
	}
	
	if (UPlayerLabel* PlayerLabel = CreateWidget<UPlayerLabel>(this, PlayerLabelClass))
	{
		PlayerLabel->SetUsername(PlayerInfo.Username);
		ScrollBox_PlayerInfo->AddChild(PlayerLabel);
	}
}

void ULobbyPlayerBox::OnPlayerRemoved(const FLobbyPlayerInfo& PlayerInfo)
{
	if (UPlayerLabel* PlayerLabel = FindPlayerLabel(PlayerInfo.Username))
	{
		ScrollBox_PlayerInfo->RemoveChild(PlayerLabel);
	}
}

UPlayerLabel* ULobbyPlayerBox::FindPlayerLabel(const FString& Username)
{
	for (UWidget* Child : ScrollBox_PlayerInfo->GetAllChildren())
	{
		if (UPlayerLabel* PlayerLabel = Cast<UPlayerLabel>(Child); PlayerLabel->GetUsername() == Username)
		{
			return PlayerLabel;
		}
	}
	return nullptr;
}
