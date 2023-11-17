// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.


#include "LobbyPlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "OceanityGame/GameModes/LobbyGameMode.h"

void ALobbyPlayerState::SetPlayerInventory_Implementation(const FPlayerInventory& NewPlayerInventory)
{
	PlayerInventory = NewPlayerInventory;
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyPlayerState, PlayerInventory);
	DOREPLIFETIME(ALobbyPlayerState, bIsReady);
}

void ALobbyPlayerState::EquipTurretComponent_Implementation(const FTurretComponentProperty& TurretComponent)
{
	PlayerInventory.EquippedTurretComponent = TurretComponent;
}

void ALobbyPlayerState::EquipHullComponent_Implementation(const FHullComponentProperty& HullComponent)
{
	PlayerInventory.EquippedHullComponent = HullComponent;
}

void ALobbyPlayerState::EquipEngineComponent_Implementation(const FEngineComponentProperty& EngineComponent)
{
	PlayerInventory.EquippedEngineComponent = EngineComponent;
}

void ALobbyPlayerState::SetIsReady_Implementation(bool NewIsReady)
{
	bIsReady = NewIsReady;
	Cast<ALobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->UpdateConnectedClients();
}
