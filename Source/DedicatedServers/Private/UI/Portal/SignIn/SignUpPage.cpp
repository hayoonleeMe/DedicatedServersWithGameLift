// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/SignIn/SignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void USignUpPage::UpdateStatusMessage(const FString& Message, bool bShouldResetWidget)
{
	TextBlock_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidget)
	{
		Button_SignUp->SetIsEnabled(true);
	}
}

void USignUpPage::NativeConstruct()
{
	Super::NativeConstruct();

	TextBox_UserName->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);
	TextBox_Password->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);
	TextBox_ConfirmPassword->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);
	TextBox_Email->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);

	Button_SignUp->SetIsEnabled(false);
}

void USignUpPage::UpdateSignUpButtonState(const FText& Text)
{
	const bool bIsUserNameValid = !TextBox_UserName->GetText().IsEmpty();
	const bool bArePasswordEqual = TextBox_Password->GetText().ToString().Equals(TextBox_ConfirmPassword->GetText().ToString());
	const bool bIsValidEmail = IsValidEmail(TextBox_Email->GetText().ToString());
	const bool bIsPasswordLongEnough = TextBox_Password->GetText().ToString().Len() >= 8;
	
	FString StatusMessage;
	const bool bIsStrongPassword = IsStrongPassword(TextBox_Password->GetText().ToString(), StatusMessage);
	
	if (!bIsUserNameValid)
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("Please enter a valid Username.")));
	}
	else if (!bIsStrongPassword)
	{
		TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));
	}
	else if (!bIsPasswordLongEnough)
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("Passwords must be at least 8 characters")));
	}
	else if (!bArePasswordEqual)
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("Please ensure that passwords match.")));
	}
	else if (!bIsValidEmail)
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("Please enter a valid email.")));
	}
	else
	{
		TextBlock_StatusMessage->SetText(FText::GetEmpty());
	}

	Button_SignUp->SetIsEnabled(bIsUserNameValid && bIsStrongPassword && bIsPasswordLongEnough && bArePasswordEqual && bIsValidEmail);
}

bool USignUpPage::IsValidEmail(const FString& Email)
{
	// R : Raw String으로, Escape Sequence(\n, \t, \" 등)을 해석하지 않고 그대로 문자열로 처리함. 줄바꿈도 가능
	/*
	 	^			: 문자열의 시작을 의미
	 	[^\s@]+		: 공백(\s) 또는 @을 포함하지 않는(^는 부정) 문자들의 연속된 그룹 (즉, 로컬 파트)
	 	@			: @ 기호가 정확히 한 개 포함되어야 함
	 	[^\s@]+		: 공백(\s) 또는 @을 포함하지 않는 문자들의 연속된 그룹 (즉, 도메인 네임)
	 	\.			: . 문자 그대로 (정규식에서 .는 "아무 문자"를 의미하는 기능)
	 	[^\s@]{2,}	: 공백(\s) 또는 @을 포함하지 않는 최소 2자 이상의 문자 그룹 (즉, .com, .net 같은 TLD)
	 	$			: 문자열의 끝을 의미
	 */
	const FRegexPattern EmailPattern(TEXT(R"(^[^\s@]+@[^\s@]+\.[^\s@]{2,}$)"));
	
	FRegexMatcher Matcher(EmailPattern, Email);
	return Matcher.FindNext();	// 일치하면 true 반환
}

bool USignUpPage::IsStrongPassword(const FString& Password, FString& StatusMessage)
{
	// contains at least one number
	const FRegexPattern NumberPattern(TEXT(R"(\d)"));
	
	// contains at least one special character
	// \w : a~z(both upper, lower case), 0~9, _
	// \s : 공백
	const FRegexPattern SpecialCharPattern(TEXT(R"([^\w\s])"));
	
	// contains at least one upper case character
	const FRegexPattern UpperCasePattern(TEXT(R"([A-Z])"));
	
	// contains at least one lower case character
	const FRegexPattern LowerCasePattern(TEXT(R"([a-z])"));

	FRegexMatcher NumberMatcher(NumberPattern, Password);
	FRegexMatcher SpecialCharMatcher(SpecialCharPattern, Password);
	FRegexMatcher UpperCaseMatcher(UpperCasePattern, Password);
	FRegexMatcher LowerCaseMatcher(LowerCasePattern, Password);

	if (!NumberMatcher.FindNext())
	{
		StatusMessage = TEXT("Password must contain at least one number.");
		return false;
	}
	if (!SpecialCharMatcher.FindNext())
	{
		StatusMessage = TEXT("Password must contain at least one special character.");
		return false;
	}
	if (!UpperCaseMatcher.FindNext())
	{
		StatusMessage = TEXT("Password must contain at least one uppercase character.");
		return false;
	}
	if (!LowerCaseMatcher.FindNext())
	{
		StatusMessage = TEXT("Password must contain at least one lowercase character.");
		return false;
	}
	return true;
}
