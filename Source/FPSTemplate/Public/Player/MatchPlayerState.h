// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MatchPlayerState.generated.h"

enum class ESpecialElimType : uint16;
class USpecialElimData;
struct FSpecialElimInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);

/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API AMatchPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AMatchPlayerState();

	void AddScoredElim();
	void AddDefeat();
	void AddHit();
	void AddMiss();
	APlayerState* GetLastAttacker() const { return LastAttacker; }
	void SetLastAttacker(APlayerState* Attacker) { LastAttacker = Attacker; }
	bool IsOnStreak() const { return bOnStreak; }
	void SetOnStreak(bool bIsOnStreak) { bOnStreak = bIsOnStreak; }
	int32 GetScoredElims() const { return ScoredElims; }

	void AddHeadShotElim();
	void AddSequentialElim(int32 SequenceCount);
	void UpdateHighestStreak(int32 StreakCount);
	void AddRevengeElim();
	void AddDethroneElim();
	void AddShowStopperElim();
	void GotFirstBlood();
	void IsTheWinner();

	UFUNCTION(Client, Reliable)
	void Client_SpecialElim(const ESpecialElimType& SpecialElim, int32 SequentialElimCount, int32 StreakCount, int32 ElimScore);

	UFUNCTION(Client, Reliable)
	void Client_ScoredElim(int32 ElimScore);

	UFUNCTION(Client, Reliable)
	void Client_LostTheLead();

	UPROPERTY(BlueprintAssignable)
	FOnScoreChanged OnScoreChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USpecialElimData> SpecialElimData;

	TArray<ESpecialElimType> DecodeElimBitmask(ESpecialElimType ElimTypeBitmask);

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SpecialElimWidgetClass;
	
private:
	int32 ScoredElims;
	int32 Defeats;
	int32 Hits;
	int32 Misses;
	bool bOnStreak;

	int32 HeadShotElims;
	TMap<int32, int32> SequentialElims;
	int32 HighestStreak;
	int32 RevengeElims;
	int32 DethroneElims;
	int32 ShowStopperElims;
	bool bFirstBlood;
	bool bWinner;

	UPROPERTY()
	TObjectPtr<APlayerState> LastAttacker;

	TQueue<FSpecialElimInfo> SpecialElimQueue;
	bool bIsProcessingQueue;

	void ProcessNextSpecialElim();
	void ShowSpecialElim(const FSpecialElimInfo& ElimMessageInfo);
};


