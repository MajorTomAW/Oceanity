// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipComponentPanel.h"

#include "OceanityGame/Libraries/ShipLibrary.h"

void UShipComponentPanel::NativeConstruct()
{
	Super::NativeConstruct();
}

void UShipComponentPanel::LinkShipComponent(const FShipComponentProperty* NewShipComponentProperty)
{
	ShipComponentProperty = const_cast<FShipComponentProperty*>(NewShipComponentProperty);
	ComponentCost->SetText(FText::FromString(FString::FromInt(ShipComponentProperty->ComponentCost)));
	ComponentName->SetText(FText::FromString(ShipComponentProperty->ComponentName));
	ComponentImage->GetDynamicMaterial()->SetTextureParameterValue(TEXT("Texture"), ShipComponentProperty->ComponentIcon);
}

void UShipComponentPanel::ComparePrice(int32 Gold)
{
	// UE LOG the price comparision
	UE_LOG(LogTemp, Warning, TEXT("Comparing price of %s with %d gold"), *ShipComponentProperty->ComponentName, Gold);
	if (ShipComponentProperty->ComponentCost <= Gold)
	{
		// White if can afford
		ComponentCost->SetColorAndOpacity(FLinearColor(0.956f, 0.896f, 0.723f));
	}
	else
	{
		// Red if can't afford
		ComponentCost->SetColorAndOpacity(FLinearColor(0.352f, 0.051f, 0.051f));
	}
}
