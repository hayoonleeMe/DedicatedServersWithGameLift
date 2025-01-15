// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShooterAmmoCounter.h"

#include "Character/ShooterCharacter.h"
#include "Combat/CombatComponent.h"
#include "Interfaces/PlayerInterface.h"
#include "Weapon/Weapon.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UShooterAmmoCounter::NativeConstruct()
{
	Super::NativeConstruct();

	Image_WeaponIcon->SetRenderOpacity(0.f);
	Text_Ammo->SetRenderOpacity(0.f);

	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &UShooterAmmoCounter::OnPossessedPawnChanged);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	
	OnPossessedPawnChanged(nullptr, ShooterCharacter);
	
	if (ShooterCharacter->bWeaponFirstReplicated)
	{
		AWeapon* CurrentWeapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(CurrentWeapon))
		{
			OnCarriedAmmoChanged(CurrentWeapon->GetAmmoCounterDynamicMaterialInstance(), IPlayerInterface::Execute_GetCarriedAmmo(ShooterCharacter), CurrentWeapon->Ammo);
		}
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &UShooterAmmoCounter::OnWeaponFirstReplicated);
	}
}

void UShooterAmmoCounter::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	OnCarriedAmmoChanged(Weapon->GetAmmoCounterDynamicMaterialInstance(), IPlayerInterface::Execute_GetCarriedAmmo(GetOwningPlayer()->GetPawn()), Weapon->Ammo);
}

void UShooterAmmoCounter::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UCombatComponent* OldPawnCombat = UCombatComponent::FindCombatComponent(OldPawn);
	if (IsValid(OldPawnCombat))
	{
		OldPawnCombat->OnCarriedAmmoChanged.RemoveDynamic(this, &UShooterAmmoCounter::OnCarriedAmmoChanged);
		OldPawnCombat->OnRoundFired.RemoveDynamic(this, &UShooterAmmoCounter::OnRoundFired);
	}
	UCombatComponent* NewPawnCombat = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(NewPawnCombat))
	{
		Image_WeaponIcon->SetRenderOpacity(1.f);
		Text_Ammo->SetRenderOpacity(1.f);
		NewPawnCombat->OnCarriedAmmoChanged.AddDynamic(this, &UShooterAmmoCounter::OnCarriedAmmoChanged);
		NewPawnCombat->OnRoundFired.AddDynamic(this, &UShooterAmmoCounter::OnRoundFired);
	}
}

void UShooterAmmoCounter::OnCarriedAmmoChanged(UMaterialInstanceDynamic* WeaponIconDynMatInst, int32 InCarriedAmmo, int32 RoundsInWeapon)
{
	CurrentWeaponIcon_DynMatInst = WeaponIconDynMatInst;
	FSlateBrush Brush;
	Brush.SetResourceObject(CurrentWeaponIcon_DynMatInst);
	if (IsValid(Image_WeaponIcon))
	{
		Image_WeaponIcon->SetBrush(Brush);
	}
	TotalAmmo = InCarriedAmmo + RoundsInWeapon;
	if (IsValid(Text_Ammo))
	{
		FText AmmoText = FText::Format(NSLOCTEXT("AmmoText", "AmmoKey", "{0}"), TotalAmmo);
		Text_Ammo->SetText(AmmoText);
	}
}

void UShooterAmmoCounter::OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsCarried)
{
	TotalAmmo = RoundsCarried + RoundsCurrent;
	if (IsValid(Text_Ammo))
	{
		FText AmmoText = FText::Format(NSLOCTEXT("AmmoText", "AmmoKey", "{0}"), TotalAmmo);
		Text_Ammo->SetText(AmmoText);
	}
}

