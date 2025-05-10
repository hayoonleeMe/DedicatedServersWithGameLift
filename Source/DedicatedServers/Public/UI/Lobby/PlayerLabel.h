// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerLabel.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UPlayerLabel : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetUsername(const FString& Username) const;
	FString GetUsername() const;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Username;
};
