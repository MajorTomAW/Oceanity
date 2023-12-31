// Copyright © 2023 MajorT. All rights reserved.


#include "OceanityHealthExecution.h"

#include "AbilitySystemComponent.h"
#include "OceanityGame/AbilitySystem/AttributeSet/ShipAttributeSet.h"
#include "OceanityGame/Characters/CharacterBase.h"
#include "OceanityGame/Characters/Ships/ShipBase.h"


UOceanityHealthExecution::UOceanityHealthExecution()
{
	AttributeStruct Attributes;
	RelevantAttributesToCapture.Add(Attributes.HealthDef);
}

void UOceanityHealthExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	const AttributeStruct Attributes;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Get base damage
	float BaseDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Attributes.HealthDef, EvaluateParameters, BaseDamage);

	// Get damage reduction
	const float DamageReduction = Cast<AShipBase>(TargetActor)->ShipProperty.HullComponent.DamageReduction;

	// Calculate final damage
	const float DamageDone = BaseDamage * (1 - DamageReduction);

	UE_LOG(LogTemp, Error, TEXT("DamageDone: %f"), DamageDone);

	if (DamageDone > 0.f)
	{
		// Apply damage modifier
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UShipAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -DamageDone));
	}
}
