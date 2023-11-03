// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipComponentPanel.h"

#include "OceanityGame/Libraries/ShipLibrary.h"

void UShipComponentPanel::NativeConstruct()
{
	Super::NativeConstruct();
}

void UShipComponentPanel::LinkShipComponent(const FShipComponentProperty* ShipComponentProperty)
{
	ComponentCost->SetText(FText::FromString(FString::FromInt(ShipComponentProperty->ComponentCost)));
	ComponentName->SetText(FText::FromString(ShipComponentProperty->ComponentName));
	ComponentImage->GetDynamicMaterial()->SetTextureParameterValue(TEXT("Texture"), ShipComponentProperty->ComponentIcon);
}
