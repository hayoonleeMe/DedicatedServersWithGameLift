#pragma once

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

class UShooterOverlay;
struct FGameplayTag;
class AWeapon;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

class FPSTEMPLATE_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FName GetWeaponAttachPoint(const FGameplayTag& WeaponType) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetSpecifcPawnMesh(bool WantFirstPerson) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetPawnMesh() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsFirstPerson() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Initiate_CycleWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Notify_CycleWeapon();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Notify_ReloadWeapon();

	UFUNCTION(BlueprintNativeEvent)
	void Initiate_FireWeapon_Pressed();

	UFUNCTION(BlueprintNativeEvent)
	void Initiate_FireWeapon_Released();

	UFUNCTION(BlueprintNativeEvent)
	void Initiate_ReloadWeapon();

	UFUNCTION(BlueprintNativeEvent)
	void Initiate_Aim_Pressed();
	
	UFUNCTION(BlueprintNativeEvent)
	void Initiate_Aim_Released();

	UFUNCTION(BlueprintNativeEvent)
	void Initiate_Crouch();

	UFUNCTION(BlueprintNativeEvent)
	void Initiate_Jump();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool DoDamage(float DamageAmount, AActor* DamageInstigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddAmmo(const FGameplayTag& WeaponType, int32 AmmoAmount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AWeapon* GetCurrentWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetCarriedAmmo();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeWidgets();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDeadOrDying();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void WeaponReplicated();
};
