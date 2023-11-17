// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OceanityGame/UserInterface/Lobby/LobbyWidget.h"
#include "LobbyController.generated.h"

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API ALobbyController : public APlayerController
{
	GENERATED_BODY()

public:
	// Called to create the lobby UI
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_CreateLobbyUI();

	// Called to update the lobby UI on server
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_UpdateLobbyUI();

	// Called on possession to create the lobby UI
	virtual void OnPossess(APawn* InPawn) override;
	
	// Called to update the lobby UI on client
	UFUNCTION(Client, Reliable)
	void Client_UpdateLobbyUI(FPlayerInventory PlayerInventory);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LobbyController")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;

private:
	ULobbyWidget* LobbyWidget;
};
