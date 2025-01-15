#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterTypes/ShooterTypes.h"
#include "EliminationComponent.generated.h"

class AMatchPlayerState;
class AMatchGameState;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPSTEMPLATE_API UEliminationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEliminationComponent();
    
    UFUNCTION()
    void OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal);

protected:
    virtual void BeginPlay() override;

private:

    // Time frame to determine sequential elims
    UPROPERTY(EditDefaultsOnly, Category = Elimination)
    float SequentialElimInterval;

    UPROPERTY(EditDefaultsOnly, Category = Elimination)
    int32 ElimsNeededForStreak;

    float LastElimTime;
    int32 SequentialElims;
    int32 Streak;
    
    AMatchPlayerState* GetPlayerStateFromActor(AActor* Actor);
    void ProcessHitOrMiss(bool bHit, AMatchPlayerState* AttackerPS);
    void ProcessElimination(bool bHeadShot, AMatchPlayerState* AttackerPS, AMatchPlayerState* VictimPS);
    void ProcessHeadshot(bool bHeadShot, ESpecialElimType& SpecialElimType, AMatchPlayerState* AttackerPS);
    void ProcessSequentialEliminations(AMatchPlayerState* AttackerPS, ESpecialElimType& SpecialElimType);
    void ProcessStreaks(AMatchPlayerState* AttackerPS, AMatchPlayerState* VictimPS, ESpecialElimType& SpecialElimType);
    void HandleFirstBlood(AMatchGameState* GameState, ESpecialElimType& SpecialElimType, AMatchPlayerState* AttackerPS);
    void UpdateLeaderStatus(AMatchGameState* GameState, ESpecialElimType& SpecialElimType, AMatchPlayerState* AttackerPS, AMatchPlayerState* VictimPS);
    bool HasSpecialElimTypes(const ESpecialElimType& SpecialElimType);
};