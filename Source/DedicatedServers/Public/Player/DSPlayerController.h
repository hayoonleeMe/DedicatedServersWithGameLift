// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void ReceivedPlayer() override;

protected:
	UFUNCTION(Server, Reliable)
	void ServerPing(float TimeOfRequest);

	UFUNCTION(Client, Reliable)
	void ClientPong(float TimeOfRequest);

private:
	float SingleTripTime = 0.f;
};
