// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ExecuteProperty.generated.h"

class AShipBase;
class AProjectileBase;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OCEANITYGAME_API UExecuteProperty : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "ExecuteProperty")
	void ExecuteProperty(AShipBase* Parent);
};
