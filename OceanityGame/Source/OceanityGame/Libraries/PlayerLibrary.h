// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShipLibrary.h"
#include "PlayerLibrary.generated.h"


/** Player Inventory*/
USTRUCT(BlueprintType)
struct FPlayerInventory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Gold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTurretComponentProperty> UnlockedTurretComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEngineComponentProperty> UnlockedEngineComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHullComponentProperty> UnlockedHullComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTurretComponentProperty EquippedTurretComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEngineComponentProperty EquippedEngineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHullComponentProperty EquippedHullComponent;
};
/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UPlayerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
