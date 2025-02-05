// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/PortalManager.h"

#include "HttpModule.h"
#include "Data/API/APIData.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/KismetSystemLibrary.h"

void UPortalManager::SignIn(const FString& UserName, const FString& Password)
{
}

void UPortalManager::SignUp(const FString& UserName, const FString& Password, const FString& Email)
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UPortalManager::SignUp_Response);
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::PortalAPI::SignUp);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	const TMap<FString, FString> Params =
	{
		{ TEXT("username"), UserName },
		{ TEXT("password"), Password },
		{ TEXT("email"), Email }
	};
	const FString Content = SerializeJsonContent(Params);
	
	Request->SetContentAsString(Content);
	Request->ProcessRequest();	
}

void UPortalManager::Confirm(const FString& ConfirmationCode)
{
	
}

void UPortalManager::QuitGame()
{
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
	}
}

void UPortalManager::SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	
}
