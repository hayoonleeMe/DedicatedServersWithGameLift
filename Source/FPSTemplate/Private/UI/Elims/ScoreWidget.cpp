// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Elims/ScoreWidget.h"
#include "Components/TextBlock.h"
#include "Player/MatchPlayerState.h"
#include "Player/ShooterPlayerController.h"

void UScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AMatchPlayerState* PlayerState = GetPlayerState();
	if (IsValid(PlayerState))
	{
		PlayerState->OnScoreChanged.AddDynamic(this, &UScoreWidget::OnScoreChanged);
		OnScoreChanged(PlayerState->GetScoredElims());
	}
	else
	{
		AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(GetOwningPlayer());
		if (IsValid(ShooterPlayerController))
		{
			ShooterPlayerController->OnPlayerStateReplicated.AddUniqueDynamic(this, &UScoreWidget::OnPlayerStateInitialized);
		}
	}
}

void UScoreWidget::OnScoreChanged(int32 NewScore)
{
	if (IsValid(ScoreText))
	{
		ScoreText->SetText(FText::AsNumber(NewScore));
	}
}

void UScoreWidget::OnPlayerStateInitialized()
{
	// Get the PlayerState and bind to the score changed delegate
	AMatchPlayerState* PlayerState = GetPlayerState();
	if (IsValid(PlayerState))
	{
		PlayerState->OnScoreChanged.AddDynamic(this, &UScoreWidget::OnScoreChanged);
		OnScoreChanged(PlayerState->GetScoredElims());
	}

	// Unsubscribe from the OnPlayerStateChanged delegate
	AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(GetOwningPlayer());
	if (IsValid(ShooterPlayerController))
	{
		ShooterPlayerController->OnPlayerStateReplicated.RemoveDynamic(this, &UScoreWidget::OnPlayerStateInitialized);
	}
}

AMatchPlayerState* UScoreWidget::GetPlayerState() const
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		return PlayerController->GetPlayerState<AMatchPlayerState>();
	}
	return nullptr;
}
