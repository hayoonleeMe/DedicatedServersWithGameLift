// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API AShooterHUD : public AHUD
{
	GENERATED_BODY()
public:
	UUserWidget* GetShooterOverlay() {return Overlay;}

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Overlay")
	TSubclassOf<UUserWidget> ShooterOverlayClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> Overlay;
};
