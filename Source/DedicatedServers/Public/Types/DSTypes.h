#pragma once

#include "DSTypes.generated.h"

UENUM(BlueprintType)
enum class ECountdownTimerState : uint8
{
	NotStarted UMETA(DisplayName = "Not Started"),
	Started UMETA(DisplayName = "Started"),
	Paused UMETA(DisplayName = "Paused"),
	Stopped UMETA(DisplayName = "Stopped")
};

UENUM(BlueprintType)
enum class ECountdownTimerType : uint8
{
	LobbyCountdown UMETA(DisplayName = "Lobby Countdown"),
	PreMatch UMETA(DisplayName = "PreMatch Countdown"),
	Match UMETA(DisplayName = "Match Countdown"),
	PostMatch UMETA(DisplayName = "PostMatch Countdown"),
	None UMETA(DisplayName = "None"),
};

UENUM()
enum class EMatchStatus : uint8
{
	WaitingForPlayers,
	Match,
	PreMatch,
	PostMatch,
	SeamlessTravelling
};

USTRUCT(BlueprintType)
struct FCountdownTimerHandle
{
	GENERATED_BODY()

	FCountdownTimerHandle()
		: State(ECountdownTimerState::NotStarted), Type(ECountdownTimerType::None), CountdownTime(0.f), CountdownUpdateInterval(1.f)
	{}

	UPROPERTY(BlueprintReadWrite)
	ECountdownTimerState State;

	UPROPERTY(BlueprintReadWrite)
	ECountdownTimerType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CountdownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CountdownUpdateInterval;

	FTimerHandle TimerFinishedHandle;
	FTimerHandle TimerUpdateHandle;
	FTimerDelegate TimerFinishedDelegate;
	FTimerDelegate TimerUpdateDelegate;
};

inline bool operator==(const FCountdownTimerHandle& lhs, const FCountdownTimerHandle& rhs)
{
	return lhs.Type == rhs.Type;
}
