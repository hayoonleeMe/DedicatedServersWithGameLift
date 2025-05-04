// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DS_MatchGameMode.h"
#include "ShooterGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API AShooterGameModeBase : public ADS_MatchGameMode
{
	GENERATED_BODY()
public:
	AShooterGameModeBase();
	virtual void Tick(float DeltaTime) override;
	virtual void StartPlayerElimination(float ElimTime, ACharacter* ElimmedCharacter, class APlayerController* VictimController, APlayerController* AttackerController);
	
	UPROPERTY()
	TMap<APlayerController*, FTimerHandle> Timers;
	
	virtual void PlayerEliminated(ACharacter* ElimmedCharacter, class APlayerController* VictimController, APlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);


	UPROPERTY(EditDefaultsOnly, Category="Respawning")
	float RespawnTime;
};
