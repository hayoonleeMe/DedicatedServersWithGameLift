// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSLocalPlayerSubsystem.h"

#include "UI/Portal/Interfaces/PortalManagement.h"

void UDSLocalPlayerSubsystem::InitializeTokens(const FDSAuthenticationResult& AuthResult, TScriptInterface<IPortalManagement> PortalManagement)
{
	AuthenticationResult = AuthResult;
	PortalManagementInterface = PortalManagement;
	SetRefreshTokenTimer();
}

void UDSLocalPlayerSubsystem::SetRefreshTokenTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(RefreshTimer, FTimerDelegate::CreateLambda([this]()
		{
			// LocalPlayerSubsystem에서 PortalManager가 유효하지 않을 수 있으므로 IsValid로 체크
			if (IsValid(PortalManagementInterface.GetObject()))
			{
				PortalManagementInterface->RefreshTokens(AuthenticationResult.RefreshToken);
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
