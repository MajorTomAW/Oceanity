// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "OceanityGame/Libraries/ShipLibrary.h"
#include "OceanityGame/UserInterface/Panels/ShipComponents/ShipComponentPanel.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetActiveComponentLayout(ActiveComponentLayout);
}

void ULobbyWidget::SetActiveComponentLayout(EActiveComponentLayout NewActiveComponentLayout)
{
	ActiveComponentLayout = NewActiveComponentLayout;

	switch (ActiveComponentLayout)
	{
	case EActiveComponentLayout::TurretComponents:
		UpdateListedComponents(TurretComponents);
		break;
	case EActiveComponentLayout::EngineComponents:
		UpdateListedComponents(EngineComponents);
		break;
	case EActiveComponentLayout::HullComponents:
		UpdateListedComponents(HullComponents);
		break;
	}
}

void ULobbyWidget::UpdateListedComponents(UDataTable* ComponentTable)
{
	ComponentList->ClearChildren();

	TArray<FName> RowNames = ComponentTable->GetRowNames();

	for (const FName RowName : RowNames)
	{
		const FShipComponentProperty* ShipComponentProperty = ComponentTable->FindRow<FShipComponentProperty>(RowName, "");
		UShipComponentPanel* ShipComponentPanel = CreateWidget<UShipComponentPanel>(GetWorld(), ShipComponentPanelClass);

		ShipComponentPanel->LinkShipComponent(ShipComponentProperty);
		ComponentList->AddChild(ShipComponentPanel);
	}
}
