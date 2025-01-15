// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "ShooterTypes/ShooterTypes.h"
#include "Weapon.generated.h"

struct FPlayerHitResult;

/** Enum used to describe what state the weapon is currently in. */
UENUM(BlueprintType)
namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
	};
}

UENUM(BlueprintType)
namespace EFireType
{
	enum Type
	{
		Auto,
		SemiAuto
	};
}

UCLASS()
class FPSTEMPLATE_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void OnRep_Instigator() override;

	/** perform initial setup */
	virtual void PostInitializeComponents() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag WeaponType;

	//////////////////////////////////////////////////////////////////////////
	// Firing the Weapon

	void Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> SurfaceType, bool bIsFirstPerson);
	int32 Auth_Fire();
	void Rep_Fire(int32 AuthAmmo);

	UFUNCTION(BlueprintImplementableEvent)
	void FireEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, EPhysicalSurface SurfaceType, bool bIsFirstPerson);

	//////////////////////////////////////////////////////////////////////////
	// Inventory
	
	/** [server] weapon was added to pawn's inventory */
	virtual void OnEnterInventory(APawn* NewOwner);

	/** weapon is holstered by owner pawn */
	virtual void OnUnEquip();

	/** weapon is being equipped by owner pawn */
	virtual void OnEquip(const AWeapon* LastWeapon);

	/** set the weapon's owning pawn */
	void SetOwningPawn(APawn* NewOwningPawn);

	/** get weapon mesh (needs pawn owner to determine variant) */
	USkeletalMeshComponent* GetWeaponMesh() const;
	
	/** update weapon state */
	void SetWeaponState(EWeaponState::Type NewState);

	EWeaponState::Type GetWeaponState() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MagCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Ammo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Sequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StartingCarriedAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimFieldOfView;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HeadShotDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EFireType::Type> FireType;

	USkeletalMeshComponent* GetMesh1P() const;
	USkeletalMeshComponent* GetMesh3P() const;
	
	UMaterialInstanceDynamic* GetReticleDynamicMaterialInstance();
	UMaterialInstanceDynamic* GetAmmoCounterDynamicMaterialInstance();
	UMaterialInstanceDynamic* GetWeaponIconDynamicMaterialInstance();

	UPROPERTY(EditDefaultsOnly, Category=Reticle)
	FReticleParams ReticleParams;

protected:
	virtual void BeginPlay() override;
	
	//////////////////////////////////////////////////////////////////////////
	// Inventory

	/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn();

	/** detaches weapon mesh from pawn */
	void DetachMeshFromPawn();

private:
	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh3P;

	/** current weapon state */
	EWeaponState::Type CurrentState;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	TObjectPtr<UMaterialInterface> ReticleMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynMatInst_Reticle;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	TObjectPtr<UMaterialInterface> AmmoCounterMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynMatInst_AmmoCounter;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	TObjectPtr<UMaterialInterface> WeaponIconMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynMatInst_WeaponIcon;
	
};
