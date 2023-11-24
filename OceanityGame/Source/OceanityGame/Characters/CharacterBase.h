// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "OceanityGame/Libraries/ShipLibrary.h"
#include "OceanityGame/AbilitySystem/AttributeSet/ShipAttributeSet.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class UOceanityAbility;
class UOceanityAbilityComponent;

UCLASS()
class OCEANITYGAME_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	/**
	 * 
	 */
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the controller is set
	virtual void PossessedBy(AController* NewController) override;

	// Called when the player state is replicated
	virtual void OnRep_PlayerState() override;

	// Called to replicate actor
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Ability System */
	// Ability System Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Ability System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOceanityAbilityComponent> OceanityAbilityComponent;

	// Ability System Attributes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UShipAttributeSet> AttributeSet;

	// Startup Gameplay Effects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "_Ability System")
	TArray<TSubclassOf<UGameplayEffect>> StartupGameplayEffects;

	// Startup Abilities
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ability System")
	TArray<TSubclassOf<UOceanityAbility>> StartupAbilities;

	// Ability System Init
	UFUNCTION(BlueprintCallable)
	void InitializeAttributes();

	// Ability System Init
	bool bAbilitiesInitialized = false;

	// Blueprint Implementable Events
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ACharacterBase* InstigatorCharacter, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float DeltaValue, float NewHealth, const struct FGameplayTagContainer& DamageTags);

	UFUNCTION(BlueprintCallable)
	virtual float GetHealth();

	// Attribute Set functions
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ACharacterBase* InstigatorCharacter, AActor* DamageCauser);

	virtual void HandleHealthChanged(float DeltaValue, float NewHealth, const struct FGameplayTagContainer& DamageTags);

	//friend UShipAttributeSet;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Ship property */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "_Ship")
	FShipProperty ShipProperty;
};
