// Copyright © 2023 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "OceanityGame/AbilitySystem/AttributeSet/ShipAttributeSet.h"
#include "OceanityHealthExecution.generated.h"

struct AttributeStruct
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	AttributeStruct()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UShipAttributeSet, Health, Target, false);
	}
};

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UOceanityHealthExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UOceanityHealthExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
