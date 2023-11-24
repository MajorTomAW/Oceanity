// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.


#include "ButtonBase.h"

void UButtonBase::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnClicked.AddDynamic(this, &UButtonBase::OnButtonClicked);
	Button->OnHovered.AddDynamic(this, &UButtonBase::OnButtonHovered);
}

void UButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	TextBlock->SetText(ButtonText);
	TextBlock->SetFont(FSlateFontInfo(FontObject, FontSize));
	SetButtonActive(bActive);
}

void UButtonBase::SetButtonActive(bool bNewActive)
{
	FButtonStyle ButtonStyle;
	ButtonStyle.PressedPadding = FMargin(0.0f, 2.0f, 0.0f, 0.0f);

	if (!ensure(NormalAsset) || !ensure(ActiveAsset)) return;
	
	if (bNewActive)
	{
		ButtonStyle.Normal = ActiveAsset->Brush;
		ButtonStyle.Hovered = ActiveAsset->Brush;
		ButtonStyle.Pressed = ActiveAsset->Brush;
	}
	else
	{
		FLinearColor Color = NormalAsset->Brush.TintColor.GetSpecifiedColor();
		Color.A = Color.A + 0.1f;
		FSlateBrush Hovered = NormalAsset->Brush;
		Hovered.TintColor = FSlateColor(Color);
		Color.A = Color.A - 0.2f;
		FSlateBrush Pressed = NormalAsset->Brush;
		Pressed.TintColor = FSlateColor(Color);
		
		ButtonStyle.Normal = NormalAsset->Brush;
		ButtonStyle.Hovered = Hovered;
		ButtonStyle.Pressed = Pressed;
	}
	
	Button->SetStyle(ButtonStyle);
}
