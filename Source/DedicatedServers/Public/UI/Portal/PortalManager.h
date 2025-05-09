﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpFwd.h"
#include "Interfaces/PortalManagement.h"
#include "UI/HTTP/HTTPRequestManager.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "PortalManager.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UPortalManager : public UHTTPRequestManager, public IPortalManagement
{
	GENERATED_BODY()

public:
	FAPIStatusMessage SignUpStatusMessageDelegate;
	FAPIStatusMessage ConfirmSignUpStatusMessageDelegate;
	FAPIStatusMessage SignInStatusMessageDelegate;
	
	FOnAPIRequestSucceeded OnSignUpSucceeded;
	FOnAPIRequestSucceeded OnConfirmSignUpSucceeded;
	
	void SignIn(const FString& UserName, const FString& Password);
	void SignUp(const FString& UserName, const FString& Password, const FString& Email);
	void Confirm(const FString& ConfirmationCode);
	void SignOut(const FString& AccessToken);

	/* IPortalManagement */
	virtual void RefreshTokens(const FString& RefreshToken) override;

	UFUNCTION()
	void QuitGame();

	FDSSignUpResponse LastSignUpResponse;
	FString LastUsername;

private:
	void SignIn_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void Confirm_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void RefreshTokens_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void SignOut_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
