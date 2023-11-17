// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.


#include "LobbyGameMode.h"
#include "Net/UnrealNetwork.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ALobbyController* LobbyController = Cast<ALobbyController>(NewPlayer);
	//LobbyController->Client_CreateLobbyUI();
	ConnectedControllers.AddUnique(LobbyController);
	UpdateConnectedClients();
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ConnectedControllers.Remove(Cast<ALobbyController>(Exiting));
	UpdateConnectedClients();
}

void ALobbyGameMode::UpdateConnectedClients()
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateConnectedClients"));
	OnUpdateConnectedClients();
}

void ALobbyGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyGameMode, ConnectedControllers);
}
