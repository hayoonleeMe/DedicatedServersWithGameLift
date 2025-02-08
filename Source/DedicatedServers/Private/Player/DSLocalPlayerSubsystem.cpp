// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSLocalPlayerSubsystem.h"

#include "UI/Portal/PortalManager.h"

void UDSLocalPlayerSubsystem::InitializeTokens(const FDSAuthenticationResult& AuthResult, UPortalManager* Manager)
{
	AuthenticationResult = AuthResult;
	PortalManager = Manager;
	SetRefreshTokenTimer();
}

void UDSLocalPlayerSubsystem::SetRefreshTokenTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(RefreshTimer, FTimerDelegate::CreateLambda([this]()
		{
			if (PortalManager)
			{
				PortalManager->RefreshTokens(AuthenticationResult.RefreshToken);
			}
		}), TokenRefreshInterval, false);
	}
}

void UDSLocalPlayerSubsystem::UpdateTokens(const FString& AccessToken, const FString& IdToken)
{
	AuthenticationResult.AccessToken = AccessToken;
	AuthenticationResult.IdToken = IdToken;
	AuthenticationResult.Dump();
	SetRefreshTokenTimer();
}
