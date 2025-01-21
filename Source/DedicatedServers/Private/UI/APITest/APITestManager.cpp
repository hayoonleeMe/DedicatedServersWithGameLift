// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/APITest/APITestManager.h"

void UAPITestManager::ListFleetsButtonClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("List Fleets Button Clicked"));
}
