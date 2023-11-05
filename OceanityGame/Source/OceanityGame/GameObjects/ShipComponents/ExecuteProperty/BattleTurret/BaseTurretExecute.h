// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OceanityGame/GameObjects/ShipComponents/ExecuteProperty/ExecuteProperty.h"
#include "BaseTurretExecute.generated.h"

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UBaseTurretExecute : public UExecuteProperty
{
	GENERATED_BODY()

public:
	virtual void ExecuteProperty_Implementation(AShipBase* Parent, bool& Success) override;
};
