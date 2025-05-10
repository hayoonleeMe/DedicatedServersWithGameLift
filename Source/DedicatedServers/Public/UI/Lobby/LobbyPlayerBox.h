// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyPlayerBox.generated.h"

struct FLobbyPlayerInfo;
class ALobbyState;
class UPlayerLabel;
class UScrollBox;

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ULobbyPlayerBox : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_PlayerInfo;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerLabel> PlayerLabelClass;

protected:
	virtual void NativeOnInitialized() override;

	void UpdatePlayerInfo(ALobbyState* LobbyState);

	UFUNCTION()
	void OnLobbyStateInitialized(ALobbyState* LobbyState);

	UFUNCTION()
	void CreateAndAddPlayerLabel(const FLobbyPlayerInfo& PlayerInfo);

	UFUNCTION()
	void OnPlayerRemoved(const FLobbyPlayerInfo& PlayerInfo);

private:
	UPlayerLabel* FindPlayerLabel(const FString& Username);
};
