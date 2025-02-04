// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignInPage.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API USignInPage : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_UserName;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_Password;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_SignIn;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_SignUp;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Quit;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessage;
};
