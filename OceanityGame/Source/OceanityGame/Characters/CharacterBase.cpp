// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "OceanityGame/Components/AbilityComponent/OceanityAbilityComponent.h"
#include "OceanityGame/AbilitySystem/GameplayAbility/OceanityAbility.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	bReplicates = true;
	GetCharacterMovement()->SetIsReplicated(true);
	bAsyncPhysicsTickEnabled = true;

	// Disable control rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Ability System Component
	OceanityAbilityComponent = CreateDefaultSubobject<UOceanityAbilityComponent>(TEXT("Ability System Component"));
	OceanityAbilityComponent->SetIsReplicated(true);
	OceanityAbilityComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UShipAttributeSet>(TEXT("Attributes"));
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute Set is null"));
	}
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Set Ability System Component
	if (OceanityAbilityComponent)
	{
		OceanityAbilityComponent->InitAbilityActorInfo(this, this);
	}
}

void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Set Ability System Component
	if (OceanityAbilityComponent)
	{
		OceanityAbilityComponent->InitAbilityActorInfo(this, this);
	}
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, ShipProperty);
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return OceanityAbilityComponent;
}

void ACharacterBase::InitializeAttributes()
{
	check(OceanityAbilityComponent);

	UE_LOG(LogTemp, Warning, TEXT("Initializing Attributes..."));

	if (GetLocalRole() == ROLE_Authority && !bAbilitiesInitialized)
	{
		// Grant Abilities
		for (TSubclassOf<UOceanityAbility>& StartupAbility : StartupAbilities)
		{
			OceanityAbilityComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, INDEX_NONE, this));
		}

		// Apply Startup Gameplay Effects
		for (const TSubclassOf<UGameplayEffect>& GameplayEffect : StartupGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContextHandle = OceanityAbilityComponent->MakeEffectContext();
			EffectContextHandle.AddSourceObject(this);

			FGameplayEffectSpecHandle EffectSpecHandle = OceanityAbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContextHandle);
			/*EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Initializer.Health"), ShipProperty.HullComponent.Health);
			EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Initializer.MaxHealth"), ShipProperty.HullComponent.Health);
			EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Initializer.Shield"), ShipProperty.HullComponent.DamageReduction);
			EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Initializer.Ammo"), ShipProperty.TurretComponent.Ammo);
			EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Initializer.Damage"), ShipProperty.TurretComponent.ProjectileProperty.Damage);
			EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Initializer.FireCooldown"), ShipProperty.TurretComponent.ShootCooldown);*/

			if (EffectSpecHandle.IsValid())
			{
				OceanityAbilityComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
			}

			UE_LOG(LogTemp, Warning, TEXT("Health: %f"), ShipProperty.HullComponent.Health);
		}

		bAbilitiesInitialized = true;
	}
}

float ACharacterBase::GetHealth()
{
	if (AttributeSet)
	{
		return AttributeSet->GetHealth();
	}
	return -1.f;
}

void ACharacterBase::HandleDamage(float DamageAmount, const FHitResult& HitInfo,
                                  const FGameplayTagContainer& DamageTags, ACharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}

void ACharacterBase::HandleHealthChanged(float DeltaValue, float NewHealth, const FGameplayTagContainer& DamageTags)
{
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue, NewHealth, DamageTags);
	}
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

