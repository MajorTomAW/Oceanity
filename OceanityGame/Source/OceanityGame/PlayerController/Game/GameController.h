// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OceanityGame/Interfaces/ControllerInterface.h"
#include "OceanityGame/UserInterface/Game/GameOverlay.h"
#include "GameController.generated.h"

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API AGameController : public APlayerController, public IControllerInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Game|UI")
	TSubclassOf<UGameOverlay> WidgetClass;
	
	// Called when the game starts or when spawned
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UGameOverlay* Widget;

public:
	// IControllerInterface override all functions
	virtual void ShowScopeUI_Implementation(bool bShow) override;
	virtual AGameController* GetGameController_Implementation() override;

	UFUNCTION(Client, Reliable)
	void Client_ShowScopeUI(bool bShow);

	UFUNCTION(Client, Reliable)
	void Client_CreateGameOverlay();
};
