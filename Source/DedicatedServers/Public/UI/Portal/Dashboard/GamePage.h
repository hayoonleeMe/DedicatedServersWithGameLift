// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePage.generated.h"

class UGameSessionsManager;
class UJoinGame;

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UGamePage : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UJoinGame> JoinGameWidget;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void JoinGameButtonClicked();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameSessionsManager> GameSessionsManagerClass;

	UPROPERTY()
	TObjectPtr<UGameSessionsManager> GameSessionsManager;
};
