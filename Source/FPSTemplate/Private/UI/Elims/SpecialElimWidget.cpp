// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Elims/SpecialElimWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void USpecialElimWidget::InitializeWidget(const FString& InElimMessage, UTexture2D* InElimTexture)
{
	if (IsValid(ElimText))
	{
		ElimText->SetText(FText::FromString(InElimMessage));
	}

	if (IsValid(ElimImage) && InElimTexture)
	{
		ElimImage->SetBrushFromTexture(InElimTexture);
	}
}

void USpecialElimWidget::CenterWidget(UUserWidget* Widget, float VerticalRatio)
{
	if (!IsValid(Widget))
	{
		return;
	}
	
	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(Widget);
	const float VerticalFraction = VerticalRatio == 0.f ? 1.f : VerticalRatio * 2.f;
	FVector2D CenterPosition(ViewportSize.X / 2.0f, VerticalFraction * ViewportSize.Y / 2.0f);
	Widget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f)); // Align widget center to the center of the viewport
	Widget->SetPositionInViewport(CenterPosition, true);
}