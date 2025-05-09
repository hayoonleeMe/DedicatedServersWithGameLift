// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "Interfaces/PlayerInterface.h"
#include "ShooterTypes/ShooterTypes.h"
#include "ShooterCharacter.generated.h"

class UEliminationComponent;
class UShooterHealthComponent;
class UShooterOverlay;
class UShooterHUDComponent;
class USpringArmComponent;
class UCameraComponent;
class UWeaponData;
class AWeapon;
class UCombatComponent;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponFirstReplicated, AWeapon*, Weapon);

UCLASS()
class FPSTEMPLATE_API AShooterCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnRep_PlayerState() override;
	virtual void BeginDestroy() override;
	virtual void UnPossessed() override;

	FRotator StartingAimRotation;

	UPROPERTY(BlueprintReadOnly)
	float AO_Yaw;
	
	float InterpAO_Yaw;

	UPROPERTY(BlueprintReadOnly)
	float MovementOffsetYaw;

	UPROPERTY(BlueprintReadOnly)
	FTransform FABRIK_SocketTransform;

	UPROPERTY(BlueprintReadOnly)
	ETurningInPlace TurningStatus;

	void TurnInPlace(float DeltaTime);

	/** PlayerInterface */
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const override;
	virtual USkeletalMeshComponent* GetSpecifcPawnMesh_Implementation(bool WantFirstPerson) const override;
	virtual USkeletalMeshComponent* GetPawnMesh_Implementation() const override;
	virtual bool IsFirstPerson_Implementation() const override;
	virtual bool DoDamage_Implementation(float DamageAmount, AActor* DamageInstigator) override;
	virtual void AddAmmo_Implementation(const FGameplayTag& WeaponType, int32 AmmoAmount) override;
	virtual AWeapon* GetCurrentWeapon_Implementation() override;
	virtual int32 GetCarriedAmmo_Implementation() override;
	virtual void InitializeWidgets_Implementation() override;
	virtual void Notify_CycleWeapon_Implementation() override;
	virtual void Notify_ReloadWeapon_Implementation() override;
	virtual void Initiate_Crouch_Implementation() override;
	virtual void Initiate_Jump_Implementation() override;
	virtual bool IsDeadOrDying_Implementation() override;
	virtual void WeaponReplicated_Implementation() override;
	virtual void EnableGameActions_Implementation(bool bEnable) override;
	/** <end> PlayerInterface */

	UFUNCTION(BlueprintCallable)
	FRotator GetFixedAimRotation() const;

	bool bWeaponFirstReplicated;

	UPROPERTY(BlueprintAssignable)
	FWeaponFirstReplicated OnWeaponFirstReplicated;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aiming")
	float DefaultFieldOfView;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact")
	TArray<TObjectPtr<UAnimMontage>> HitReacts;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnAim(bool bIsAiming);
	
	UFUNCTION(BlueprintImplementableEvent)
	void DeathEffects(AActor* DeathInstigator, UAnimMontage* DeathMontage);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HitReact(int32 MontageIndex);
	
private:
	void Input_CycleWeapon();
	void Input_FireWeapon_Pressed();
	void Input_FireWeapon_Released();
	void Input_ReloadWeapon();
	void Input_Aim_Pressed();
	void Input_Aim_Released();
	bool IsLocalFirstPerson() const;
	
	UFUNCTION()
	void OnDeathStarted(AActor* DyingActor, AActor* DeathInstigator);
	
	/** 1st person view */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShooterHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEliminationComponent> EliminationComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CycleWeaponAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> FireWeaponAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ReloadWeaponAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> AimAction;
	
	FTimerHandle InitiializeWidgets_Timer;

	bool bEnableGameActions;
};

