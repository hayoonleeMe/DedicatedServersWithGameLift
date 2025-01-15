// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MatchGameState.h"

#include "Player/MatchPlayerState.h"


AMatchGameState::AMatchGameState()
{
	Leaders = TArray<TObjectPtr<AMatchPlayerState>>();
	bHasFirstBloodBeenHad = false;
}

AMatchPlayerState* AMatchGameState::GetLeader() const
{
	if (Leaders.Num() == 1)
	{
		return Leaders[0];
	}
	return nullptr;
}

void AMatchGameState::UpdateLeader()
{
	TArray<APlayerState*> SortedPlayers = PlayerArray;
	SortedPlayers.Sort([](const APlayerState& A, const APlayerState& B)
	{
		const AMatchPlayerState* PlayerA = Cast<AMatchPlayerState>(&A);
		const AMatchPlayerState* PlayerB = Cast<AMatchPlayerState>(&B);
		return PlayerA->GetScoredElims() > PlayerB->GetScoredElims();
	});

	Leaders.Empty();
	
	if (SortedPlayers.Num() > 0)
	{
		int32 HighestScore = 0;
		for (APlayerState* PlayerState : SortedPlayers)
		{
			AMatchPlayerState* Player = Cast<AMatchPlayerState>(PlayerState);
			if (IsValid(Player))
			{
				int32 PlayerScore = Player->GetScoredElims();
                
				// On the first iteration, set the highest score
				if (Leaders.Num() == 0)
				{
					HighestScore = PlayerScore;
					Leaders.Add(Player);
				}
				else if (PlayerScore == HighestScore)
				{
					Leaders.Add(Player); // Add to leaders if scores are tied
				}
				else
				{
					break; // As it's sorted, no need to check further
				}
			}
		}
	}

	bHasFirstBloodBeenHad = true;
}

bool AMatchGameState::IsTiedForTheLead(AMatchPlayerState* PlayerState)
{
	if (Leaders.Contains(PlayerState)) return true;
	return false;
}

void AMatchGameState::BeginPlay()
{
	Super::BeginPlay();
	
}
