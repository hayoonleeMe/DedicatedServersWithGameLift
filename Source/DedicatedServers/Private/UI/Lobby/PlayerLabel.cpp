// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/PlayerLabel.h"

#include "Components/TextBlock.h"

void UPlayerLabel::SetUsername(const FString& Username) const
{
	TextBlock_Username->SetText(FText::FromString(Username));
}

FString UPlayerLabel::GetUsername() const
{
	return TextBlock_Username->GetText().ToString();
}
