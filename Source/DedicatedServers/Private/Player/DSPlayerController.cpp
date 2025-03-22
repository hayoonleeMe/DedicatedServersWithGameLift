// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSPlayerController.h"

void ADSPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (GetNetMode() == NM_Standalone)
	{
		return;
	}
	
	if (IsLocalController())
	{
		ServerPing(GetWorld()->GetTimeSeconds());
	}
}

void ADSPlayerController::ServerPing_Implementation(float TimeOfRequest)
{
	ClientPong(TimeOfRequest);	
}

void ADSPlayerController::ClientPong_Implementation(float TimeOfRequest)
{
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfRequest;
	SingleTripTime = RoundTripTime * 0.5f;
}
