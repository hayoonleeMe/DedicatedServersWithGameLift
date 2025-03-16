// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/SignIn/AccountDropdown_Expanded.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/DSLocalPlayerSubsystem.h"
#include "UI/Portal/PortalManager.h"

void UAccountDropdown_Expanded::NativeConstruct()
{
	Super::NativeConstruct();

	PortalManager = NewObject<UPortalManager>(this, PortalManagerClass);

	Button_SignOut->OnClicked.AddDynamic(this, &UAccountDropdown_Expanded::SignOutButton_OnClicked);
	Button_SignOut->OnHovered.AddDynamic(this, &UAccountDropdown_Expanded::SignOutButton_Hover);
	Button_SignOut->OnUnhovered.AddDynamic(this, &UAccountDropdown_Expanded::SignOutButton_Unhover);
}

void UAccountDropdown_Expanded::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetSignOutButtonStyleTransparent();
	SignOutButton_Unhover();
}

void UAccountDropdown_Expanded::SignOutButton_Hover()
{
	TextBlock_SignOutButtonText->SetColorAndOpacity(HoveredTextColor);
}

void UAccountDropdown_Expanded::SignOutButton_Unhover()
{
	TextBlock_SignOutButtonText->SetColorAndOpacity(UnhoveredTextColor);
}

void UAccountDropdown_Expanded::SignOutButton_OnClicked()
{
	Button_SignOut->SetIsEnabled(false);

	check(PortalManager);
	const APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController) && IsValid(PlayerController->GetLocalPlayer()))
	{
		if (const UDSLocalPlayerSubsystem* LocalPlayerSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UDSLocalPlayerSubsystem>())
		{
			PortalManager->SignOut(LocalPlayerSubsystem->GetAuthResult().AccessToken);
		}
	}
}

void UAccountDropdown_Expanded::SetSignOutButtonStyleTransparent()
{
	FButtonStyle Style;
	FSlateBrush Brush;
	Brush.TintColor = FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	Style.Normal = Brush;
	Style.Pressed = Brush;
	Style.Hovered = Brush;
	Style.Disabled = Brush;
	Button_SignOut->SetStyle(Style);
}
