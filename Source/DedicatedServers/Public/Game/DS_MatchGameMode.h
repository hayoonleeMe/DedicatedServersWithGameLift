// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DS_GameModeBase.h"
#include "DS_MatchGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADS_MatchGameMode : public ADS_GameModeBase
{
	GENERATED_BODY()

public:
	ADS_MatchGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY()
	EMatchStatus MatchStatus;

protected:
	virtual void OnCountdownTimerFinished(ECountdownTimerType Type) override;
	
	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle PreMatchTimer;
	
	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle MatchTimer;

	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle PostMatchTimer;
};
