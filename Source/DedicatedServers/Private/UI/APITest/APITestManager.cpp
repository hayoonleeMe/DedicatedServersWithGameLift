// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/APITest/APITestManager.h"

#include "HttpModule.h"
#include "Data/API/APIData.h"

void UAPITestManager::ListFleetsButtonClicked()
{
	check(APIData);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UAPITestManager::ListFleets_Response);

	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::ListFleets);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->ProcessRequest();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("List Fleets Request Made"));
}

void UAPITestManager::ListFleets_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("List Fleets Response Received"));
}
