// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/PortalHUD.h"
#include "UI/Portal/SignIn/SignInOverlay.h"
#include "Blueprint/UserWidget.h"

void APortalHUD::BeginPlay()
{
	Super::BeginPlay();

	check(SignInOverlayClass);
	if (APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		SignInOverlay = CreateWidget<USignInOverlay>(OwningPlayerController, SignInOverlayClass);
		if (SignInOverlay)
		{
			SignInOverlay->AddToViewport();
		}

		OwningPlayerController->SetInputMode(FInputModeGameAndUI());
		OwningPlayerController->SetShowMouseCursor(true);
	}
}
