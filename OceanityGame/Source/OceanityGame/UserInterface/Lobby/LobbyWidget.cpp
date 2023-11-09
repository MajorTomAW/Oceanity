// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "AsyncTreeDifferences.h"
#include "OceanityGame/PlayerStates/Lobby/LobbyPlayerState.h"
#include "OceanityGame/UserInterface/Panels/ShipComponents/ShipComponentPanel.h"

struct FEngineComponentProperty;
struct FHullComponentProperty;
struct FTurretComponentProperty;

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetActiveComponentLayout(ActiveComponentLayout);

	LobbyPlayerState = Cast<ALobbyPlayerState>(GetOwningPlayerState());
}

void ULobbyWidget::SetActiveComponentLayout(EActiveComponentLayout NewActiveComponentLayout)
{
	ActiveComponentLayout = NewActiveComponentLayout;

	switch (ActiveComponentLayout)
	{
	case EActiveComponentLayout::TurretComponents:
		UpdateListedComponents(TurretComponents, ActiveComponentLayout);
		break;
	case EActiveComponentLayout::EngineComponents:
		UpdateListedComponents(EngineComponents, ActiveComponentLayout);
		break;
	case EActiveComponentLayout::HullComponents:
		UpdateListedComponents(HullComponents, ActiveComponentLayout);
		break;
	}
}

void ULobbyWidget::UpdateListedComponents(UDataTable* ComponentTable, EActiveComponentLayout ComponentType)
{
	ComponentList->ClearChildren();

	TArray<FName> RowNames = ComponentTable->GetRowNames();

	for (const FName RowName : RowNames)
	{
		UShipComponentPanel* ShipComponentPanel = CreateWidget<UShipComponentPanel>(GetWorld(), ShipComponentPanelClass);
		ShipComponentPanel->LinkShipComponent(ComponentTable->FindRow<FShipComponentProperty>(RowName, ""));
	
		if (LobbyPlayerState)
		{
			switch (ComponentType)
			{
			case EActiveComponentLayout::EngineComponents:
				if (const FEngineComponentProperty* EngineComponentProperty = ComponentTable->FindRow<FEngineComponentProperty>(RowName, ""))
				{
					ShipComponentPanel->SetIsComponentOwned(LobbyPlayerState->PlayerInventory.UnlockedEngineComponents.Contains(*EngineComponentProperty));
				}
				break;
			case EActiveComponentLayout::HullComponents:
				if (const FHullComponentProperty* HullComponentProperty = ComponentTable->FindRow<FHullComponentProperty>(RowName, ""))
				{
					ShipComponentPanel->SetIsComponentOwned(LobbyPlayerState->PlayerInventory.UnlockedHullComponents.Contains(*HullComponentProperty));
				}
				break;
			case EActiveComponentLayout::TurretComponents:
				if (const FTurretComponentProperty* TurretComponentProperty = ComponentTable->FindRow<FTurretComponentProperty>(RowName, ""))
				{
					ShipComponentPanel->SetIsComponentOwned(LobbyPlayerState->PlayerInventory.UnlockedTurretComponents.Contains(*TurretComponentProperty));
				}
				break;
			}

			ShipComponentPanel->ComparePrice(LobbyPlayerState->PlayerInventory.Gold);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LobbyPlayerState is null"));
		}
		
		ComponentList->AddChild(ShipComponentPanel);
	}
}
