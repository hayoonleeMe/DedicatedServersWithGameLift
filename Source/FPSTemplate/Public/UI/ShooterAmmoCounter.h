// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterAmmoCounter.generated.h"

class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API UShooterAmmoCounter : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_WeaponIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Ammo;



	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CurrentWeaponIcon_DynMatInst;

private:
	
	int32 TotalAmmo;
	
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	void OnCarriedAmmoChanged(UMaterialInstanceDynamic* WeaponIconDynMatInst, int32 InCarriedAmmo, int32 RoundsInWeapon);

	UFUNCTION()
	void OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsCarried);

	UFUNCTION()
	void OnWeaponFirstReplicated(AWeapon* Weapon);
};
