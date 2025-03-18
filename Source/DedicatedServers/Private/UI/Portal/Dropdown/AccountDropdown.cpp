// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dropdown/AccountDropdown.h"

#include "Components/TextBlock.h"
#include "Player/DSLocalPlayerSubsystem.h"

void UAccountDropdown::NativeConstruct()
{
	Super::NativeConstruct();

	if (const UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem())
	{
		TextBlock_ButtonText->SetText(FText::FromString(LocalPlayerSubsystem->Username));
	}
}

UDSLocalPlayerSubsystem* UAccountDropdown::GetLocalPlayerSubsystem() const
{
	const APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController) && IsValid(PlayerController->GetLocalPlayer()))
	{
		return PlayerController->GetLocalPlayer()->GetSubsystem<UDSLocalPlayerSubsystem>();
	}
	return nullptr;
}
