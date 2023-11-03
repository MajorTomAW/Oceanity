// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OceanityGame/Characters/Ships/ShipBase.h"

AProjectileBase::AProjectileBase()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->SetIsReplicated(true);

	ProjectileHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	RootComponent = ProjectileHitbox;

	// Set collision object type to Projectile
	ProjectileHitbox->SetCollisionObjectType(ECC_GameTraceChannel2);
	ProjectileHitbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set collision response to Projectile
	FCollisionResponseContainer ResponseContainer;
	ResponseContainer.SetAllChannels(ECR_Ignore);
	ResponseContainer.SetResponse(ECC_GameTraceChannel1, ECR_Overlap); // ShipHull
	ResponseContainer.SetResponse(ECC_GameTraceChannel2, ECR_Overlap); // Projectile
	ProjectileHitbox->SetCollisionResponseToChannels(ResponseContainer);

	ProjectileParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Component"));
	ProjectileParticleSystem->SetupAttachment(RootComponent);
	
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	InitialLifeSpan = 10.f;
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// Bind delegate to ProjectileHitbox overlapping event
	ProjectileHitbox->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnProjectileHitboxOverlap);

	ProjectileMovement->InitialSpeed = ProjectileProperty.Speed;
	ProjectileMovement->MaxSpeed = ProjectileProperty.Speed;
}

void AProjectileBase::OnProjectileHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Return if not server
	if (!HasAuthority()) return;

	// Return if OtherActor is null or OtherActor is the owner
	if (!OtherActor || OtherActor == GetOwner()) return;

	// UE LOG the OtherActor and the Owner
	UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s, Owner: %s"), *OtherActor->GetName(), *GetOwner()->GetName());
	

	UE_LOG(LogTemp, Warning, TEXT("OnProjectileHitboxOverlap"));
	
	// Apply damage to OtherActor
	if (OtherActor->ActorHasTag("Ship"))
	{
		// Apply damage to OtherActor
		UGameplayStatics::ApplyDamage(OtherActor, ProjectileProperty.Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		
		// AddForceAtLocation to the CapsuleComponent of TargetActor in direction the projectile was coming from
		const AShipBase* TargetActor = Cast<AShipBase>(OtherActor);
		TargetActor->GetCapsuleComponent()->AddForceAtLocation(GetActorForwardVector() * ProjectileProperty.ImpactImpulse, SweepResult.ImpactPoint);
		
		NetMulti_SpawnParticleSystem(ProjectileProperty.ImpactEffect, SweepResult.ImpactPoint);
		Destroy();
	}
	else
	{
		NetMulti_SpawnParticleSystem(ProjectileProperty.ImpactEffect, SweepResult.ImpactPoint);
		Destroy();
	}
}

void AProjectileBase::NetMulti_SpawnParticleSystem_Implementation(UNiagaraSystem* ParticleSystem, FVector Location)
{
	// Spawn particle system
	 UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleSystem, Location);
}
