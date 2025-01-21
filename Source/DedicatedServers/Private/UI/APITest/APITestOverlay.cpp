// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/APITest/APITestOverlay.h"

#include "Components/Button.h"
#include "UI/API/ListFleets/ListFleetsBox.h"
#include "UI/APITest/APITestManager.h"

void UAPITestOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	check(APITestManagerClass);
	APITestManager = NewObject<UAPITestManager>(this, APITestManagerClass);

	check(ListFleetsBox && ListFleetsBox->Button_ListFleets);
	ListFleetsBox->Button_ListFleets->OnClicked.AddDynamic(this, &UAPITestOverlay::ListFleetsButtonClicked);
}

void UAPITestOverlay::ListFleetsButtonClicked()
{
	check(APITestManager);
	APITestManager->OnListFleetsResponseReceived.AddDynamic(this, &UAPITestOverlay::OnListFleetsResponseReceived);
	APITestManager->ListFleets();
	ListFleetsBox->Button_ListFleets->SetIsEnabled(false);
}

void UAPITestOverlay::OnListFleetsResponseReceived(const FDSListFleetsResponse& ListFleetsResponse, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		
	}
	else
	{
		
	}
	ListFleetsBox->Button_ListFleets->SetIsEnabled(true);
}
