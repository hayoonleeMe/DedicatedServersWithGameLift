// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MatchGameState.generated.h"

class AMatchPlayerState;

/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API AMatchGameState : public AGameState
{
	GENERATED_BODY()
public:
	AMatchGameState();

	AMatchPlayerState* GetLeader() const;

	void UpdateLeader();
	bool HasFirstBloodBeenHad() const { return bHasFirstBloodBeenHad; }
	bool IsTiedForTheLead(AMatchPlayerState* PlayerState);
protected:
	virtual void BeginPlay() override;
private:

	UPROPERTY()
	TArray<TObjectPtr<AMatchPlayerState>> Leaders;

	UPROPERTY()
	TArray<TObjectPtr<AMatchPlayerState>> SortedPlayerStates;

	bool bHasFirstBloodBeenHad;
};
