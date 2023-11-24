// Copyright © 2023 MajorT. All rights reserved.


#include "ShipAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "OceanityGame/Characters/CharacterBase.h"

UShipAttributeSet::UShipAttributeSet()
{
}

void UShipAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle ContextHandle = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	float DeltaValue {0.f};

	// Get positive delta value
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive)
	{
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	// Get source actor
	ACharacterBase* SourceCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceCharacter = Cast<ACharacterBase>(Data.Target.AbilityActorInfo->AvatarActor.Get());
	}

	// Clamp Values
	// Health
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

		if (SourceCharacter)
		{
			SourceCharacter->HandleHealthChanged(DeltaValue, Data.EvaluatedData.Magnitude, SourceTags);
		}
	}
}

void UShipAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// If a Max value changes, adjust current to keep Current % of Current to Max
	if (Attribute == GetMaxHealthAttribute()) // GetMaxHealthAttribute comes from the Macros defined at the top of the header
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void UShipAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData & AffectedAttribute, const FGameplayAttributeData & MaxAttribute, float NewMaxValue, const FGameplayAttribute & AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurrentMaxValue > 0.f)
			                       ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue
			                       : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

/** Replicated using of attributes */
void UShipAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShipAttributeSet, Health, OldHealth);
}

void UShipAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShipAttributeSet, MaxHealth, OldMaxHealth);
}

void UShipAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShipAttributeSet, Damage, OldDamage);
}

void UShipAttributeSet::OnRep_Ammo(const FGameplayAttributeData& OldAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShipAttributeSet, Ammo, OldAmmo);
}

void UShipAttributeSet::OnRep_FireCooldown(const FGameplayAttributeData& OldFireCooldown)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShipAttributeSet, FireCooldown, OldFireCooldown);
}

void UShipAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UShipAttributeSet, Health);
	DOREPLIFETIME(UShipAttributeSet, MaxHealth);
	DOREPLIFETIME(UShipAttributeSet, Damage);
	DOREPLIFETIME(UShipAttributeSet, Ammo);
	DOREPLIFETIME(UShipAttributeSet, FireCooldown);
}
