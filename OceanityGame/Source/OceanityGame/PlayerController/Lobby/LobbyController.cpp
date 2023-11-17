// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.


#include "LobbyController.h"

#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "OceanityGame/GameModes/LobbyGameMode.h"

void ALobbyController::Client_CreateLobbyUI_Implementation()
{
	if (!IsLocalController()) return;
	UE_LOG(LogTemp, Warning, TEXT("Client_CreateLobbyUI_Implementation"));
	LobbyWidget = CreateWidget<ULobbyWidget>(GetWorld(), LobbyWidgetClass);
	LobbyWidget->AddToViewport();

	FInputModeUIOnly InputModeGameAndUI;
	InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);

	SetInputMode(InputModeGameAndUI);
	SetShowMouseCursor(true);
		
	//Cast<ALobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->UpdateConnectedClients();
}

void ALobbyController::Server_UpdateLobbyUI_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server_UpdateLobbyUI_Implementation"));
	if (ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState))
	{
		Client_UpdateLobbyUI(LobbyPlayerState->PlayerInventory);
	}
}

void ALobbyController::Client_UpdateLobbyUI_Implementation(FPlayerInventory PlayerInventory)
{
	if (IsValid(LobbyWidget))
	{
		TArray<ALobbyPlayerState*> LobbyPlayerStates;
		for (APlayerState* NewPlayerState : GetWorld()->GetGameState()->PlayerArray)
		{
			LobbyPlayerStates.AddUnique(Cast<ALobbyPlayerState>(NewPlayerState));
		}
		
		LobbyWidget->UpdateLobbyUI(PlayerInventory);
		LobbyWidget->UpdateConnectedPlayers(LobbyPlayerStates);
	}
}

void ALobbyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	Client_CreateLobbyUI();
}
