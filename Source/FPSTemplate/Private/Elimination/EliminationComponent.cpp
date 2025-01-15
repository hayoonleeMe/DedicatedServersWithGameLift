#include "Elimination/EliminationComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MatchPlayerState.h"
#include "Game/MatchGameState.h"


UEliminationComponent::UEliminationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SequentialElimInterval = 2.f;
    ElimsNeededForStreak = 5;
    LastElimTime = 0.f;
    SequentialElims = 0;
    Streak = 0;
}

void UEliminationComponent::BeginPlay()
{
    Super::BeginPlay();
    
}

AMatchPlayerState* UEliminationComponent::GetPlayerStateFromActor(AActor* Actor)
{
    APawn* Pawn = Cast<APawn>(Actor);
    if (IsValid(Pawn))
    {
        return Pawn->GetPlayerState<AMatchPlayerState>();
    }
    return nullptr;
}


void UEliminationComponent::OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal)
{
    AMatchPlayerState* AttackerPS = GetPlayerStateFromActor(Attacker);
    if (!IsValid(AttackerPS)) return;

    ProcessHitOrMiss(bHit, AttackerPS);
    if (!bHit) return; // Early return if it was a miss

    AMatchPlayerState* VictimPS = GetPlayerStateFromActor(Victim);
    if (!IsValid(VictimPS)) return;

    if (bLethal) ProcessElimination(bHeadShot, AttackerPS, VictimPS);
}

void UEliminationComponent::ProcessHitOrMiss(bool bHit, AMatchPlayerState* AttackerPS)
{
	if (bHit)
	{
		AttackerPS->AddHit();
	}
	else
	{
		AttackerPS->AddMiss();
	}
}

void UEliminationComponent::ProcessHeadshot(bool bHeadShot, ESpecialElimType& SpecialElimType, AMatchPlayerState* AttackerPS)
{
    if (bHeadShot)
    {
        SpecialElimType |= ESpecialElimType::Headshot;
        AttackerPS->AddHeadShotElim();
    }
}

void UEliminationComponent::ProcessSequentialEliminations(AMatchPlayerState* AttackerPS, ESpecialElimType& SpecialElimType)
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastElimTime <= SequentialElimInterval)
    {
        SequentialElims++;
    }
    else
    {
        SequentialElims = 1;
    }
    LastElimTime = CurrentTime;

    if (SequentialElims > 1)
    {
        SpecialElimType |= ESpecialElimType::Sequential;
        AttackerPS->AddSequentialElim(SequentialElims);
    }
}

void UEliminationComponent::ProcessStreaks(AMatchPlayerState* AttackerPS, AMatchPlayerState* VictimPS, ESpecialElimType& SpecialElimType)
{
    ++Streak;
    if (Streak >= ElimsNeededForStreak)
    {
        SpecialElimType |= ESpecialElimType::Streak;
        AttackerPS->SetOnStreak(true);
        AttackerPS->UpdateHighestStreak(Streak);
    }
    if (VictimPS->IsOnStreak())
    {
        SpecialElimType |= ESpecialElimType::Showstopper;
        AttackerPS->AddShowStopperElim();
    }
    VictimPS->SetOnStreak(false);

    if (AttackerPS->GetLastAttacker() == VictimPS)
    {
        SpecialElimType |= ESpecialElimType::Revenge;
        AttackerPS->AddRevengeElim();
        AttackerPS->SetLastAttacker(nullptr);
    }
    VictimPS->SetLastAttacker(AttackerPS);
}

void UEliminationComponent::HandleFirstBlood(AMatchGameState* GameState, ESpecialElimType& SpecialElimType, AMatchPlayerState* AttackerPS)
{
    if (!GameState->HasFirstBloodBeenHad())
    {
        SpecialElimType |= ESpecialElimType::FirstBlood;
        AttackerPS->GotFirstBlood();
    }
}

void UEliminationComponent::UpdateLeaderStatus(AMatchGameState* GameState, ESpecialElimType& SpecialElimType, AMatchPlayerState* AttackerPS, AMatchPlayerState* VictimPS)
{
    AMatchPlayerState* LastLeader = GameState->GetLeader();
    const bool bAttackerWasTiedForTheLead = GameState->IsTiedForTheLead(AttackerPS);
    GameState->UpdateLeader();
    if (!bAttackerWasTiedForTheLead && GameState->IsTiedForTheLead(AttackerPS))
    {
        SpecialElimType |= ESpecialElimType::TiedTheLeader;
    }
    if (IsValid(LastLeader) && LastLeader != GameState->GetLeader())
    {
        // LastLeader has now lost the lead
        LastLeader->Client_LostTheLead();
        
        if (VictimPS == LastLeader)
        {
            SpecialElimType |= ESpecialElimType::Dethrone;
            AttackerPS->AddDethroneElim();
        }
    }
    
    if (AttackerPS != LastLeader && AttackerPS == GameState->GetLeader())
    {
        SpecialElimType |= ESpecialElimType::GainedTheLead;
    }
}

bool UEliminationComponent::HasSpecialElimTypes(const ESpecialElimType& SpecialElimType)
{
    return static_cast<uint8>(SpecialElimType) != 0;
}

void UEliminationComponent::ProcessElimination(bool bHeadShot, AMatchPlayerState* AttackerPS, AMatchPlayerState* VictimPS)
{
    AttackerPS->AddScoredElim();
    VictimPS->AddDefeat();

    ESpecialElimType SpecialElimType{};
    ProcessHeadshot(bHeadShot, SpecialElimType, AttackerPS);

    ProcessSequentialEliminations(AttackerPS, SpecialElimType);
    ProcessStreaks(AttackerPS, VictimPS, SpecialElimType);

    AMatchGameState* GameState = Cast<AMatchGameState>(UGameplayStatics::GetGameState(AttackerPS));
    if (IsValid(GameState))
    {
        HandleFirstBlood(GameState, SpecialElimType, AttackerPS);
        UpdateLeaderStatus(GameState, SpecialElimType, AttackerPS, VictimPS);
    }

    if (HasSpecialElimTypes(SpecialElimType))
    {
        // inform the client of a special elim.
        AttackerPS->Client_SpecialElim(SpecialElimType, SequentialElims, Streak, AttackerPS->GetScoredElims());
    }
    else
    {
        // no special elims; save some bandwidth.
        AttackerPS->Client_ScoredElim(AttackerPS->GetScoredElims()); 
    }
}
