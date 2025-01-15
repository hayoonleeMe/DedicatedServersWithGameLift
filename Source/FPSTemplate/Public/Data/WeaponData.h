// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "WeaponData.generated.h"

USTRUCT(BlueprintType)
struct FPlayerAnims
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> IdleAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> AimIdleAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> CrouchIdleAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> SprintAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBlendSpace> AimOffset_Hip = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBlendSpace> AimOffset_Aim = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBlendSpace> Strafe_Standing = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBlendSpace> Strafe_Crouching = nullptr;
	
};

USTRUCT(BlueprintType)
struct FMontageData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> EquipMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ReloadMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> FireMontage = nullptr;
};

UCLASS()
class FPSTEMPLATE_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData|FirstPerson")
	TMap<FGameplayTag, FPlayerAnims> FirstPersonAnims;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData|ThirdPerson")
	TMap<FGameplayTag, FPlayerAnims> ThirdPersonAnims;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData|Weapons")
	TMap<FGameplayTag, FMontageData> WeaponMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData|Weapons")
	TMap<FGameplayTag, FName> GripPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData|FirstPerson")
	TMap<FGameplayTag, FMontageData> FirstPersonMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData|ThirdPerson")
	TMap<FGameplayTag, FMontageData> ThirdPersonMontages;
};
