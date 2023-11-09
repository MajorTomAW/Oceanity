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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LobbyPlayerState")
	FPlayerInventory PlayerInventory;
};
