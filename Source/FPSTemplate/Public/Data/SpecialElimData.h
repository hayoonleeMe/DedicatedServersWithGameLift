// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShooterTypes/ShooterTypes.h"
#include "SpecialElimData.generated.h"

USTRUCT(BlueprintType)
struct FSpecialElimInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ESpecialElimType ElimType = ESpecialElimType::None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString ElimMessage = FString();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> ElimIcon = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 SequentialElimCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 StreakCount = 0;
};

/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API USpecialElimData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpecialEliminations")
	TMap<ESpecialElimType, FSpecialElimInfo> SpecialElimInfo;
};
