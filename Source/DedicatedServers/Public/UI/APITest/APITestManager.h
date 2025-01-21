// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/HTTP/HTTPRequestManager.h"
#include "APITestManager.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UAPITestManager : public UHTTPRequestManager
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void ListFleetsButtonClicked();

	void ListFleets_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
