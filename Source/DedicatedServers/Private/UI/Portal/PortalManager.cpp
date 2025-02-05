// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/PortalManager.h"

#include "Kismet/KismetSystemLibrary.h"

void UPortalManager::SignIn(const FString& UserName, const FString& Password)
{
}

void UPortalManager::SignUp(const FString& UserName, const FString& Password, const FString& Email)
{
	
}

void UPortalManager::Confirm(const FString& ConfirmationCode)
{
	
}

void UPortalManager::QuitGame()
{
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
	}
}
