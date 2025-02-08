// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSLocalPlayerSubsystem.h"

void UDSLocalPlayerSubsystem::InitializeTokens(const FDSAuthenticationResult& AuthResult, UPortalManager* Manager)
{
	AuthenticationResult = AuthResult;
	PortalManager = Manager;
}
