// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ListFleetsBox.generated.h"

class UButton;
class UScrollBox;

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UListFleetsBox : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_ListFleets;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_ListFleets;
};
