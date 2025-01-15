// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MatchPlayerState.h"

#include "Data/SpecialElimData.h"
#include "ShooterTypes/ShooterTypes.h"
#include "UI/Elims/SpecialElimWidget.h"

AMatchPlayerState::AMatchPlayerState()
{
	NetUpdateFrequency = 100.f; // let's not be sluggish, alright?
	
	ScoredElims = 0;
	Defeats = 0;
	Hits = 0;
	Misses = 0;
	bOnStreak = false;
	HeadShotElims = 0;
	SequentialElims = TMap<int32, int32>();
	HighestStreak = 0;
	RevengeElims = 0;
	DethroneElims = 0;
	ShowStopperElims = 0;
	bFirstBlood = false;
	bWinner = false;
}

void AMatchPlayerState::AddScoredElim()
{
	++ScoredElims;
}

void AMatchPlayerState::AddDefeat()
{
	++Defeats;
}

void AMatchPlayerState::AddHit()
{
	++Hits;
}

void AMatchPlayerState::AddMiss()
{
	++Misses;
}

void AMatchPlayerState::AddHeadShotElim()
{
	++HeadShotElims;
}

void AMatchPlayerState::AddSequentialElim(int32 SequenceCount)
{
	if (SequentialElims.Contains(SequenceCount))
	{
		SequentialElims[SequenceCount]++;
	}
	else
	{
		SequentialElims.Add(SequenceCount, 1);
	}

	// Reduce the count for all lower sequence counts
	// This is because a triple elim means a double was scored first,
	// But we want to count this as just a triple, i.e:
	// elim 1, elim 2, elim 3 = just a triple, not a double and a triple.
	for (auto& Elem : SequentialElims)
	{
		if (Elem.Key < SequenceCount && Elem.Value > 0)
		{
			Elem.Value--;
		}
	}
}

void AMatchPlayerState::UpdateHighestStreak(int32 StreakCount)
{
	if (StreakCount > HighestStreak)
	{
		HighestStreak = StreakCount;
	}
}

void AMatchPlayerState::AddRevengeElim()
{
	++RevengeElims;
}

void AMatchPlayerState::AddDethroneElim()
{
	++DethroneElims;
}

void AMatchPlayerState::AddShowStopperElim()
{
	++ShowStopperElims;
}

void AMatchPlayerState::GotFirstBlood()
{
	bFirstBlood = true;
}

void AMatchPlayerState::IsTheWinner()
{
	bWinner = true;
}

TArray<ESpecialElimType> AMatchPlayerState::DecodeElimBitmask(ESpecialElimType ElimTypeBitmask)
{
	TArray<ESpecialElimType> ValidElims;
	uint8 BitmaskValue = static_cast<uint8>(ElimTypeBitmask);

	for(uint8 i = 0; i < 16; i++) // Assuming 8 bits since ESpecialElimType is a uint16
	{
		if (BitmaskValue & (1 << i))
		{
			ESpecialElimType EnumValue = static_cast<ESpecialElimType>(1 << i);
			ValidElims.Add(EnumValue);
		}
	}

	return ValidElims;
}

void AMatchPlayerState::ProcessNextSpecialElim()
{
	FSpecialElimInfo ElimInfo;
	if (SpecialElimQueue.Dequeue(ElimInfo))
	{
		bIsProcessingQueue = true;
		ShowSpecialElim(ElimInfo);

		// Schedule the next elimination processing after a delay
		GetWorldTimerManager().SetTimerForNextTick([this]()
		{
			constexpr float ElimDisplayTime = 0.5f; // Adjust the time in seconds as needed
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AMatchPlayerState::ProcessNextSpecialElim, ElimDisplayTime, false);
		});
	}
	else
	{
		bIsProcessingQueue = false;
	}
}

void AMatchPlayerState::ShowSpecialElim(const FSpecialElimInfo& ElimMessageInfo)
{
	FString ElimMessageString = ElimMessageInfo.ElimMessage;
	if (ElimMessageInfo.ElimType == ESpecialElimType::Sequential)
	{
		FString Msg = ElimMessageInfo.ElimMessage;
		
		int32 Seq = ElimMessageInfo.SequentialElimCount;
		int32 Streak = ElimMessageInfo.StreakCount;
		
		if (ElimMessageInfo.SequentialElimCount == 2) ElimMessageString = FString("Double Elim!");
		else if (ElimMessageInfo.SequentialElimCount == 3) ElimMessageString = FString("Triple Elim!");
		else if (ElimMessageInfo.SequentialElimCount == 4) ElimMessageString = FString("Quad Elim!");
		else if (ElimMessageInfo.SequentialElimCount > 4) ElimMessageString = FString::Printf(TEXT("Rampage x%d!"), ElimMessageInfo.SequentialElimCount);
	}
	if (ElimMessageInfo.ElimType == ESpecialElimType::Streak) ElimMessageString = FString::Printf(TEXT("Streak x%d!"), ElimMessageInfo.StreakCount);
		
	if (SpecialElimWidgetClass)
	{
		USpecialElimWidget* ElimWidget = CreateWidget<USpecialElimWidget>(GetWorld(), SpecialElimWidgetClass);
		if (IsValid(ElimWidget))
		{
			ElimWidget->InitializeWidget(ElimMessageString, ElimMessageInfo.ElimIcon);
			ElimWidget->AddToViewport();
		}
	}
}

void AMatchPlayerState::Client_ScoredElim_Implementation(int32 ElimScore)
{
	OnScoreChanged.Broadcast(ElimScore);
}

void AMatchPlayerState::Client_SpecialElim_Implementation(const ESpecialElimType& SpecialElim, int32 SequentialElimCount, int32 StreakCount, int32 ElimScore)
{
	OnScoreChanged.Broadcast(ElimScore);
	
	if (!IsValid(SpecialElimData)) return;

	TArray<ESpecialElimType> ElimTypes = DecodeElimBitmask(SpecialElim);
	for (ESpecialElimType ElimType : ElimTypes)
	{
		FSpecialElimInfo& ElimInfo = SpecialElimData->SpecialElimInfo.FindChecked(ElimType);
		if (ElimType == ESpecialElimType::Sequential)
		{
			ElimInfo.SequentialElimCount = SequentialElimCount;
		}
		if (ElimType == ESpecialElimType::Streak)
		{
			ElimInfo.StreakCount = StreakCount;
		}
		ElimInfo.ElimType = ElimType;
		SpecialElimQueue.Enqueue(ElimInfo);
	}
	if (!bIsProcessingQueue)
	{
		ProcessNextSpecialElim();
	}
}

void AMatchPlayerState::Client_LostTheLead_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Lost the Lead"), *GetName());
	if (!IsValid(SpecialElimData)) return;
	auto& ElimMessageInfo = SpecialElimData->SpecialElimInfo.FindChecked(ESpecialElimType::LostTheLead);
	
	if (SpecialElimWidgetClass)
	{
		USpecialElimWidget* ElimWidget = CreateWidget<USpecialElimWidget>(GetWorld(), SpecialElimWidgetClass);
		if (IsValid(ElimWidget))
		{
			ElimWidget->InitializeWidget(ElimMessageInfo.ElimMessage, ElimMessageInfo.ElimIcon);
			ElimWidget->AddToViewport();
		}
	}
}
