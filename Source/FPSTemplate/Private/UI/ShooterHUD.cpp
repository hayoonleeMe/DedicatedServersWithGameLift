// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShooterHUD.h"

#include "Blueprint/UserWidget.h"


void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetOwningPlayerController();
	if (IsValid(PlayerController) && ShooterOverlayClass)
	{
		Overlay = CreateWidget<UUserWidget>(PlayerController, ShooterOverlayClass);
		Overlay->AddToViewport();
	}
}
