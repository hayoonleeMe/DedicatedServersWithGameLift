// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DSGameState.h"

#include "Lobby/LobbyState.h"
#include "Net/UnrealNetwork.h"

ADSGameState::ADSGameState()
{
	bReplicates = true;
}

void ADSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSGameState, LobbyState);
}

void ADSGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CreateLobbyState();
		OnLobbyStateInitialized.Broadcast(LobbyState);
	}
}

void ADSGameState::CreateLobbyState()
{
	if (UWorld* World = GetWorld())
	{
		LobbyState = World->SpawnActor<ALobbyState>(ALobbyState::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		if (LobbyState)
		{
			LobbyState->SetOwner(this);
		}
	}
}

void ADSGameState::OnRep_LobbyState()
{
	OnLobbyStateInitialized.Broadcast(LobbyState);
}
