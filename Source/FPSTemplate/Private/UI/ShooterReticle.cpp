// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShooterReticle.h"

#include "Character/ShooterCharacter.h"
#include "Components/Image.h"
#include "Combat/CombatComponent.h"
#include "Weapon/Weapon.h"
#include "Interfaces/PlayerInterface.h"

namespace Reticle
{
	const FName Inner_RGBA = FName("Inner_RGBA");
	const FName RoundedCornerScale = FName("RoundedCornerScale");
	const FName ShapeCutThickness = FName("ShapeCutThickness");
}

namespace Ammo
{
	const FName Rounds_Current = FName("Rounds_Current");
	const FName Rounds_Max = FName("Rounds_Max");
}

void UShooterReticle::NativeConstruct()
{
	Super::NativeConstruct();

	Image_Reticle->SetRenderOpacity(0.f);
	Image_AmmoCounter->SetRenderOpacity(0.f);

	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &UShooterReticle::OnPossessedPawnChanged);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	
	OnPossessedPawnChanged(nullptr, ShooterCharacter);
	
	if (ShooterCharacter->bWeaponFirstReplicated)
	{
		AWeapon* CurrentWeapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(CurrentWeapon))
		{
			OnReticleChange(CurrentWeapon->GetReticleDynamicMaterialInstance(), CurrentWeapon->ReticleParams);
			OnAmmoCounterChange(CurrentWeapon->GetAmmoCounterDynamicMaterialInstance(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
		}
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &UShooterReticle::OnWeaponFirstReplicated);
	}
}

void UShooterReticle::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	OnReticleChange(Weapon->GetReticleDynamicMaterialInstance(), Weapon->ReticleParams);
	OnAmmoCounterChange(Weapon->GetAmmoCounterDynamicMaterialInstance(), Weapon->Ammo, Weapon->MagCapacity);
}

void UShooterReticle::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UCombatComponent* OldPawnCombat = UCombatComponent::FindCombatComponent(OldPawn);
	if (IsValid(OldPawnCombat))
	{
		OldPawnCombat->OnReticleChanged.RemoveDynamic(this, &UShooterReticle::OnReticleChange);
		OldPawnCombat->OnAmmoCounterChanged.RemoveDynamic(this, &UShooterReticle::OnAmmoCounterChange);
		OldPawnCombat->OnTargetingPlayerStatusChanged.RemoveDynamic(this, &UShooterReticle::OnTargetingPlayerStatusChanged);
		OldPawnCombat->OnRoundFired.RemoveDynamic(this, &UShooterReticle::OnRoundFired);
		OldPawnCombat->OnAimingStatusChanged.RemoveDynamic(this, &UShooterReticle::OnAimingStatusChanged);
	}
	UCombatComponent* NewPawnCombat = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(NewPawnCombat))
	{
		Image_Reticle->SetRenderOpacity(1.f);
		Image_AmmoCounter->SetRenderOpacity(1.f);
		NewPawnCombat->OnReticleChanged.AddDynamic(this, &UShooterReticle::OnReticleChange);
		NewPawnCombat->OnAmmoCounterChanged.AddDynamic(this, &UShooterReticle::OnAmmoCounterChange);
		NewPawnCombat->OnTargetingPlayerStatusChanged.AddDynamic(this, &UShooterReticle::OnTargetingPlayerStatusChanged);
		NewPawnCombat->OnRoundFired.AddDynamic(this, &UShooterReticle::OnRoundFired);
		NewPawnCombat->OnAimingStatusChanged.AddDynamic(this, &UShooterReticle::OnAimingStatusChanged);
	}
}

void UShooterReticle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	_BaseCornerScaleFactor_TargetingPlayer = FMath::FInterpTo(_BaseCornerScaleFactor_TargetingPlayer, bTargetingPlayer ? CurrentReticleParams.ScaleFactor_Targeting : CurrentReticleParams.ScaleFactor_NotTargeting, InDeltaTime, CurrentReticleParams.TargetingPlayerInterpSpeed);
	_BaseCornerScaleFactor_Aiming = FMath::FInterpTo(_BaseCornerScaleFactor_Aiming, bAiming ? CurrentReticleParams.ScaleFactor_Aiming : CurrentReticleParams.ScaleFactor_NotAiming, InDeltaTime, CurrentReticleParams.AimingInterpSpeed);
	_BaseCornerScaleFactor_RoundFired = FMath::FInterpTo(_BaseCornerScaleFactor_RoundFired, 0.f, InDeltaTime, CurrentReticleParams.RoundFiredInterpSpeed);

	_BaseShapeCutFactor_Aiming = FMath::FInterpTo(BaseShapeCutFactor, bAiming ? CurrentReticleParams.ShapeCutFactor_Aiming : CurrentReticleParams.ShapeCutFactor_NotAiming, InDeltaTime, CurrentReticleParams.AimingInterpSpeed);
	_BaseShapeCutFactor_RoundFired = FMath::FInterpTo(_BaseShapeCutFactor_RoundFired, 0.f, InDeltaTime, CurrentReticleParams.RoundFiredInterpSpeed);

	BaseCornerScaleFactor = _BaseCornerScaleFactor_TargetingPlayer + _BaseCornerScaleFactor_Aiming + _BaseCornerScaleFactor_RoundFired;
	BaseShapeCutFactor = _BaseShapeCutFactor_Aiming + _BaseShapeCutFactor_RoundFired;
	if (IsValid(CurrentReticle_DynMatInst))
	{
		CurrentReticle_DynMatInst->SetScalarParameterValue(Reticle::RoundedCornerScale, BaseCornerScaleFactor);
		CurrentReticle_DynMatInst->SetScalarParameterValue(Reticle::ShapeCutThickness, BaseShapeCutFactor);
	}
}

void UShooterReticle::OnReticleChange(UMaterialInstanceDynamic* ReticleDynMatInst, const FReticleParams& ReticleParams,
	bool bCurrentlyTargetingPlayer)
{
	CurrentReticleParams = ReticleParams;
	
	CurrentReticle_DynMatInst = ReticleDynMatInst;
	FSlateBrush Brush;
	Brush.SetResourceObject(ReticleDynMatInst);
	if (IsValid(Image_Reticle))
	{
		Image_Reticle->SetBrush(Brush);
	}
	
	OnTargetingPlayerStatusChanged(bCurrentlyTargetingPlayer);
}

void UShooterReticle::OnAmmoCounterChange(UMaterialInstanceDynamic* AmmoCounterDynMatInst, int32 RoundsCurrent,
	int32 RoundsMax)
{
	CurrentAmmoCounter_DynMatInst = AmmoCounterDynMatInst;
	CurrentAmmoCounter_DynMatInst->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
	CurrentAmmoCounter_DynMatInst->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	FSlateBrush Brush;
	Brush.SetResourceObject(CurrentAmmoCounter_DynMatInst);

	if (IsValid(Image_AmmoCounter))
	{
		Image_AmmoCounter->SetBrush(Brush);
	}
}

void UShooterReticle::OnTargetingPlayerStatusChanged(bool bIsTargetingPlayer)
{
	if (IsValid(CurrentReticle_DynMatInst))
	{
		FLinearColor ReticleColor = bIsTargetingPlayer ? FLinearColor::Red : FLinearColor::White;
		CurrentReticle_DynMatInst->SetVectorParameterValue(Reticle::Inner_RGBA, ReticleColor);
	}
	bTargetingPlayer = bIsTargetingPlayer;
}

void UShooterReticle::OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsCarried)
{
	_BaseCornerScaleFactor_RoundFired += CurrentReticleParams.ScaleFactor_RoundFired;
	_BaseShapeCutFactor_RoundFired += CurrentReticleParams.ShapeCutFactor_RoundFired;

	if (IsValid(CurrentAmmoCounter_DynMatInst))
	{
		CurrentAmmoCounter_DynMatInst->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
		CurrentAmmoCounter_DynMatInst->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	}
}

void UShooterReticle::OnAimingStatusChanged(bool bIsAiming)
{
	bAiming = bIsAiming;
}

