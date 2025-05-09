// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/DSTypes.h"
#include "TimerWidget.generated.h"

class UTextBlock;
class ADSPlayerController;

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UTimerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTimerWidget(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECountdownTimerType TimerType;

	UPROPERTY()
	TObjectPtr<ADSPlayerController> OwningPlayerController;

	bool bActive = false;

	UPROPERTY(EditAnywhere)
	bool bCanBeNegative;
	
	UPROPERTY(EditAnywhere)
	bool bShowCentiSeconds;

	UPROPERTY(EditAnywhere)
	bool bHiddenWhenInactive;

	UFUNCTION()
	virtual void OnTimerUpdated(float CountdownTimeLeft, ECountdownTimerType Type);
	
	UFUNCTION()
	virtual void OnTimerStopped(float CountdownTimeLeft, ECountdownTimerType Type);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Timer Updated"))
	void K2_OnTimerUpdated(float Time, ECountdownTimerType Type);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Timer Started"))
	void K2_OnTimerStarted(float Time, ECountdownTimerType Type);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Timer Stopped"))
	void K2_OnTimerStopped(float Time, ECountdownTimerType Type);

	FString FormatTimeAsString(float TimeSeconds) const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Time;

	float InternalCountdown = 0.f;

	void TimerStarted(float InitialTime);
	void TimerStopped();
	void UpdateCountdown(float TimeSeconds);
};
