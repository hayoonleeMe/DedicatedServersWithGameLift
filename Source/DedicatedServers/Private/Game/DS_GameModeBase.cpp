// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DS_GameModeBase.h"

#include "Player/DSPlayerController.h"

void ADS_GameModeBase::StartCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle)
{
	CountdownTimerHandle.TimerFinishedDelegate.BindWeakLambda(this, [&]()
	{
		StopCountdownTimer(CountdownTimerHandle);
		OnCountdownTimerFinished(CountdownTimerHandle.Type);
	});

	GetWorldTimerManager().SetTimer(CountdownTimerHandle.TimerFinishedHandle, CountdownTimerHandle.TimerFinishedDelegate, CountdownTimerHandle.CountdownTime, false);

	CountdownTimerHandle.TimerUpdateDelegate.BindWeakLambda(this, [&]()
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			const ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(It->Get());
			if (IsValid(DSPlayerController))
			{
				const float CountdownTimeLeft = CountdownTimerHandle.CountdownTime - GetWorldTimerManager().GetTimerElapsed(CountdownTimerHandle.TimerFinishedHandle);
				DSPlayerController->ClientTimerUpdated(CountdownTimeLeft, CountdownTimerHandle.Type);
			}
		}
	});
	
	GetWorldTimerManager().SetTimer(CountdownTimerHandle.TimerUpdateHandle, CountdownTimerHandle.TimerUpdateDelegate, CountdownTimerHandle.CountdownUpdateInterval, true, 0.f);
}

void ADS_GameModeBase::StopCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle)
{
	CountdownTimerHandle.State = ECountdownTimerState::Stopped;
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle.TimerFinishedHandle);
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle.TimerUpdateHandle);
	if (CountdownTimerHandle.TimerFinishedDelegate.IsBound())
	{
		CountdownTimerHandle.TimerFinishedDelegate.Unbind();
	}
	if (CountdownTimerHandle.TimerUpdateDelegate.IsBound())
	{
		CountdownTimerHandle.TimerUpdateDelegate.Unbind();
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(It->Get());
		if (IsValid(DSPlayerController))
		{
			DSPlayerController->ClientTimerStopped(0.f, CountdownTimerHandle.Type);
		}
	}
}

void ADS_GameModeBase::OnCountdownTimerFinished(ECountdownTimerType Type)
{
}
