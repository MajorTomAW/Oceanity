// Copyright © 2023 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "ShipAttributeSet.generated.h"

// User macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UShipAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UShipAttributeSet();

	// Called when gameplay effect is executed
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void AdjustAttributeForMaxChange(FGameplayAttributeData & AffectedAttribute, const FGameplayAttributeData & MaxAttribute, float NewMaxValue, const FGameplayAttribute & AffectedAttributeProperty);
	
	/** Attributes */
	// Health
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes|Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UShipAttributeSet, Health);

	// Max Health
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes|Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UShipAttributeSet, MaxHealth);

	// Damage
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Damage, Category = "Attributes|Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UShipAttributeSet, Damage);

	// Ammo
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Ammo, Category = "Attributes|Ammo")
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS(UShipAttributeSet, Ammo);

	// Fire Cooldown
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireCooldown, Category = "Attributes|Fire Cooldown")
	FGameplayAttributeData FireCooldown;
	ATTRIBUTE_ACCESSORS(UShipAttributeSet, FireCooldown);

	/** Replication */
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData& OldDamage);

	UFUNCTION()
	virtual void OnRep_Ammo(const FGameplayAttributeData& OldAmmo);

	UFUNCTION()
	virtual void OnRep_FireCooldown(const FGameplayAttributeData& OldFireCooldown);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
