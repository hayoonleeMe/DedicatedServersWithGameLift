// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/PortalHUD.h"
#include "UI/Portal/SignIn/SignInOverlay.h"
#include "Blueprint/UserWidget.h"
#include "UI/Portal/Dashboard/DashboardOverlay.h"

void APortalHUD::OnSignIn()
{
	if (SignInOverlay)
	{
		SignInOverlay->RemoveFromParent();
	}

	check(DashboardOverlayClass);
	if (APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		DashboardOverlay = CreateWidget<UDashboardOverlay>(OwningPlayerController, DashboardOverlayClass);
		if (DashboardOverlay)
		{
			DashboardOverlay->AddToViewport();
		}
	}
}

void APortalHUD::OnSignOut()
{
	if (DashboardOverlay)
	{
		DashboardOverlay->RemoveFromParent();
	}

	if (APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		SignInOverlay = CreateWidget<USignInOverlay>(OwningPlayerController, SignInOverlayClass);
		if (SignInOverlay)
		{
			SignInOverlay->AddToViewport();
		}
	}
}

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
