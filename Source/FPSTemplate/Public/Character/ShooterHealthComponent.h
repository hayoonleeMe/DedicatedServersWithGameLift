// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShooterHealth_DeathEvent, AActor*, OwningActor, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FShooterHealth_AttributeChanged, UShooterHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * EDeathState
 *
 *	Defines current state of death.
 */
UENUM(BlueprintType)
enum class EDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

/**
 * UShooterHealthComponent
 *
 *	An actor component used to handle anything related to health.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSTEMPLATE_API UShooterHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UShooterHealthComponent();

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Shooter|Health")
	static UShooterHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UShooterHealthComponent>() : nullptr); }

	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "Shooter|Health")
	float GetHealth() const { return Health; };

	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "Shooter|Health")
	float GetMaxHealth() const { return MaxHealth; };

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Shooter|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Shooter|Health")
	EDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Shooter|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EDeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath(AActor* Instigator);

	// Ends the death sequence for the owner.
	virtual void FinishDeath(AActor* Instigator);

public:

	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FShooterHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FShooterHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FShooterHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FShooterHealth_DeathEvent OnDeathFinished;

	// return true if lethal
	virtual bool ChangeHealthByAmount(float Amount, AActor* Instigator);
	virtual void ChangeMaxHealthByAmount(float Amount, AActor* Instigator);	

protected:
	virtual void BeginPlay() override;
	

	virtual void HandleOutOfHealth();

	UFUNCTION()
	virtual void OnRep_DeathState(EDeathState OldDeathState);

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EDeathState DeathState;

	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health;

	UPROPERTY(ReplicatedUsing=OnRep_MaxHealth)
	float MaxHealth;

	UFUNCTION()
	void OnRep_Health(float OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(float OldValue);
		
};
