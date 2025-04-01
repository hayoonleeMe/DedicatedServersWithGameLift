// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Timers/TimerWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"
#include "Player/DSPlayerController.h"

UTimerWidget::UTimerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TimerType = ECountdownTimerType::None;
	
	bCanBeNegative = false;
	bShowCentiSeconds = true;
	bHiddenWhenInactive = true;
}

void UTimerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OwningPlayerController = GetOwningPlayer<ADSPlayerController>();
	if (OwningPlayerController)
	{
		OwningPlayerController->OnTimerUpdated.AddDynamic(this, &UTimerWidget::OnTimerUpdated);
		OwningPlayerController->OnTimerStopped.AddDynamic(this, &UTimerWidget::OnTimerStopped);
	}

	if (bHiddenWhenInactive)
	{
		TextBlock_Time->SetRenderOpacity(0.f);
	}
}

void UTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bActive)
	{
		UpdateCountdown(InternalCountdown - InDeltaTime);
	}
}

void UTimerWidget::OnTimerUpdated(float CountdownTimeLeft, ECountdownTimerType Type)
{
	if (Type != TimerType)
	{
		return;
	}

	if (!bActive)
	{
		TimerStarted(CountdownTimeLeft);
	}
	
	UpdateCountdown(CountdownTimeLeft);
	K2_OnTimerUpdated(CountdownTimeLeft, TimerType);
}

void UTimerWidget::OnTimerStopped(float CountdownTimeLeft, ECountdownTimerType Type)
{
	if (Type != TimerType)
	{
		return;
	}
	
	TimerStopped();
	K2_OnTimerStopped(CountdownTimeLeft, TimerType);
}

FString UTimerWidget::FormatTimeAsString(float TimeSeconds) const
{
	TimeSeconds = bCanBeNegative ? TimeSeconds : FMath::Abs(TimeSeconds);
	FString DisplayTimeString;
	if (bShowCentiSeconds)
	{
		DisplayTimeString = UKismetStringLibrary::TimeSecondsToString(TimeSeconds); 
	}
	else
	{
		// this code is from UKismetStringLibrary::TimeSecondsToString() 
		const TCHAR* NegativeModifier = TimeSeconds < 0.f? TEXT("-") : TEXT("");
		TimeSeconds = FMath::Abs(TimeSeconds);
		const int32 NumMinutes = FMath::FloorToInt(TimeSeconds/60.f);
		const int32 NumSeconds = FMath::FloorToInt(TimeSeconds-(NumMinutes*60.f));
		DisplayTimeString = FString::Printf(TEXT("%s%02d:%02d"), NegativeModifier, NumMinutes, NumSeconds);
	}
	return DisplayTimeString;
}

void UTimerWidget::TimerStarted(float InitialTime)
{
	bActive = true;
	TextBlock_Time->SetRenderOpacity(1.f);
}

void UTimerWidget::TimerStopped()
{
	bActive = false;
	UpdateCountdown(0.f);
	if (bHiddenWhenInactive)
	{
		TextBlock_Time->SetRenderOpacity(0.f);
	}
}

void UTimerWidget::UpdateCountdown(float TimeSeconds)
{
	InternalCountdown = TimeSeconds;
	const FText TimeText = FText::FromString(FormatTimeAsString(InternalCountdown));
	TextBlock_Time->SetText(TimeText);
}
