// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/SignIn/SignInOverlay.h"

#include "Components/Button.h"
#include "UI/API/GameSessions/JoinGame.h"
#include "UI/Portal/PortalManager.h"

void USignInOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	check(PortalManagerClass);
	check(JoinGameWidget && JoinGameWidget->Button_JoinGame);
	
	PortalManager = NewObject<UPortalManager>(this, PortalManagerClass);
	JoinGameWidget->Button_JoinGame->OnClicked.AddDynamic(this, &USignInOverlay::OnJoinGameButtonClicked);
}

void USignInOverlay::OnJoinGameButtonClicked()
{
	check(IsValid(PortalManager));
	check(JoinGameWidget && JoinGameWidget->Button_JoinGame);
	
	PortalManager->BroadcastJoinGameSessionMessage.AddDynamic(this, &USignInOverlay::UpdateJoinGameStatusMessage);
	PortalManager->JoinGameSession();

	JoinGameWidget->Button_JoinGame->SetIsEnabled(false);
}

void USignInOverlay::UpdateJoinGameStatusMessage(const FString& StatusMessage, bool bResetJoinGameButton)
{
	check(JoinGameWidget && JoinGameWidget->Button_JoinGame);
	JoinGameWidget->SetStatusMessage(StatusMessage);

	if (bResetJoinGameButton)
	{
		JoinGameWidget->Button_JoinGame->SetIsEnabled(true);
	}
}
