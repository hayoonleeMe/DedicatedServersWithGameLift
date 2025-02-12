﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/SignIn/SignInOverlay.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Portal/PortalManager.h"
#include "UI/Portal/SignIn/ConfirmSignUpPage.h"
#include "UI/Portal/SignIn/SignInPage.h"
#include "UI/Portal/SignIn/SignUpPage.h"
#include "UI/Portal/SignIn/SuccessConfirmedPage.h"

void USignInOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	check(PortalManagerClass);
	PortalManager = NewObject<UPortalManager>(this, PortalManagerClass);
	
	SignInPage->Button_SignIn->OnClicked.AddDynamic(this, &USignInOverlay::SignInButtonClicked);
	SignInPage->Button_SignUp->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignUpPage);
	SignInPage->Button_Quit->OnClicked.AddDynamic(PortalManager, &UPortalManager::QuitGame);
	PortalManager->SignInStatusMessageDelegate.AddDynamic(SignInPage, &USignInPage::UpdateStatusMessage);

	SignUpPage->Button_Back->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignInPage);
	SignUpPage->Button_SignUp->OnClicked.AddDynamic(this, &USignInOverlay::SignUpButtonClicked);
	PortalManager->SignUpStatusMessageDelegate.AddDynamic(SignUpPage, &USignUpPage::UpdateStatusMessage);
	PortalManager->OnSignUpSucceeded.AddDynamic(this, &USignInOverlay::OnSignUpSucceeded);

	ConfirmSignUpPage->Button_Confirm->OnClicked.AddDynamic(this, &USignInOverlay::ConfirmButtonClicked);
	ConfirmSignUpPage->Button_Back->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignUpPage);
	PortalManager->ConfirmSignUpStatusMessageDelegate.AddDynamic(ConfirmSignUpPage, &UConfirmSignUpPage::UpdateStatusMessage);
	PortalManager->OnConfirmSignUpSucceeded.AddDynamic(this, &USignInOverlay::OnConfirmSignUpSucceeded);

	SuccessConfirmedPage->Button_Ok->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignInPage);
}

void USignInOverlay::ShowSignInPage()
{
	WidgetSwitcher->SetActiveWidget(SignInPage);
}

void USignInOverlay::ShowSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(SignUpPage);
}

void USignInOverlay::ShowConfirmSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(ConfirmSignUpPage);
}

void USignInOverlay::ShowSuccessConfirmedPage()
{
	WidgetSwitcher->SetActiveWidget(SuccessConfirmedPage);
}

void USignInOverlay::SignInButtonClicked()
{
	SignInPage->Button_SignIn->SetIsEnabled(false);
	const FString UserName = SignInPage->TextBox_UserName->GetText().ToString();
	const FString Password = SignInPage->TextBox_Password->GetText().ToString();
	if (PortalManager)
	{
		PortalManager->SignIn(UserName, Password);
	}
}

void USignInOverlay::SignUpButtonClicked()
{
	const FString UserName = SignUpPage->TextBox_UserName->GetText().ToString();
	const FString Password = SignUpPage->TextBox_Password->GetText().ToString();
	const FString Email = SignUpPage->TextBox_Email->GetText().ToString();
	if (PortalManager)
	{
		PortalManager->SignUp(UserName, Password, Email);
	}
}

void USignInOverlay::ConfirmButtonClicked()
{
	ConfirmSignUpPage->Button_Confirm->SetIsEnabled(false);
	const FString ConfirmationCode = ConfirmSignUpPage->TextBox_ConfirmationCode->GetText().ToString();
	if (PortalManager)
	{
		PortalManager->Confirm(ConfirmationCode);
	}
}

void USignInOverlay::OnSignUpSucceeded()
{
	SignUpPage->ClearTextBoxes();
	ConfirmSignUpPage->TextBlock_Destination->SetText(FText::FromString(PortalManager->LastSignUpResponse.CodeDeliveryDetails.Destination));
	ShowConfirmSignUpPage();
}

void USignInOverlay::OnConfirmSignUpSucceeded()
{
	ConfirmSignUpPage->ClearTextBoxes();
	ShowSuccessConfirmedPage();
}
