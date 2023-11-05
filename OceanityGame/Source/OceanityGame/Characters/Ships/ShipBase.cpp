// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "OceanityGame/GameObjects/Projectiles/ProjectileBase.h"
#include "OceanityGame/Interfaces/ControllerInterface.h"

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
	ShipCabin->SetMassOverrideInKg(NAME_None, 0.f);
	ShipCabin->SetCollisionProfileName(TEXT("NoCollision"), false);
	ShipCabin->SetupAttachment(ShipBody, TEXT("CabinSocket"));

	// Turret Base
	TurretBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	TurretBase->SetMassOverrideInKg(NAME_None, 0.f);
	TurretBase->SetCollisionProfileName(TEXT("NoCollision"), false);
	TurretBase->SetupAttachment(ShipCabin, TEXT("TurretSocket"));

	// Turret
	Turret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurrentGun"));
	Turret->SetMassOverrideInKg(NAME_None, 0.f);
	Turret->SetCollisionProfileName(TEXT("NoCollision"), false);
	Turret->SetupAttachment(TurretBase, TEXT("TurretSocket"));

	// Engine
	Engine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Engine"));
	Engine->SetMassOverrideInKg(NAME_None, 0.f);
	Engine->SetCollisionProfileName(TEXT("NoCollision"), false);
	Engine->SetupAttachment(ShipBody);
	
	ProjectileSpawn = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawn"));
	ProjectileSpawn->SetupAttachment(Turret);

	/** Camera logic */
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(Turret);
	SpringArm->TargetArmLength = 1200.f ;
	SpringArm->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Outside Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bAutoActivate = true;

	// Add Ship tag
	Tags.Add("Ship");
}

// Called when the game starts or when spawned

void AShipBase::BeginPlay()
{
	Super::BeginPlay();
	
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
	
	// Create instance of turret execute property
	TurretExecuteProperty = NewObject<UExecuteProperty>(this, ShipProperty.TurretComponent.ExecuteProperty);
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
	ShipBody->SetMassOverrideInKg(NAME_None, NewShipProperty.HullComponent.Mass);
	ShipCabin->AttachToComponent(ShipBody, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("CabinSocket"));
	TurretBase->AttachToComponent(ShipCabin, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("TurretSocket"));
	Turret->SetStaticMesh(TurretMesh);
	Turret->AttachToComponent(TurretBase, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("TurretSocket"));
	Engine->SetStaticMesh(EngineMesh);
	Engine->AttachToComponent(ShipBody, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("EngineSocket"));
	SpringArm->AttachToComponent(Turret, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("CameraSocket"));
}

/** Enhanced Movement */
// Movement
void AShipBase::Move(const FInputActionValue& Value)
{
	Server_Move(Value.Get<FVector2d>());
}

void AShipBase::Server_Move_Implementation(FVector2D InputValue)
{
	const float Velocity = GetVelocity().Length();
	const float AngularVelocity = GetCapsuleComponent()->GetPhysicsAngularVelocityInDegrees().Length();
	const FEngineComponentProperty EngineComponent = ShipProperty.EngineComponent;
	
	const FVector ForceDirection = FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f);
	const FVector Torque = ForceDirection * -InputValue.X * GetWorld()->DeltaTimeSeconds * 100 * EngineComponent.TurnSpeedForce * EngineComponent.TurnSpeedMultiplier;

	// Turning
	if (EngineComponent.bCanTurn && AngularVelocity <= EngineComponent.MaxTurnSpeed && InputValue.X != 0)
	{
		// Physics based rotation for this ship
		GetCapsuleComponent()->AddTorqueInDegrees(Torque, NAME_None, true);
	}

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
		
		ForceMultiplier *= EngineComponent.AccelerationForce;
		const FVector ForceLocation = GetCapsuleComponent()->GetCenterOfMass();
		const FVector Force = ForceDirection * InputValue.Y * GetWorld()->DeltaTimeSeconds * 100 * ForceMultiplier;

		GetCapsuleComponent()->AddForceAtLocation(Force, ForceLocation);
	}
}

// Called every frame
void AShipBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	
	AddControllerPitchInput(Value.Get<FVector2d>().Y * Multiplier);
	AddControllerYawInput(Value.Get<FVector2d>().X * Multiplier);
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

void AShipBase::Server_ShootProjectile_Implementation()
{
	bool bSuccess = false;
	TurretExecuteProperty->ExecuteProperty(this, bSuccess);
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
	// Client
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
			SavedControlRotation_Outside = GetControlRotation();

			// Set turret rotation to current control rotation yaw but keep pitch
			const float TurretYawRotation = SavedControlRotation_Outside.Yaw;
			Turret->SetWorldRotation(FRotator(Turret->GetComponentRotation().Pitch, TurretYawRotation, 0.f));
			GetController()->SetControlRotation(FRotator(Turret->GetComponentRotation().Pitch, TurretYawRotation, 0.f));
		}
	}
	else
	{
		if (GetLocalRole() != ROLE_SimulatedProxy)
		{
			SavedControlRotation_Scoped = GetControlRotation();

			// Set control rotation to current turret rotation but only use yaw
			FRotator ControlRotation = SavedControlRotation_Outside;
			ControlRotation.Yaw = Turret->GetComponentRotation().Yaw;
			GetController()->SetControlRotation(ControlRotation);
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
		TurretBase->SetWorldRotation(FRotator(0.f, GetControlRotation().Yaw, 0.f));
		Turret->SetWorldRotation(FRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.f));
	}
	
	// Replicate the rotation to the server
	if (GetLocalRole() != ROLE_SimulatedProxy)
	{
		AimRotation = GetControlRotation();
		Server_ChangeTurretRotation(AimRotation);
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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShipBase::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AShipBase::Move);

		// Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AShipBase::Shoot);

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
	DOREPLIFETIME(AShipBase, ShipProperty);
	DOREPLIFETIME_CONDITION(AShipBase, AimRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShipBase, SavedControlRotation_Outside, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShipBase, SavedControlRotation_Scoped, COND_SkipOwner);
}

