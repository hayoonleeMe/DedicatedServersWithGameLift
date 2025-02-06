// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/SignIn/SignInPage.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void USignInPage::UpdateStatusMessage(const FString& Message, bool bShouldResetWidget)
{
	TextBlock_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidget)
	{
		Button_SignIn->SetIsEnabled(true);
	}
}
