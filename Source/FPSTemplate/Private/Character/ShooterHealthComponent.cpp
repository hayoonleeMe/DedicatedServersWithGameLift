// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterHealthComponent.h"
#include "Net/UnrealNetwork.h"

UShooterHealthComponent::UShooterHealthComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	Health = 100.f;
	MaxHealth = 100.f;
	DeathState = EDeathState::NotDead;
}

void UShooterHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UShooterHealthComponent, DeathState);
	DOREPLIFETIME_CONDITION(UShooterHealthComponent, Health, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UShooterHealthComponent, MaxHealth, COND_OwnerOnly);
}

float UShooterHealthComponent::GetHealthNormalized() const
{
	return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
}

void UShooterHealthComponent::StartDeath(AActor* Instigator)
{
	if (DeathState != EDeathState::NotDead)
	{
		return;
	}

	DeathState = EDeathState::DeathStarted;

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner, Instigator);

	Owner->ForceNetUpdate();
}

void UShooterHealthComponent::FinishDeath(AActor* Instigator)
{
	if (DeathState != EDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EDeathState::DeathFinished;

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner, Instigator);

	Owner->ForceNetUpdate();
}

void UShooterHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

bool UShooterHealthComponent::ChangeHealthByAmount(float Amount, AActor* Instigator)
{
	float OldValue = Health;
	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(this, OldValue, Health, Instigator);
	if (Health <= 0.f)
	{
		StartDeath(Instigator);
		return true;
	}
	return false;
}

void UShooterHealthComponent::ChangeMaxHealthByAmount(float Amount, AActor* Instigator)
{
	float OldValue = MaxHealth;
	MaxHealth += Amount;
	OnHealthChanged.Broadcast(this, OldValue, MaxHealth, Instigator);
}

void UShooterHealthComponent::HandleOutOfHealth()
{
	
}

void UShooterHealthComponent::OnRep_DeathState(EDeathState OldDeathState)
{
	const EDeathState NewDeathState = DeathState;
	
	// Revert the death state for now since we rely on StartDeath and FinishDeath to change it.
	DeathState = OldDeathState;

	if (OldDeathState > NewDeathState)
	{
		// The server is trying to set us back but we've already predicted past the server state.
		UE_LOG(LogTemp, Warning, TEXT("HealthComponent: Predicted past server death state [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		return;
	}

	if (OldDeathState == EDeathState::NotDead)
	{
		if (NewDeathState == EDeathState::DeathStarted)
		{
			StartDeath(nullptr);
		}
		else if (NewDeathState == EDeathState::DeathFinished)
		{
			StartDeath(nullptr);
			FinishDeath(nullptr);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}
	else if (OldDeathState == EDeathState::DeathStarted)
	{
		if (NewDeathState == EDeathState::DeathFinished)
		{
			FinishDeath(nullptr);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}
}

void UShooterHealthComponent::OnRep_Health(float OldValue)
{
	OnHealthChanged.Broadcast(this, OldValue, Health, nullptr);
}

void UShooterHealthComponent::OnRep_MaxHealth(float OldValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, MaxHealth, nullptr);
}

