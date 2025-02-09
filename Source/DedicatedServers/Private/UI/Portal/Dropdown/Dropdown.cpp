// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dropdown/Dropdown.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void UDropdown::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Expander->OnClicked.AddDynamic(this, &UDropdown::ToggleDropdown);
	Button_Expander->OnHovered.AddDynamic(this, &UDropdown::Hover);
	Button_Expander->OnUnhovered.AddDynamic(this, &UDropdown::Unhover);
}

void UDropdown::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetStyleTransparent();
	Collapse();
	Unhover();
}

void UDropdown::SetStyleTransparent()
{
	FButtonStyle Style;
	FSlateBrush Brush;
	Brush.TintColor = FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	Style.Normal = Brush;
	Style.Pressed = Brush;
	Style.Hovered = Brush;
	Style.Disabled = Brush;
	Button_Expander->SetStyle(Style);
}

void UDropdown::ToggleDropdown()
{
	if (bIsExpanded)
	{
		Collapse();
	}
	else
	{
		Expand();
	}
}

void UDropdown::Expand()
{
	WidgetSwitcher->SetActiveWidget(ExpandedWidget);
	bIsExpanded = true;
	Image_Triangle->SetBrush(Triangle_Up);
}

void UDropdown::Collapse()
{
	WidgetSwitcher->SetActiveWidget(CollapsedWidget);
	bIsExpanded = false;
	Image_Triangle->SetBrush(Triangle_Down);
}

void UDropdown::Hover()
{
	TextBlock_ButtonText->SetColorAndOpacity(HoveredTextColor);
}

void UDropdown::Unhover()
{
	TextBlock_ButtonText->SetColorAndOpacity(UnhoveredTextColor);
}
