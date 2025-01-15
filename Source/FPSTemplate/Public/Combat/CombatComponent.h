// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ShooterTypes/ShooterTypes.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReticleChangedDelegate, UMaterialInstanceDynamic*, ReticleDynMatInst, const FReticleParams&, ReticleParams, bool, bCurrentlyTargetingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoCounterChangedDelegate, UMaterialInstanceDynamic*, AmmoCounterDynMatInst, int32, RoundsCurrent, int32, RoundsMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetingPlayerStatusChangedDelegate, bool, bIsTargetingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRoundFiredDelegate, int32, RoundsCurrent, int32, RoundsMax, int32, RoundsCarried);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimingStatusChangedDelegate, bool, bIsAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCarriedAmmoChangedDelegate, UMaterialInstanceDynamic*, WeaponIconDynMatInst, int32, InCarriedAmmo, int32, RoundsInWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FRoundReportedDelegate, AActor*, Attacker, AActor*, Victim, bool, bHit, bool, bHeadShot, bool, bLethal);

class UShooterOverlay;
class UWeaponData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSTEMPLATE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	void Initiate_CycleWeapon();
	void Notify_CycleWeapon();
	void Notify_ReloadWeapon();
	void Initiate_FireWeapon_Pressed();
	void Initiate_FireWeapon_Released();
	void Initiate_ReloadWeapon();
	void Initiate_AimPressed();
	void Initiate_AimReleased();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAim(bool bIsAiming);	
	
	UFUNCTION(BlueprintPure, Category = "Shooter|Health")
	static UCombatComponent* FindCombatComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UCombatComponent>() : nullptr); }
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void AddAmmo(FGameplayTag WeaponType, int32 AmmoAmount);
	void InitializeWeaponWidgets() const;
	void SpawnDefaultInventory();
	void DestroyInventory();
	
	UFUNCTION(Reliable, Server)
	void ServerEquipWeapon(AWeapon* NewWeapon);

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming;

	UPROPERTY(BlueprintAssignable, Category=Combat)
	FReticleChangedDelegate OnReticleChanged;

	UPROPERTY(BlueprintAssignable, Category=Combat)
	FAmmoCounterChangedDelegate OnAmmoCounterChanged;

	UPROPERTY(BlueprintAssignable, Category=Combat)
	FTargetingPlayerStatusChangedDelegate OnTargetingPlayerStatusChanged;

	UPROPERTY(BlueprintAssignable, Category=Combat)
	FRoundFiredDelegate OnRoundFired;

	UPROPERTY(BlueprintAssignable, Category=Combat)
	FAimingStatusChangedDelegate OnAimingStatusChanged;

	UPROPERTY(BlueprintAssignable, Category=Combat)
	FCarriedAmmoChangedDelegate OnCarriedAmmoChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UWeaponData> WeaponData;
	
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	TMap<FGameplayTag, int32> CarriedAmmoMap;
	
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<AWeapon>> DefaultInventoryClasses;
	
	UPROPERTY(Transient, Replicated)
	TArray<AWeapon*> Inventory;
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon, BlueprintReadOnly)
	TObjectPtr<AWeapon> CurrentWeapon;
	
	UPROPERTY(EditDefaultsOnly)
	float TraceLength;
	
	UPROPERTY(BlueprintReadOnly)
	FPlayerHitResult Local_PlayerHitResult;

	UPROPERTY(BlueprintAssignable)
	FRoundReportedDelegate OnRoundReported;

protected:
	virtual void BeginPlay() override;

private:
	void SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon = nullptr);
	void AddWeapon(AWeapon* Weapon);
	void EquipWeapon(AWeapon* Weapon);
	void Local_CycleWeapon(const int32 WeaponIndex);
	void FireTimerFinished();	
	int32 AdvanceWeaponIndex();
	void Local_ReloadWeapon();
	void Local_Aim(bool bPressed);
	FVector HitScanTrace(float SweepRadius, FHitResult& OutHit);
	
	UFUNCTION(Server, Reliable)
	void Server_CycleWeapon(const int32 WeaponIndex);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CycleWeapon(const int32 WeaponIndex);

	UFUNCTION()
	void BlendOut_CycleWeapon(UAnimMontage* Montage, bool bInterrupted);

	void Local_FireWeapon();

	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FVector_NetQuantize& TraceStart, const FHitResult& Impact, bool bScoredHit, bool bHeadShot);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(const FHitResult& Impact, int32 AuthAmmo);

	UFUNCTION(Server, Reliable)
	void Server_ReloadWeapon(bool bLocalOwnerReload = false);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ReloadWeapon(int32 NewWeaponAmmo, int32 NewCarriedAmmo, bool bLocalOwnerReload = false);

	UFUNCTION(Client, Reliable)
	void Client_ReloadWeapon(int32 NewWeaponAmmo, int32 NewCarriedAmmo);

	UFUNCTION(Server, Reliable)
	void Server_Aim(bool bPressed);

	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* LastWeapon);

	UFUNCTION()
	void OnRep_CarriedAmmo();
	
	int32 Local_WeaponIndex;	
	bool bTriggerPressed;
	FTimerHandle FireTimer;

};
