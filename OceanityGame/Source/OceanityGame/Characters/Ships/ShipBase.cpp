// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AShipBase::AShipBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1200.f ;
	SpringArm->bUsePawnControlRotation = true;
	
	OutsideCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Outside Camera"));
	OutsideCamera->SetupAttachment(SpringArm);
	OutsideCamera->bAutoActivate = true;
	
	InsideCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Inside Camera"));
	InsideCamera->SetupAttachment(SpringArm);
	InsideCamera->bAutoActivate = false;
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
}

/** Enhanced Movement */
// Movement
void AShipBase::Move(const FInputActionValue& Value)
{
	const FVector2d InputVector = Value.Get<FVector2d>();

	// Accelerate
	if (InputVector.Y != 0 && ShipInfo.MovementInfo.bCanAccelerate)
	{
		Accelerate(InputVector.Y);
	}

	// Turn
	if (InputVector.X != 0 && ShipInfo.MovementInfo.bCanTurn)
	{
		Turn(InputVector.X);
	}
}

void AShipBase::Accelerate(float Value)
{
	CalculateVelocity(Value);
}

void AShipBase::Turn(float Value)
{
	const float TurnValue = Value * ShipInfo.MovementInfo.TurnSpeed * GetWorld()->GetDeltaSeconds();
	AddActorLocalRotation(FRotator(0.f, TurnValue, 0.f));
}

void AShipBase::CalculateVelocity(float Value)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		Server_CalculateVelocity(Value);
	}
	
	// Calculate velocity
	const float TempVel = CurrentVelocity + (Value * ShipInfo.MovementInfo.Acceleration * GetWorld()->GetDeltaSeconds());

	// Clamp velocity
	CurrentVelocity = FMath::Clamp(TempVel, -ShipInfo.MovementInfo.MaxSpeed, ShipInfo.MovementInfo.MaxSpeed);

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
	const float NewMaxSpeed = FMath::Abs(CurrentVelocity < 0.f ? CurrentVelocity * BackwardsMultiplier : CurrentVelocity);
	GetCharacterMovement()->MaxWalkSpeed = NewMaxSpeed;

	// Debug print out CurrentVelocity and InputVelocity
	// UE_LOG(LogTemp, Warning, TEXT("CurrentVelocity: %f, InputVelocity: %f"), GetCharacterMovement()->GetMaxSpeed(), InputVelocity);
}

void AShipBase::Server_CalculateVelocity_Implementation(float Value)
{
	CalculateVelocity(Value);
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
		Multiplier = CamSensitivityMultiplier_Inside;
		break;
	case ECameraState::Scoped:
		Multiplier = CamSensitivityMultiplier_Scoped;
		break;
	}
	
	AddControllerPitchInput(Value.Get<FVector2d>().Y * ShipInfo.MovementInfo.TurnSpeed * GetWorld()->GetDeltaSeconds() * Multiplier);
	AddControllerYawInput(Value.Get<FVector2d>().X * ShipInfo.MovementInfo.TurnSpeed * GetWorld()->GetDeltaSeconds() * Multiplier);
}

void AShipBase::Shoot()
{
}

// Called every frame
void AShipBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShipInfo.MovementInfo.bCanAccelerate)
	{
		if (InputVelocity != 0)
		{
			AddMovementInput(GetActorForwardVector(), InputVelocity);
		}
	}
}

// Called to bind functionality to input
void AShipBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShipBase::Move);

		// Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AShipBase::Shoot);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShipBase::Look);
	}
}

void AShipBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShipBase, CurrentVelocity);
	DOREPLIFETIME(AShipBase, InputVelocity);
}

