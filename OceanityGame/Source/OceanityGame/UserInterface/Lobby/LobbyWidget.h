// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "OceanityGame/Libraries/UILibrary.h"
#include "OceanityGame/PlayerStates/Lobby/LobbyPlayerState.h"
#include "LobbyWidget.generated.h"

class UStatsWidget;
class UShipComponentPanel;
/**
 * 
 */
UCLASS()
class OCEANITYGAME_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* ComponentList;

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UShipComponentPanel> ShipComponentPanelClass;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButtonBase* TurretComponentsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButtonBase* EngineComponentsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButtonBase* HullComponentsButton;

	UPROPERTY(BlueprintReadWrite)
	UStatsWidget* StatsWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButtonBase* BuyEquipButton;
	
	
	// Constructor
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetActiveComponentLayout(EActiveComponentLayout NewActiveComponentLayout);

	void UpdateListedComponents(UDataTable* ComponentTable, EActiveComponentLayout ComponentType);

	void SelectNewComponentPanel(UShipComponentPanel& NewComponentPanel);

	UFUNCTION()
	void OnBuyEquipButtonClicked();

	UFUNCTION(BlueprintImplementableEvent)
	void OnComponentBought(int NewGold);

	void UpdateLobbyUI(FPlayerInventory NewPlayerInventory);

	void UpdateConnectedPlayers(TArray<ALobbyPlayerState*> ConnectedPlayers);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnLobbyUpdated")
	void OnLobbyUIUpdated(FPlayerInventory NewPlayerInventory);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnConnectedPlayersUpdated")
	void OnConnectedPlayersUpdated();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnComponentSelected")
	void OnComponentSelected(FShipComponentProperty ShipComponentProperty);

	UPROPERTY(BlueprintReadOnly)
	TArray<ALobbyPlayerState*> ConnectedPlayerStates;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* TurretComponents;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* EngineComponents;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* HullComponents;

private:
	UShipComponentPanel* SelectedComponentPanel;

	FPlayerInventory* PlayerInventory;

public:
	UPROPERTY(BlueprintReadWrite)
	ALobbyPlayerState* LobbyPlayerState;

	UPROPERTY(BlueprintReadWrite)
	EActiveComponentLayout ActiveComponentLayout = EActiveComponentLayout::TurretComponents;
};
