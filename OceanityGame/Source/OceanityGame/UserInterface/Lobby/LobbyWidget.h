// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "OceanityGame/Libraries/UILibrary.h"
#include "OceanityGame/PlayerStates/Lobby/LobbyPlayerState.h"
#include "LobbyWidget.generated.h"

class UShipComponentPanel;
/**
 * 
 */
UCLASS()
class OCEANITYGAME_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* ComponentList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UShipComponentPanel> ShipComponentPanelClass;
	
	
	// Constructor
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetActiveComponentLayout(EActiveComponentLayout NewActiveComponentLayout);

	void UpdateListedComponents(UDataTable* ComponentTable, EActiveComponentLayout ComponentType);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* TurretComponents;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* EngineComponents;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* HullComponents;

private:
	EActiveComponentLayout ActiveComponentLayout = EActiveComponentLayout::TurretComponents;

	ALobbyPlayerState* LobbyPlayerState;
};
