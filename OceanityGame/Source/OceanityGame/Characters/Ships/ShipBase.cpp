// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "OceanityGame/Components/AbilityComponent/OceanityAbilityComponent.h"
#include "OceanityGame/GameObjects/Projectiles/ProjectileBase.h"
#include "OceanityGame/Interfaces/ControllerInterface.h"

class UExecuteProperty;

// Sets default values
AShipBase::AShipBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create components */
	// Capsule Component
	GetCapsuleComponent()->SetAngularDamping(0.5f);
	GetCapsuleComponent()->SetIsReplicated(true);
	GetCapsuleComponent()->SetSimulatePhysics(true);
	
	// Hull
	ShipBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipBody"));
	ShipBody->SetupAttachment(RootComponent);

	// Cabin
	ShipCabin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipCabin"));
	ShipCabin->BodyInstance.bOverrideMass = true;
	ShipCabin->BodyInstance.SetMassOverride(0.f);
	ShipCabin->SetCollisionProfileName(TEXT("NoCollision"), false);
	ShipCabin->SetupAttachment(ShipBody, TEXT("CabinSocket"));

	// Turret Base
	TurretBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	TurretBase->BodyInstance.bOverrideMass = true;
	TurretBase->BodyInstance.SetMassOverride(0.f);
	TurretBase->SetCollisionProfileName(TEXT("NoCollision"), false);
	TurretBase->SetupAttachment(ShipCabin, TEXT("TurretSocket"));

	// Turret
	Turret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurrentGun"));
	Turret->BodyInstance.bOverrideMass = true;
	Turret->BodyInstance.SetMassOverride(0.f);
	Turret->SetCollisionProfileName(TEXT("NoCollision"), false);
	Turret->SetupAttachment(TurretBase, TEXT("TurretSocket"));

	// Engine
	Engine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Engine"));
	Engine->BodyInstance.bOverrideMass = true;
	Engine->BodyInstance.SetMassOverride(0.f);
	Engine->SetCollisionProfileName(TEXT("NoCollision"), false);
	Engine->SetupAttachment(ShipBody);
	
	ProjectileSpawn = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawn"));
	ProjectileSpawn->SetupAttachment(Turret);

	/** Camera logic */
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(Turret, TEXT("CameraSocket"));
	SpringArm->TargetArmLength = 1200.f ;
	SpringArm->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Outside Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bAutoActivate = true;

	// Add Ship tag
	Tags.Add("Ship");

	// Store all ship meshes
	ShipMeshes.Add(Engine);
	ShipMeshes.Add(ShipCabin);
	ShipMeshes.Add(TurretBase);
	ShipMeshes.Add(Turret);
	ShipMeshes.Add(ShipBody);
}

// Called when the game starts or when spawned

void AShipBase::BeginPlay()
{
	Super::BeginPlay();
}

void AShipBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	Client_EnableShipInput();
}

void AShipBase::Client_EnableShipInput_Implementation()
{
	// Add input mappings
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MovementContext, 0);
		}
	}

	// Set InputMode back to Game
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		const FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(false);
	}
}

void AShipBase::OverwriteShipStats_Implementation(FShipProperty NewShipProperty)
{
	ShipProperty = NewShipProperty;
	NetMulti_UpdateShipMeshes(ShipProperty);
}

void AShipBase::NetMulti_UpdateShipMeshes_Implementation(FShipProperty NewShipProperty)
{
	UStaticMesh* HullMesh = NewShipProperty.HullComponent.ComponentMesh;
	UStaticMesh* TurretMesh = NewShipProperty.TurretComponent.ComponentMesh;
	UStaticMesh* EngineMesh = NewShipProperty.EngineComponent.ComponentMesh;

	ShipBody->SetStaticMesh(HullMesh);
	ShipBody->BodyInstance.bOverrideMass = true;
	ShipBody->BodyInstance.SetMassOverride(0.f);
	
	ShipCabin->AttachToComponent(ShipBody, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("CabinSocket"));
	
	TurretBase->AttachToComponent(ShipCabin, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("TurretSocket"));
	
	Turret->SetStaticMesh(TurretMesh);
	Turret->AttachToComponent(TurretBase, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("TurretSocket"));
	
	Engine->SetStaticMesh(EngineMesh);
	Engine->AttachToComponent(ShipBody, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("EngineSocket"));
	
	SpringArm->AttachToComponent(Turret, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("CameraSocket"));

	// Apply new color
	for (UStaticMeshComponent* MeshComponent : ShipMeshes)
	{
		UMaterialInstanceDynamic* DynamicMaterialInst = UMaterialInstanceDynamic::Create(MeshComponent->GetMaterial(0), this);

		TMap<FMaterialParameterInfo, FMaterialParameterMetadata>& OutMaterial = *new TMap<FMaterialParameterInfo, FMaterialParameterMetadata>();
		DynamicMaterialInst->GetAllParametersOfType(EMaterialParameterType::Vector, OutMaterial);

		// Check if the OutMaterials has a parameter called "Color"
		if (DynamicMaterialInst == nullptr || !OutMaterial.Find(FMaterialParameterInfo(TEXT("Color"))))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s does not have a parameter called 'Color'"), *MeshComponent->GetStaticMesh()->GetName());
			continue;
		}
		
		DynamicMaterialInst->SetVectorParameterValue(TEXT("Color"), NewShipProperty.Color);
		MeshComponent->SetMaterial(0, DynamicMaterialInst);
	}
}

/** Enhanced Movement */
// Movement
void AShipBase::Turn(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.Get<float>() * (ShipProperty.EngineComponent.TurnSpeedForce / 100) * ShipProperty.EngineComponent.TurnSpeedMultiplier * GetWorld()->GetDeltaSeconds() * 100);
}

void AShipBase::Accelerate(const FInputActionValue& Value)
{
	if (Value.Get<float>() != 0) CalculateAcceleration(Value.Get<float>() * GetWorld()->GetDeltaSeconds() * 100);
}

void AShipBase::CalculateAcceleration(float Value)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		Server_CalculateAcceleration(Value);
	}
	
	// Calculate velocity
	const float TempVel = CurrentVelocity + (Value * ShipProperty.EngineComponent.Acceleration * GetWorld()->GetDeltaSeconds() * ShipProperty.HullComponent.HullMovementMultiplier);

	// Clamp velocity
	CurrentVelocity = FMath::Clamp(TempVel, -ShipProperty.EngineComponent.MaxAcceleration, ShipProperty.EngineComponent.MaxAcceleration);

	// Zero Buffer
	if (CurrentVelocity < ZeroBuffer && CurrentVelocity > -ZeroBuffer)
	{
		InputVelocity = 0.f;
	}
	else
	{
		CurrentVelocity > 0.f ? InputVelocity = 1.f : InputVelocity = -1.f;
	}

	// Apply new max walk speed
	const float NewMaxSpeed = FMath::Abs(CurrentVelocity < 0.f ? CurrentVelocity * ShipProperty.EngineComponent.BackwardsMultiplier : CurrentVelocity);
	GetCharacterMovement()->MaxWalkSpeed = NewMaxSpeed;

	// Debug print out CurrentVelocity and InputVelocity
	UE_LOG(LogTemp, Warning, TEXT("CurrentVelocity: %f, InputVelocity: %f"), GetCharacterMovement()->GetMaxSpeed(), InputVelocity);
}

void AShipBase::Server_CalculateAcceleration_Implementation(float Value)
{
	CalculateAcceleration(Value);
}

void AShipBase::Server_Move_Implementation(FVector2D InputValue)
{
	const float Velocity = GetVelocity().Length();
	const FEngineComponentProperty EngineComponent = ShipProperty.EngineComponent;
	
	const FVector ForceDirection = FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f);


	/** Turning 
	if (EngineComponent.bCanTurn && AngularVelocity <= EngineComponent.MaxTurnSpeed && InputValue.X != 0)
	{
		// Physics based rotation for this ship
		GetCapsuleComponent()->AddTorqueInDegrees(Torque, NAME_None, true);
	}*/

	// Acceleration
	if (EngineComponent.bCanAccelerate && Velocity <= EngineComponent.MaxAcceleration && InputValue.Y != 0)
	{
		const float BoostRange = EngineComponent.AccelerationHelperRange;
		float ForceMultiplier;

		// Check if the velocity is in the boost range
		if (Velocity < BoostRange && Velocity > -BoostRange)
		{
			ForceMultiplier = EngineComponent.AccelerationHelperMultiplier;
		}
		else
		{
			ForceMultiplier = EngineComponent.AccelerationMultiplier;
		}
		
		ForceMultiplier *= EngineComponent.Acceleration;
		const FVector ForceLocation = GetCapsuleComponent()->GetCenterOfMass();
		const FVector Force = ForceDirection * InputValue.Y * GetWorld()->DeltaTimeSeconds * 100 * ForceMultiplier;

		GetCapsuleComponent()->AddForceAtLocation(Force, ForceLocation);
	}
}

// Called every frame
void AShipBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddMovementInput(GetActorForwardVector(), InputVelocity);
}

// Camera look
void AShipBase::Look(const FInputActionValue& Value)
{
	float Multiplier = 0.f;
	switch (CameraState)
	{
	case ECameraState::Outside:
		Multiplier = CamSensitivityMultiplier_Outside;
		break;
	case ECameraState::Inside:
		CalculateControlRotation();
		Multiplier = CamSensitivityMultiplier_Inside;
		break;
	case ECameraState::Scoped:
		CalculateControlRotation();
		Multiplier = CamSensitivityMultiplier_Scoped;
		break;
	}
	
	const FRotator NewLookRotation = FRotator(Value.Get<FVector2d>().Y * -1.f * Multiplier, Value.Get<FVector2d>().X * Multiplier, 0.f);
	SpringArm->AddRelativeRotation(NewLookRotation);
}

// Aiming
void AShipBase::Server_ChangeAiming_Implementation(bool bNewAiming)
{
	bAiming = bNewAiming;

	// UE_LOG the bAiming
	UE_LOG(LogTemp, Warning, TEXT("bAiming: %s"), bAiming ? TEXT("true") : TEXT("false"));
}

// Shooting
void AShipBase::Shoot()
{
	Server_ShootProjectile();
}

void AShipBase::StopShooting()
{
	Server_StopShooting();
}

void AShipBase::Server_StopShooting_Implementation()
{
	//OceanityAbilityCompone
}

void AShipBase::Server_ShootProjectile_Implementation()
{
	if (OceanityAbilityComponent)
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FGameplayTag::RequestGameplayTag("Ability.Player.PrimaryFire"));
		OceanityAbilityComponent->TryActivateAbilitiesByTag(TagContainer);
	}
}

void AShipBase::NetMulti_SpawnParticleSystem_Implementation(UNiagaraSystem* ParticleSystem, FVector Location, FRotator Rotation)
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (CameraState == ECameraState::Outside)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlash, ProjectileSpawn, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
		}
	}
	else
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlash, ProjectileSpawn, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);	
	}
}

void AShipBase::Aim()
{
	// Camera change FOV to zoom in
	if (CameraState == ECameraState::Inside)
	{
		Camera->SetFieldOfView(50);
		CameraState = ECameraState::Scoped;
	}
}

void AShipBase::CancelAim()
{
	// Camera change FOV to zoom out
	if (CameraState == ECameraState::Scoped)
	{
		Camera->SetFieldOfView(90);
		CameraState = ECameraState::Inside;
	}
}

// Toggle View
void AShipBase::ToggleView()
{
	switch (CameraState)
	{
	case ECameraState::Outside:
		CameraState = ECameraState::Inside;
		SpringArm->TargetArmLength = 0.f;
		SpringArm->AddRelativeLocation(FVector(0.f, 0.f, -200.f));
		HandleToggleView(true);
		CalculateControlRotation();
		break;
	case ECameraState::Inside:
		CameraState = ECameraState::Outside;
		SpringArm->TargetArmLength = 1200.f;
		SpringArm->AddRelativeLocation(FVector(0.f, 0.f, 200.f));
		HandleToggleView(false);
		break;
	case ECameraState::Scoped:
		// Get out of scoped fov
		CancelAim();
		
		CameraState = ECameraState::Outside;
		SpringArm->TargetArmLength = 1200.f;
		SpringArm->AddRelativeLocation(FVector(0.f, 0.f, 200.f));
		HandleToggleView(false);
		break;
	}
}

void AShipBase::HandleToggleView(bool bInside)
{
	// Client hide turret for better aiming
	Client_HideTurret(bInside);
	
	// Call show scope ui interface
	if (const IControllerInterface* ControllerInterface = Cast<IControllerInterface>(GetController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleToggleView"));
		ControllerInterface->Execute_ShowScopeUI(GetController(), bInside);
	}

	// Saved last control rotation
	if (bInside)
	{
		if (GetLocalRole() != ROLE_SimulatedProxy)
		{
			SavedControlRotation_Outside = SpringArm->GetRelativeRotation();

			// Set turret rotation to current control rotation yaw but keep pitch
			const float TurretYawRotation = SavedControlRotation_Outside.Yaw;
			Turret->SetWorldRotation(FRotator(Turret->GetRelativeRotation().Pitch, TurretYawRotation, 0.f));
			TurretBase->SetWorldRotation(FRotator(0.f, TurretYawRotation, 0.f));
		}
	}
	else
	{
		if (GetLocalRole() != ROLE_SimulatedProxy)
		{
			SavedControlRotation_Scoped = SpringArm->GetRelativeRotation();

			// Set control rotation to current turret rotation but only use yaw
			FRotator ControlRotation = SpringArm->GetRelativeRotation();
			ControlRotation.Pitch = SavedControlRotation_Outside.Pitch;
			SpringArm->SetWorldRotation(ControlRotation);
		}
	}

	// Server
	Server_ChangeAiming(bInside);
}


void AShipBase::Client_HideTurret_Implementation(bool bHide)
{
	// Hide turret
	TurretBase->SetVisibility(!bHide);
	Turret->SetVisibility(!bHide);
}

/** Replicate Aiming */
void AShipBase::CalculateControlRotation()
{
	// Client prediction
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		const FRotator LookRotation = SpringArm->GetRelativeRotation();
		TurretBase->SetWorldRotation(FRotator(0.f, LookRotation.Yaw, 0.f));
		Turret->SetWorldRotation(FRotator(LookRotation.Pitch, LookRotation.Yaw, 0.f));
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("LookRotation: %s"), *LookRotation.ToString()));
	}

	// Replicate to all clients
	if (GetLocalRole() != ROLE_SimulatedProxy)
	{
		const FRotator LookRotation = SpringArm->GetRelativeRotation();
		Server_ChangeTurretRotation(LookRotation);
	}
}

void AShipBase::Server_ChangeTurretRotation_Implementation(FRotator ControlRotation)
{
	if (bAiming && ControlRotation != FRotator::ZeroRotator)
	{
		AimRotation = ControlRotation;
		NetMulti_ChangeTurretRotation(AimRotation);
	}
}

void AShipBase::NetMulti_ChangeTurretRotation_Implementation(FRotator ControlRotation)
{
	// Apply rotation
	TurretBase->SetWorldRotation(FRotator(0.f, ControlRotation.Yaw, 0.f));
	Turret->SetWorldRotation(FRotator(ControlRotation.Pitch, ControlRotation.Yaw, 0.f));
}

// Called to bind functionality to input
void AShipBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AShipBase::Turn);
		EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Triggered, this, &AShipBase::Accelerate);

		// Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AShipBase::Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &AShipBase::StopShooting);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShipBase::Look);

		// Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AShipBase::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AShipBase::CancelAim);

		// Toggle View
		EnhancedInputComponent->BindAction(ToggleViewAction, ETriggerEvent::Started, this, &AShipBase::ToggleView);
	}
}

// Replication
void AShipBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShipBase, CurrentVelocity);
	DOREPLIFETIME(AShipBase, InputVelocity);
	DOREPLIFETIME(AShipBase, bAiming);
	DOREPLIFETIME_CONDITION(AShipBase, AimRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShipBase, SavedControlRotation_Outside, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShipBase, SavedControlRotation_Scoped, COND_SkipOwner);
}

void AShipBase::SetShipProperty_Implementation(FShipProperty NewShipProperty)
{
	ShipProperty = NewShipProperty;
	NetMulti_UpdateShipMeshes(ShipProperty);
	
	OnShipPropertyUpdated(ShipProperty);

	//UE_LOG(LogAbilitySystemComponent, Error, TEXT("Health Attribute: %f"), AttributeSet->GetHealth());
}

