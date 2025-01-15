// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreWidget.generated.h"

class UTextBlock;
class AMatchPlayerState;

/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

protected:
	UFUNCTION()
	void OnScoreChanged(int32 NewScore);

	UFUNCTION()
	void OnPlayerStateInitialized();

	AMatchPlayerState* GetPlayerState() const;
};
