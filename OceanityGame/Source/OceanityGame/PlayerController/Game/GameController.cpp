// Fill out your copyright notice in the Description page of Project Settings.


#include "GameController.h"

void AGameController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Client_CreateGameOverlay();
}

void AGameController::ShowScopeUI_Implementation(bool bShow)
{
	if (IsLocalController())
	{
		Client_ShowScopeUI(bShow);
	}	
}

AGameController* AGameController::GetGameController_Implementation()
{
	return this;
}

void AGameController::Client_CreateGameOverlay_Implementation()
{
	// Create Widget
	if (WidgetClass)
	{
		Widget = CreateWidget<UGameOverlay>(this, WidgetClass);
		Widget->AddToViewport();

		// Set Input Mode to game only
		const FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		SetShowMouseCursor(false);
	}
}

void AGameController::Client_ShowScopeUI_Implementation(bool bShow)
{
	if (Widget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client_ShowScopeUI_Implementation"));
		Widget->ShowScope(bShow);
	}
}

