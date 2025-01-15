// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpecialElimWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API USpecialElimWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ElimText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UImage> ElimImage;

	void InitializeWidget(const FString& InElimMessage, UTexture2D* InElimTexture);

	UFUNCTION(BlueprintCallable)
	static void CenterWidget(UUserWidget* Widget, float VerticalRatio = 0.f);
};
