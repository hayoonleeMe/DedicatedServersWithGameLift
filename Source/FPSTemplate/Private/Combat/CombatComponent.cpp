// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"

#include "Character/ShooterCharacter.h"
#include "Data/WeaponData.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "TimerManager.h"
#include "FPSTemplate/FPSTemplate.h"
#include "Kismet/GameplayStatics.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CarriedAmmo = 0;
	Local_WeaponIndex = 0;
	bTriggerPressed = false;
	bAiming = false;
	TraceLength = 20'000.f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);

	DOREPLIFETIME(UCombatComponent, Inventory);
	DOREPLIFETIME(UCombatComponent, CurrentWeapon);
	DOREPLIFETIME_CONDITION(UCombatComponent, bAiming, COND_SkipOwner);
}

void UCombatComponent::AddAmmo(FGameplayTag WeaponType, int32 AmmoAmount)
{
	if (GetOwner()->HasAuthority() && IsValid(CurrentWeapon))
	{
		int32 NewAmmo = CarriedAmmoMap.FindChecked(WeaponType) + AmmoAmount;
		CarriedAmmoMap[WeaponType] = NewAmmo;
		if (CurrentWeapon->WeaponType.MatchesTagExact(WeaponType))
		{
			CarriedAmmo = NewAmmo;
			if (CurrentWeapon->Ammo == 0 && NewAmmo > 0)
			{
				Server_ReloadWeapon(true);
			}
		}

		OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamicMaterialInstance(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
		OnCarriedAmmoChanged.Broadcast(CurrentWeapon->GetWeaponIconDynamicMaterialInstance(), CarriedAmmo, CurrentWeapon->Ammo);
	}
}

void UCombatComponent::InitializeWeaponWidgets() const
{
	if (IsValid(CurrentWeapon))
	{
		OnReticleChanged.Broadcast(CurrentWeapon->GetReticleDynamicMaterialInstance(), CurrentWeapon->ReticleParams, Local_PlayerHitResult.bHitPlayer);
		OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamicMaterialInstance(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
		OnCarriedAmmoChanged.Broadcast(CurrentWeapon->GetWeaponIconDynamicMaterialInstance(), CarriedAmmo, CurrentWeapon->Ammo);
	}
}

void UCombatComponent::SpawnDefaultInventory()
{
	AActor* OwningActor = GetOwner();
	if (!IsValid(OwningActor)) return;
	if (OwningActor->GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	for (TSubclassOf<AWeapon>& WeaponClass : DefaultInventoryClasses)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = Cast<APawn>(OwningActor);
		SpawnInfo.Owner = OwningActor;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnInfo);
		
		CarriedAmmoMap.Add(NewWeapon->WeaponType, NewWeapon->StartingCarriedAmmo);
		
		AddWeapon(NewWeapon);
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
		CarriedAmmo = CarriedAmmoMap.FindChecked(Inventory[0]->WeaponType);
	}
}

void UCombatComponent::DestroyInventory()
{
	for (AWeapon* Weapon : Inventory)
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}
	}
}

void UCombatComponent::OnRep_CurrentWeapon(AWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
	check(GetOwner());
	check(GetOwner()->Implements<UPlayerInterface>());
	IPlayerInterface::Execute_WeaponReplicated(GetOwner());
}

void UCombatComponent::ServerEquipWeapon_Implementation(AWeapon* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	if (IsValid(CurrentWeapon))
	{
		OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamicMaterialInstance(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
		OnCarriedAmmoChanged.Broadcast(CurrentWeapon->GetWeaponIconDynamicMaterialInstance(), CarriedAmmo, CurrentWeapon->Ammo);
	}
}

void UCombatComponent::SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon)
{
	AWeapon* LocalLastWeapon = nullptr;

	if (IsValid(LastWeapon))
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (IsValid(LocalLastWeapon))
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;
	APawn* OwningPawn = Cast<APawn>(GetOwner());	
	if (IsValid(OwningPawn) && OwningPawn->HasAuthority() && IsValid(CurrentWeapon))
	{
		CarriedAmmo = CarriedAmmoMap.FindChecked(CurrentWeapon->WeaponType);
	}

	// equip new one
	if (IsValid(NewWeapon))
	{
		NewWeapon->SetOwningPawn(Cast<APawn>(GetOwner()));
		NewWeapon->OnEquip(LastWeapon);
	}
	
	if (IsValid(CurrentWeapon) && CurrentWeapon->Ammo == 0 && CarriedAmmo > 0 && IsValid(OwningPawn) && OwningPawn->IsLocallyControlled())
	{
		Local_ReloadWeapon();
		Server_ReloadWeapon();
	}
}

void UCombatComponent::AddWeapon(AWeapon* Weapon)
{
	if (IsValid(Weapon) && IsValid(GetOwner()) && GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		Weapon->OnEnterInventory(Cast<APawn>(GetOwner()));
		Inventory.AddUnique(Weapon);
	}
}

void UCombatComponent::EquipWeapon(AWeapon* Weapon)
{
	if (!IsValid(Weapon) || !IsValid(GetOwner())) return;
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		SetCurrentWeapon(Weapon, CurrentWeapon);
	}
	else
	{
		ServerEquipWeapon(Weapon);
	}
}

void UCombatComponent::Initiate_CycleWeapon()
{
	if (!IsValid(CurrentWeapon)) return;
	if (CurrentWeapon->GetWeaponState() == EWeaponState::Equipping) return;
	if (!IsValid(GetOwner())) return;
	if (GetOwner()->Implements<UPlayerInterface>() && IPlayerInterface::Execute_IsDeadOrDying(GetOwner())) return;
	
	AdvanceWeaponIndex();
	Local_CycleWeapon(Local_WeaponIndex);
	Server_CycleWeapon(Local_WeaponIndex);
}

void UCombatComponent::Notify_CycleWeapon()
{
	if (!IsValid(CurrentWeapon)) return;
	CurrentWeapon->SetWeaponState(EWeaponState::Idle);
	AWeapon* NextWeapon = Inventory[Local_WeaponIndex];
	if (IsValid(NextWeapon))
	{
		EquipWeapon(NextWeapon);
	}
	CurrentWeapon->SetWeaponState(EWeaponState::Equipping);
}

void UCombatComponent::Local_CycleWeapon(const int32 WeaponIndex)
{
	if (!IsValid(GetOwner()) || !GetOwner()->Implements<UPlayerInterface>()) return;
	const AWeapon* NextWeapon = Inventory[WeaponIndex];
	if (!IsValid(NextWeapon) || !IsValid(WeaponData)) return;

	const FMontageData& FirstPersonMontages = WeaponData->FirstPersonMontages.FindChecked(NextWeapon->WeaponType);
	USkeletalMeshComponent* Mesh1P = IPlayerInterface::Execute_GetSpecifcPawnMesh(GetOwner(), true);
	if (IsValid(Mesh1P))
	{
		Mesh1P->GetAnimInstance()->Montage_Play(FirstPersonMontages.EquipMontage);
		Mesh1P->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &UCombatComponent::BlendOut_CycleWeapon);
	}
	
	const FMontageData& ThirdPersonMontages = WeaponData->ThirdPersonMontages.FindChecked(NextWeapon->WeaponType);
	USkeletalMeshComponent* Mesh3P = IPlayerInterface::Execute_GetSpecifcPawnMesh(GetOwner(), false);
	if (IsValid(Mesh3P))
	{
		Mesh3P->GetAnimInstance()->Montage_Play(ThirdPersonMontages.EquipMontage);
	}
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->SetWeaponState(EWeaponState::Equipping);	
	}
}

void UCombatComponent::Multicast_CycleWeapon_Implementation(const int32 WeaponIndex)
{
	if (!IsValid(Cast<APawn>(GetOwner())) || Cast<APawn>(GetOwner())->IsLocallyControlled()) return;
	Local_WeaponIndex = WeaponIndex;
	Local_CycleWeapon(WeaponIndex);
}

void UCombatComponent::Server_CycleWeapon_Implementation(const int32 WeaponIndex)
{
	Local_WeaponIndex = WeaponIndex;
	Multicast_CycleWeapon(WeaponIndex);
}

int32 UCombatComponent::AdvanceWeaponIndex()
{
	if (Inventory.Num() >= 2)
	{
		Local_WeaponIndex = (Local_WeaponIndex + 1) % Inventory.Num();
	}
	return Local_WeaponIndex;
}

void UCombatComponent::BlendOut_CycleWeapon(UAnimMontage* Montage, bool bInterrupted)
{
	UAnimInstance* AnimInstance = IPlayerInterface::Execute_GetSpecifcPawnMesh(GetOwner(), true)->GetAnimInstance();
	if (IsValid(AnimInstance) && AnimInstance->OnMontageBlendingOut.IsAlreadyBound(this, &UCombatComponent::BlendOut_CycleWeapon))
	{
		AnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UCombatComponent::BlendOut_CycleWeapon);
	}
	if (!IsValid(CurrentWeapon)) return;
	CurrentWeapon->SetWeaponState(EWeaponState::Idle);
	
	OnReticleChanged.Broadcast(CurrentWeapon->GetReticleDynamicMaterialInstance(), CurrentWeapon->ReticleParams, Local_PlayerHitResult.bHitPlayer);
	OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamicMaterialInstance(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
	OnCarriedAmmoChanged.Broadcast(CurrentWeapon->GetWeaponIconDynamicMaterialInstance(), CarriedAmmo, CurrentWeapon->Ammo);
	
	if (bTriggerPressed && CurrentWeapon->FireType == EFireType::Auto && CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
}

void UCombatComponent::Initiate_FireWeapon_Pressed()
{
	bTriggerPressed = true;
	if (!IsValid(GetOwner())) return;
	if (GetOwner()->Implements<UPlayerInterface>() && IPlayerInterface::Execute_IsDeadOrDying(GetOwner()))
	{
		bTriggerPressed = false;
		return;
	}

	if (!IsValid(CurrentWeapon)) return;
	if (CurrentWeapon->GetWeaponState() == EWeaponState::Idle && CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
}

void UCombatComponent::Initiate_FireWeapon_Released()
{
	bTriggerPressed = false;
}

void UCombatComponent::Local_FireWeapon()
{
	if (!IsValid(WeaponData) || !IsValid(CurrentWeapon)) return;
	
	UAnimMontage* Montage1P = WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
	USkeletalMeshComponent* Mesh1P = IPlayerInterface::Execute_GetSpecifcPawnMesh(GetOwner(), true);
	if (IsValid(Montage1P) && IsValid(Mesh1P))
	{
		Mesh1P->GetAnimInstance()->Montage_Play(Montage1P);
	}

	UAnimMontage* Montage3P = WeaponData->ThirdPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
	USkeletalMeshComponent* Mesh3P = IPlayerInterface::Execute_GetSpecifcPawnMesh(GetOwner(), false);
	if (IsValid(Montage3P) && IsValid(Mesh3P))
	{
		Mesh3P->GetAnimInstance()->Montage_Play(Montage3P);
	}

	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (IsValid(OwningPawn) && OwningPawn->IsLocallyControlled())
	{
		GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &UCombatComponent::FireTimerFinished, CurrentWeapon->FireTime);

		FHitResult Hit;
		FVector TraceStart = HitScanTrace(5.f, Hit);
		// Tell the weapon the impact point, normal, and physical material
		EPhysicalSurface SurfaceType = Hit.PhysMaterial.IsValid(false) ? Hit.PhysMaterial->SurfaceType.GetValue() : EPhysicalSurface::SurfaceType1;
		CurrentWeapon->Local_Fire(Hit.ImpactPoint, Hit.ImpactNormal, SurfaceType, true);

		// Send the server the hit info.
		const bool bHitPlayer = IsValid(Hit.GetActor()) ? Hit.GetActor()->Implements<UPlayerInterface>() : false;
		const bool bHeadShot = Hit.BoneName == "head";
		OnRoundFired.Broadcast(CurrentWeapon->Ammo, CurrentWeapon->MagCapacity, CarriedAmmo);

		if (GetNetMode() == NM_Standalone) return;
		Server_FireWeapon(TraceStart, Hit, bHitPlayer, bHeadShot);
	}
}

void UCombatComponent::Server_FireWeapon_Implementation(const FVector_NetQuantize& TraceStart, const FHitResult& Impact, bool bScoredHit, bool bHeadShot)
{
	// Do your server-side rewind validation here...
	if (!IsValid(CurrentWeapon) || !IsValid(GetOwner())) return;
	float Damage = bHeadShot ? CurrentWeapon->HeadShotDamage : CurrentWeapon->Damage;

	bool bLethal = false;
	bool bHit = false;
	if (IsValid(Impact.GetActor()) && Impact.GetActor()->Implements<UPlayerInterface>())
	{
		bLethal = IPlayerInterface::Execute_DoDamage(Impact.GetActor(), Damage, GetOwner());
		bHit = true;
	}
	else
	{
		bHit = false;
	}
	OnRoundReported.Broadcast(GetOwner(), Impact.GetActor(), bHit, bHeadShot, bLethal);
	
	if (GetNetMode() != NM_ListenServer || !Cast<APawn>(GetOwner())->IsLocallyControlled())
	{
		// We still need to update ammo server-side for non-hosting player-controlled proxies on a listen server
		CurrentWeapon->Auth_Fire();
	}
	Multicast_FireWeapon(Impact, CurrentWeapon->Ammo);
}

void UCombatComponent::Multicast_FireWeapon_Implementation(const FHitResult& Impact, int32 AuthAmmo)
{
	if (!IsValid(CurrentWeapon) || !IsValid(GetOwner())) return;
	if (Cast<APawn>(GetOwner())->IsLocallyControlled())
	{
		CurrentWeapon->Rep_Fire(AuthAmmo);
	}
	else
	{
		EPhysicalSurface SurfaceType = Impact.PhysMaterial.IsValid(false) ? Impact.PhysMaterial->SurfaceType.GetValue() : SurfaceType1;
		if (IsValid(CurrentWeapon))
		{
			CurrentWeapon->Local_Fire(Impact.ImpactPoint, Impact.ImpactNormal, SurfaceType, false);

			if (IsValid(WeaponData))
			{
				UAnimMontage* Montage1P = WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
				USkeletalMeshComponent* Mesh1P = IPlayerInterface::Execute_GetSpecifcPawnMesh(GetOwner(), true);
				if (IsValid(Mesh1P) && IsValid(Montage1P))
				{
					Mesh1P->GetAnimInstance()->Montage_Play(Montage1P);
				}
				UAnimMontage* Montage3P = WeaponData->ThirdPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
				USkeletalMeshComponent* Mesh3P = IPlayerInterface::Execute_GetSpecifcPawnMesh(GetOwner(), false);
				if (IsValid(Mesh3P) && IsValid(Montage3P))
				{
					Mesh3P->GetAnimInstance()->Montage_Play(Montage3P);
				}

			}
		}
	}
}

void UCombatComponent::FireTimerFinished()
{
	if (!IsValid(GetOwner())) return;
	if (GetOwner()->Implements<UPlayerInterface>() && IPlayerInterface::Execute_IsDeadOrDying(GetOwner()))
	{
		bTriggerPressed = false;
		return;
	}

	if (!IsValid(CurrentWeapon)) return;
	if (CurrentWeapon->Ammo == 0 && CarriedAmmo > 0 && Cast<APawn>(GetOwner())->IsLocallyControlled())
	{
		Local_ReloadWeapon();
		Server_ReloadWeapon();
		return;
	}
	if (CurrentWeapon->FireType == EFireType::Auto && bTriggerPressed && CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
		return;
	}
	CurrentWeapon->SetWeaponState(EWeaponState::Idle);
}

void UCombatComponent::Initiate_ReloadWeapon()
{
	if (!IsValid(CurrentWeapon)) return;
	if (CurrentWeapon->GetWeaponState() == EWeaponState::Equipping || CurrentWeapon->GetWeaponState() == EWeaponState::Reloading) return;
	if (CurrentWeapon->Ammo == CurrentWeapon->MagCapacity || CarriedAmmo == 0) return;
	if (!IsValid(GetOwner())) return;
	if (GetOwner()->Implements<UPlayerInterface>() && IPlayerInterface::Execute_IsDeadOrDying(GetOwner())) return;
	
	Local_ReloadWeapon();
	Server_ReloadWeapon();
}

void UCombatComponent::Local_ReloadWeapon()
{
	if (!IsValid(CurrentWeapon) || !IsValid(GetOwner())) return;
	UAnimMontage* Montage1P = WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->WeaponType).ReloadMontage;
	USkeletalMeshComponent* Mesh1P = IPlayerInterface::Execute_GetSpecifcPawnMesh(GetOwner(), true);
	if (IsValid(Montage1P) && IsValid(Mesh1P))
	{
		Mesh1P->GetAnimInstance()->Montage_Play(Montage1P);
	}
	
	
	UAnimMontage* Montage3P = WeaponData->ThirdPersonMontages.FindChecked(CurrentWeapon->WeaponType).ReloadMontage;
	USkeletalMeshComponent* Mesh3P = IPlayerInterface::Execute_GetSpecifcPawnMesh(GetOwner(), false);
	if (IsValid(Montage3P) && IsValid(Mesh3P))
	{
		Mesh3P->GetAnimInstance()->Montage_Play(Montage3P);
	}
	
	UAnimMontage* WeaponMontage = WeaponData->WeaponMontages.FindChecked(CurrentWeapon->WeaponType).ReloadMontage;
	if (IsValid(CurrentWeapon->GetMesh1P()) && IsValid(CurrentWeapon->GetMesh3P()))
	{
		CurrentWeapon->GetMesh1P()->GetAnimInstance()->Montage_Play(WeaponMontage);
		CurrentWeapon->GetMesh3P()->GetAnimInstance()->Montage_Play(WeaponMontage);
	}

	CurrentWeapon->SetWeaponState(EWeaponState::Reloading);
}

void UCombatComponent::Server_ReloadWeapon_Implementation(bool bLocalOwnerReload)
{
	Multicast_ReloadWeapon(CurrentWeapon->Ammo, CarriedAmmo, bLocalOwnerReload);
}

void UCombatComponent::Multicast_ReloadWeapon_Implementation(int32 NewWeaponAmmo, int32 NewCarriedAmmo, bool bLocalOwnerReload)
{
	Local_ReloadWeapon();
}

void UCombatComponent::Client_ReloadWeapon_Implementation(int32 NewWeaponAmmo, int32 NewCarriedAmmo)
{
	if (!IsValid(GetOwner()) || !IsValid(CurrentWeapon)) return;
	if (Cast<APawn>(GetOwner())->IsLocallyControlled())
	{
		CurrentWeapon->Ammo = NewWeaponAmmo;
		CarriedAmmo = NewCarriedAmmo;

		OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamicMaterialInstance(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
		OnCarriedAmmoChanged.Broadcast(CurrentWeapon->GetWeaponIconDynamicMaterialInstance(), CarriedAmmo, CurrentWeapon->Ammo);
	}
}

void UCombatComponent::Notify_ReloadWeapon()
{
	if (!IsValid(CurrentWeapon)) return;
	if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_Standalone)
	{
		int32 EmptySpace = CurrentWeapon->MagCapacity - CurrentWeapon->Ammo;
		int32 AmountToRefill = FMath::Min(EmptySpace, CarriedAmmo);
		CurrentWeapon->Ammo += AmountToRefill;
		CarriedAmmoMap[CurrentWeapon->WeaponType] = CarriedAmmoMap[CurrentWeapon->WeaponType] - AmountToRefill;
		CarriedAmmo = CarriedAmmoMap[CurrentWeapon->WeaponType];
		Client_ReloadWeapon(CurrentWeapon->Ammo, CarriedAmmo);
	}
	CurrentWeapon->SetWeaponState(EWeaponState::Idle);
	if (bTriggerPressed && CurrentWeapon->FireType == EFireType::Auto && CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
}

void UCombatComponent::Initiate_AimPressed()
{
	if (!IsValid(GetOwner())) return;
	if (GetOwner()->Implements<UPlayerInterface>() && IPlayerInterface::Execute_IsDeadOrDying(GetOwner())) return;
	
	Local_Aim(true);
	Server_Aim(true);
}

void UCombatComponent::Server_Aim_Implementation(bool bPressed)
{
	Local_Aim(bPressed);
}

void UCombatComponent::Initiate_AimReleased()
{
	Local_Aim(false);
	Server_Aim(false);
}

void UCombatComponent::Local_Aim(bool bPressed)
{
	bAiming = bPressed;
	OnAimingStatusChanged.Broadcast(bAiming);
	OnAim(bPressed);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!IsValid(OwningPawn) || !OwningPawn->IsLocallyControlled()) return;

	if (APlayerController* PC = Cast<APlayerController>(OwningPawn->GetController()); IsValid(PC))
	{
		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}
		FVector2D ReticleLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		FVector ReticleWorldLocation;
		FVector ReticleWorlDirection;
		UGameplayStatics::DeprojectScreenToWorld(PC, ReticleLocation, ReticleWorldLocation, ReticleWorlDirection);
		
		Local_PlayerHitResult.Start = ReticleWorldLocation;
		Local_PlayerHitResult.End = Local_PlayerHitResult.Start + ReticleWorlDirection * TraceLength;
		Local_PlayerHitResult.bHitPlayerLastFrame = Local_PlayerHitResult.bHitPlayer;

		FHitResult TraceHit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());
		for (AWeapon* Weapon : Inventory)
		{
			QueryParams.AddIgnoredActor(Weapon);
		}
		FCollisionResponseParams ResponseParams;
		ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
		ResponseParams.CollisionResponse.SetResponse(ECC_WorldStatic, ECR_Block);
		ResponseParams.CollisionResponse.SetResponse(ECC_WorldDynamic, ECR_Block);
		ResponseParams.CollisionResponse.SetResponse(ECC_PhysicsBody, ECR_Block);
		ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
			
		GetWorld()->LineTraceSingleByChannel(TraceHit, Local_PlayerHitResult.Start, Local_PlayerHitResult.End, ECC_Weapon, QueryParams, ResponseParams);
			
		Local_PlayerHitResult.End = TraceHit.bBlockingHit ? TraceHit.ImpactPoint : Local_PlayerHitResult.End;
		if (IsValid(TraceHit.GetActor()) && TraceHit.GetActor()->Implements<UPlayerInterface>())
		{
			Local_PlayerHitResult.bHitPlayer = true;
		}
		else
		{
			Local_PlayerHitResult.bHitPlayer = false;
		}
		
		Local_PlayerHitResult.bHeadShot = TraceHit.bBlockingHit && TraceHit.BoneName == "head";
			
		if (Local_PlayerHitResult.bHitPlayer != Local_PlayerHitResult.bHitPlayerLastFrame)
		{
			OnTargetingPlayerStatusChanged.Broadcast(Local_PlayerHitResult.bHitPlayer);
		}
	}
}

FVector UCombatComponent::HitScanTrace(float SweepRadius, FHitResult& OutHit)
{
	FVector Start = GetOwner()->GetActorLocation();
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());
	for (AWeapon* Weapon : Inventory)
	{
		QueryParams.AddIgnoredActor(Weapon);
	}
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldStatic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldDynamic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_PhysicsBody, ECR_Block);

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	if (APlayerController* PC = Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController()); IsValid(PC))
	{
		FVector2D ReticleLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		FVector ReticleWorldLocation;
		FVector ReticleWorlDirection;
		UGameplayStatics::DeprojectScreenToWorld(PC, ReticleLocation, ReticleWorldLocation, ReticleWorlDirection);

		Start = ReticleWorldLocation;
		FVector End = Start + ReticleWorlDirection * TraceLength;

		const bool bHit = GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, ECC_Weapon, FCollisionShape::MakeSphere(SweepRadius), QueryParams, ResponseParams);
		if (!bHit)
		{
			OutHit.ImpactPoint = End;
			OutHit.ImpactNormal = (Start - End).GetSafeNormal();
		}
	}
	return Start;
}



