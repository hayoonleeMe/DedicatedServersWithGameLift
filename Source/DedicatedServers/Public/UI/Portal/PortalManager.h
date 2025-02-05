// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpFwd.h"
#include "UI/HTTP/HTTPRequestManager.h"
#include "PortalManager.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UPortalManager : public UHTTPRequestManager
{
	GENERATED_BODY()

public:
	void SignIn(const FString& UserName, const FString& Password);
	void SignUp(const FString& UserName, const FString& Password, const FString& Email);
	void Confirm(const FString& ConfirmationCode);

	UFUNCTION()
	void QuitGame();

private:
	void SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
