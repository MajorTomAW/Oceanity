// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OceanityGame/Libraries/PlayerLibrary.h"
#include "LobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "LobbyPlayerState")
	FPlayerInventory PlayerInventory;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetPlayerInventory(const FPlayerInventory& NewPlayerInventory);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "LobbyPlayerState")
	bool bIsReady = false;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetIsReady(bool NewIsReady);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void EquipEngineComponent(const FEngineComponentProperty& EngineComponent);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void EquipHullComponent(const FHullComponentProperty& HullComponent);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void EquipTurretComponent(const FTurretComponentProperty& TurretComponent);
};
