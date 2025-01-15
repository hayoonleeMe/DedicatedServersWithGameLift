// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameLiftServerSDK.h"
#include "ShooterGameModeBase.h"
#include "ShooterGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogShooterGameMode, Log, All);

/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API AShooterGameMode : public AShooterGameModeBase
{
	GENERATED_BODY()
	
public:
	AShooterGameMode();
	
protected:
	virtual void BeginPlay() override;

private:
	FProcessParameters ProcessParameters;

	void InitGameLift();
	void SetServerParameters(FServerParameters& OutServerParameters);
};
