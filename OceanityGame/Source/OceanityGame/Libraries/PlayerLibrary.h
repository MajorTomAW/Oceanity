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
	int32 Gold = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerId = ""; 

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

public:
	UFUNCTION(BlueprintCallable)
	static bool IsComponentOwned(const FShipComponentProperty ShipComponentProperty, const FPlayerInventory& PlayerInventory)
	{
		switch (ShipComponentProperty.ComponentType)
		{
			case EShipComponentType::Engine:
				return PlayerInventory.UnlockedEngineComponents.ContainsByPredicate([&](const FEngineComponentProperty& EngineComponentProperty)
				{
					return EngineComponentProperty.ComponentId == ShipComponentProperty.ComponentId;
				});
			case EShipComponentType::Hull:
				return PlayerInventory.UnlockedHullComponents.ContainsByPredicate([&](const FHullComponentProperty& HullComponentProperty)
				{
					return HullComponentProperty.ComponentId == ShipComponentProperty.ComponentId;
				});
			case EShipComponentType::Turret:
				return PlayerInventory.UnlockedTurretComponents.ContainsByPredicate([&](const FTurretComponentProperty& TurretComponentProperty)
				{
					return TurretComponentProperty.ComponentId == ShipComponentProperty.ComponentId;
				});
		}
		return false;
	};
};
