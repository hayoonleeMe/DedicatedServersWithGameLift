// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "APITestOverlay.generated.h"

class UFleetId;
class UAPITestManager;
class UListFleetsBox;

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UAPITestOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAPITestManager> APITestManagerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFleetId> FleetIdWidgetClass;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UListFleetsBox> ListFleetsBox;

	UPROPERTY()
	TObjectPtr<UAPITestManager> APITestManager;

	UFUNCTION()
	void ListFleetsButtonClicked();

	UFUNCTION()
	void OnListFleetsResponseReceived(const FDSListFleetsResponse& ListFleetsResponse, bool bWasSuccessful);
};
