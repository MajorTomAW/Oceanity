// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OceanityGame/PlayerController/Lobby/LobbyController.h"
#include "LobbyGameMode.generated.h"
/**
 * 
 */
UCLASS()
class OCEANITYGAME_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	// Called when a player connects, override to handle
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Called when a player disconnects, override to handle
	virtual void Logout(AController* Exiting) override;

	// Called to replicate variables to clients
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called to update all connected clients
	UFUNCTION(BlueprintCallable)
	void UpdateConnectedClients();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateConnectedClients();
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<ALobbyController*> ConnectedControllers;
};
