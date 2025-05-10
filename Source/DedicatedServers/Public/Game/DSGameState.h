// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DSGameState.generated.h"

class ALobbyState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyStateInitialized, ALobbyState*, LobbyState);

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADSGameState : public AGameState
{
	GENERATED_BODY()

public:
	ADSGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_LobbyState)
	TObjectPtr<ALobbyState> LobbyState;

	UPROPERTY(BlueprintAssignable)
	FOnLobbyStateInitialized OnLobbyStateInitialized;

protected:
	virtual void BeginPlay() override;
	void CreateLobbyState();

private:
	UFUNCTION()
	void OnRep_LobbyState();
};
