﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HTTPRequestManager.generated.h"

class UDSLocalPlayerSubsystem;
class UAPIData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAPIStatusMessage, const FString&, StatusMessage, bool, bShouldResetWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAPIRequestSucceeded);

/**
 * 
 */
UCLASS(Blueprintable)
class DEDICATEDSERVERS_API UHTTPRequestManager : public UObject
{
	GENERATED_BODY()

public:
	UDSLocalPlayerSubsystem* GetDSLocalPlayerSubsystem() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAPIData> APIData;

	static bool ContainsErrors(const TSharedPtr<FJsonObject>& JsonObject);
	static void DumpMetaData(const TSharedPtr<FJsonObject>& JsonObject);

	static FString SerializeJsonContent(const TMap<FString, FString>& Params);
};
