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
}
