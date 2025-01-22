// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/PortalManager.h"

void UPortalManager::JoinGameSession()
{
	BroadcastJoinGameSessionMessage.Broadcast(TEXT("Searching for Game Session..."));
}
