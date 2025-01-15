// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterTypes/ShooterTypes.h"
#include "ShooterReticle.generated.h"

namespace Reticle
{
	extern FPSTEMPLATE_API const FName Inner_RGBA;
	extern FPSTEMPLATE_API const FName RoundedCornerScale;
	extern FPSTEMPLATE_API const FName ShapeCutThickness;
}

namespace Ammo
{
	extern FPSTEMPLATE_API const FName Rounds_Current;
	extern FPSTEMPLATE_API const FName Rounds_Max;
}

class UImage;
class AWeapon;

/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API UShooterReticle : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Reticle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_AmmoCounter;


private:

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CurrentReticle_DynMatInst;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CurrentAmmoCounter_DynMatInst;

	FReticleParams CurrentReticleParams;
	float BaseCornerScaleFactor;
	float _BaseCornerScaleFactor_TargetingPlayer;
	float _BaseCornerScaleFactor_Aiming;
	float _BaseCornerScaleFactor_RoundFired;	
	float BaseShapeCutFactor;
	float _BaseShapeCutFactor_Aiming;
	float _BaseShapeCutFactor_RoundFired;
	bool bTargetingPlayer;
	bool bAiming;

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	void OnReticleChange(UMaterialInstanceDynamic* ReticleDynMatInst, const FReticleParams& ReticleParams, bool bCurrentlyTargetingPlayer = false);

	UFUNCTION()
	void OnAmmoCounterChange(UMaterialInstanceDynamic* AmmoCounterDynMatInst, int32 RoundsCurrent, int32 RoundsMax);

	UFUNCTION()
	void OnTargetingPlayerStatusChanged(bool bIsTargetingPlayer);

	UFUNCTION()
	void OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsCarried);

	UFUNCTION()
	void OnAimingStatusChanged(bool bIsAiming);

	UFUNCTION()
	void OnWeaponFirstReplicated(AWeapon* Weapon);
};


