// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "Kismet/GameplayStatics.h"
#include "OceanityGame/Characters/Ships/ShipBase.h"
#include "OceanityGame/Interfaces/PlayerInterface.h"
#include "OceanityGame/PlayerStates/Lobby/LobbyPlayerState.h"
#include "OceanityGame/UserInterface/BaseClasses/Buttons/ButtonBase.h"
#include "OceanityGame/UserInterface/Panels/ShipComponents/ShipComponentPanel.h"

struct FEngineComponentProperty;
struct FHullComponentProperty;
struct FTurretComponentProperty;

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LobbyPlayerState = Cast<ALobbyPlayerState>(GetOwningPlayer()->PlayerState);

	BuyEquipButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnBuyEquipButtonClicked);
}

void ULobbyWidget::SetActiveComponentLayout(EActiveComponentLayout NewActiveComponentLayout)
{
	ActiveComponentLayout = NewActiveComponentLayout;

	switch (ActiveComponentLayout)
	{
	case EActiveComponentLayout::TurretComponents:
		UpdateListedComponents(TurretComponents, ActiveComponentLayout);

		// Set button active
		TurretComponentsButton->SetButtonActive(true);
		EngineComponentsButton->SetButtonActive(false);
		HullComponentsButton->SetButtonActive(false);
		break;
	case EActiveComponentLayout::EngineComponents:
		UpdateListedComponents(EngineComponents, ActiveComponentLayout);

		// Set button active
		TurretComponentsButton->SetButtonActive(false);
		EngineComponentsButton->SetButtonActive(true);
		HullComponentsButton->SetButtonActive(false);
		break;
	case EActiveComponentLayout::HullComponents:
		UpdateListedComponents(HullComponents, ActiveComponentLayout);

		// Set button active
		TurretComponentsButton->SetButtonActive(false);
		EngineComponentsButton->SetButtonActive(false);
		HullComponentsButton->SetButtonActive(true);
		break;
	}
}

void ULobbyWidget::UpdateListedComponents(UDataTable* ComponentTable, EActiveComponentLayout ComponentType)
{
	if (!LobbyPlayerState) return;
	
	ComponentList->ClearChildren();

	TArray<FName> RowNames = ComponentTable->GetRowNames();

	for (const FName RowName : RowNames)
	{
		UShipComponentPanel* ShipComponentPanel = CreateWidget<UShipComponentPanel>(GetWorld(), ShipComponentPanelClass);
		ShipComponentPanel->LinkShipComponent(ComponentTable->FindRow<FShipComponentProperty>(RowName, ""), RowName);
		ShipComponentPanel->SetOwner(this);
		
		if (SelectedComponentPanel)
		{
			if (SelectedComponentPanel->GetShipComponentProperty()->ComponentId == ShipComponentPanel->GetShipComponentProperty()->ComponentId)
			{
				SelectNewComponentPanel(*ShipComponentPanel);
			}
		}
		
		ShipComponentPanel->SetIsComponentOwned(UPlayerLibrary::IsComponentOwned(*ShipComponentPanel->GetShipComponentProperty(), LobbyPlayerState->PlayerInventory));
		ShipComponentPanel->ComparePrice(LobbyPlayerState->PlayerInventory.Gold);
		
		ComponentList->AddChild(ShipComponentPanel);
	}
}

void ULobbyWidget::SelectNewComponentPanel(UShipComponentPanel& NewComponentPanel)
{
	if (!IsValid(&NewComponentPanel) || !LobbyPlayerState) return;
	
	UE_LOG(LogTemp, Warning, TEXT("Handle new component panel"));
	if (SelectedComponentPanel)
	{
		SelectedComponentPanel->ComponentButton->SetIsEnabled(true);
		SelectedComponentPanel->PanelEnabled(true);
	}
	
	SelectedComponentPanel = &NewComponentPanel;
	SelectedComponentPanel->ComponentButton->SetIsEnabled(false);
	UE_LOG(LogTemp, Warning, TEXT("Disable new component panel"));
	SelectedComponentPanel->PanelEnabled(false);

	// Logic if can get bought
	UE_LOG(LogTemp, Warning, TEXT("Logic if can get bought"));
	if (UPlayerLibrary::IsComponentOwned(*SelectedComponentPanel->GetShipComponentProperty(), LobbyPlayerState->PlayerInventory))
	{
			UE_LOG(LogTemp, Warning, TEXT("Component is owned"));
		BuyEquipButton->Button->SetIsEnabled(true);
		BuyEquipButton->TextBlock->SetText(FText::FromString("Equip"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Component is not owned"));
		if (SelectedComponentPanel->GetShipComponentProperty()->ComponentCost <= LobbyPlayerState->PlayerInventory.Gold)
		{
			BuyEquipButton->Button->SetIsEnabled(true);
			BuyEquipButton->TextBlock->SetText(FText::FromString("Buy"));
		}
		else
		{
			BuyEquipButton->Button->SetIsEnabled(false);
			BuyEquipButton->TextBlock->SetText(FText::FromString("Buy"));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("--Finished--"));
}

void ULobbyWidget::OnBuyEquipButtonClicked()
{
	BuyEquipButton->Button->SetIsEnabled(false);
	
	if (!SelectedComponentPanel || !LobbyPlayerState) return;

	// If component is owned, equip it
	if (UPlayerLibrary::IsComponentOwned(*SelectedComponentPanel->GetShipComponentProperty(), LobbyPlayerState->PlayerInventory))
	{
		// Get AShipBase Actor of class
		AShipBase* ShipBase = Cast<AShipBase>(UGameplayStatics::GetActorOfClass(GetWorld(), AShipBase::StaticClass()));
		IPlayerInterface::Execute_EquipComponent(ShipBase, *SelectedComponentPanel->GetShipComponentProperty());

		// Set Equipped Component
		switch (SelectedComponentPanel->GetShipComponentProperty()->ComponentType)
		{
		case EShipComponentType::Engine:
			{
				const FEngineComponentProperty* EngineToAdd = EngineComponents->FindRow<FEngineComponentProperty>(SelectedComponentPanel->ThisRowName, "");
				LobbyPlayerState->EquipEngineComponent(*EngineToAdd);
				break;
			}
		case EShipComponentType::Hull:
			{
				const FHullComponentProperty* HullToAdd = HullComponents->FindRow<FHullComponentProperty>(SelectedComponentPanel->ThisRowName, "");
				LobbyPlayerState->EquipHullComponent(*HullToAdd);
				break;
			}
		case EShipComponentType::Turret:
			{
				const FTurretComponentProperty* TurretToAdd = TurretComponents->FindRow<FTurretComponentProperty>(SelectedComponentPanel->ThisRowName, "");
				LobbyPlayerState->EquipTurretComponent(*TurretToAdd);
				break;
			}
		}
		
	}
	else
	{
		// If component is not owned, buy it. Make sure the player has enough gold
		if (SelectedComponentPanel->GetShipComponentProperty()->ComponentCost <= LobbyPlayerState->PlayerInventory.Gold)
		{
			FPlayerInventory NewPlayerInventory = LobbyPlayerState->PlayerInventory;
			NewPlayerInventory.Gold -= SelectedComponentPanel->GetShipComponentProperty()->ComponentCost;

			switch (SelectedComponentPanel->GetShipComponentProperty()->ComponentType)
			{
				case EShipComponentType::Engine:
					{
						const FEngineComponentProperty* EngineToAdd = EngineComponents->FindRow<FEngineComponentProperty>(SelectedComponentPanel->ThisRowName, "");
						NewPlayerInventory.UnlockedEngineComponents.Add(*EngineToAdd);
						LobbyPlayerState->SetPlayerInventory(NewPlayerInventory);
						//UpdateListedComponents(EngineComponents, EActiveComponentLayout::EngineComponents);
						break;
					}
				case EShipComponentType::Hull:
					{
						const FHullComponentProperty* HullToAdd = HullComponents->FindRow<FHullComponentProperty>(SelectedComponentPanel->ThisRowName, "");
						NewPlayerInventory.UnlockedHullComponents.Add(*HullToAdd);
						LobbyPlayerState->SetPlayerInventory(NewPlayerInventory);
						//UpdateListedComponents(HullComponents, EActiveComponentLayout::HullComponents);
						break;
					}
				case EShipComponentType::Turret:
					{
						const FTurretComponentProperty* TurretToAdd = TurretComponents->FindRow<FTurretComponentProperty>(SelectedComponentPanel->ThisRowName, "");
						NewPlayerInventory.UnlockedTurretComponents.Add(*TurretToAdd);
						LobbyPlayerState->SetPlayerInventory(NewPlayerInventory);
						//UpdateListedComponents(TurretComponents, EActiveComponentLayout::TurretComponents);
						break;
					}
			}

			OnComponentBought(NewPlayerInventory.Gold);
		}
	}
}

void ULobbyWidget::UpdateLobbyUI(FPlayerInventory NewPlayerInventory)
{
	PlayerInventory = &NewPlayerInventory;
	OnLobbyUIUpdated(NewPlayerInventory);
}

void ULobbyWidget::UpdateConnectedPlayers(TArray<ALobbyPlayerState*> ConnectedPlayers)
{
	ConnectedPlayerStates = ConnectedPlayers;
	OnConnectedPlayersUpdated();

	// Check if all players are ready
	bool bAllPlayersReady = true;
	for (const ALobbyPlayerState* PlayerState : ConnectedPlayers)
	{
		if (!PlayerState->bIsReady)
		{
			bAllPlayersReady = false;
			break;
		}
	}

	if (bAllPlayersReady)
	{
		// Start Game
	}
}
