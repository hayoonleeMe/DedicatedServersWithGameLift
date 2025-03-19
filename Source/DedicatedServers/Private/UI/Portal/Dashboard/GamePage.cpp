// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dashboard/GamePage.h"

#include "Components/Button.h"
#include "UI/API/GameSessions/JoinGame.h"
#include "UI/GameSessions/GameSessionsManager.h"

void UGamePage::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(GameSessionsManagerClass);
	GameSessionsManager = NewObject<UGameSessionsManager>(this, GameSessionsManagerClass);
	GameSessionsManager->BroadcastJoinGameSessionMessage.AddDynamic(JoinGameWidget, &UJoinGame::SetStatusMessage);
	
	JoinGameWidget->Button_JoinGame->OnClicked.AddDynamic(this, &UGamePage::JoinGameButtonClicked);
}

void UGamePage::JoinGameButtonClicked()
{
	JoinGameWidget->Button_JoinGame->SetIsEnabled(false);

	if (GameSessionsManager)
	{
		GameSessionsManager->JoinGameSession();
	}
}
