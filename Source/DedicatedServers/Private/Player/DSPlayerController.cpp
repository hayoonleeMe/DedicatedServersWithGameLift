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

void ADSPlayerController::ClientTimerUpdated_Implementation(float CountdownTimeLeft, ECountdownTimerType Type) const
{
	OnTimerUpdated.Broadcast(CountdownTimeLeft - SingleTripTime, Type);
}

void ADSPlayerController::ClientTimerStopped_Implementation(float CountdownTimeLeft, ECountdownTimerType Type) const
{
	OnTimerStopped.Broadcast(CountdownTimeLeft - SingleTripTime, Type);
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
