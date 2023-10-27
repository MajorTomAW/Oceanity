// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "Kismet/GameplayStatics.h"

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
	
	SetReplicates(true);
	SetReplicateMovement(true);
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// Bind delegate to ProjectileHitbox overlapping event
	ProjectileHitbox->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnProjectileHitboxOverlap);
}

void AProjectileBase::OnProjectileHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Return if not server
	if (!HasAuthority()) return;

	// Return if OtherActor is null
	if (!OtherActor) return;

	// Apply damage to OtherActor
	if (OtherActor->ActorHasTag("Ship"))
	{
		// Apply damage to OtherActor
		UGameplayStatics::ApplyDamage(OtherActor, ProjectileInfo.ProjectileDamage, GetInstigatorController(), this, UDamageType::StaticClass());

		// Spawn impact particle system
		NetMulti_SpawnParticleSystem(ProjectileInfo.ImpactExplosion, SweepResult.ImpactPoint);

		// Destroy projectile
		Destroy();
	}
}

void AProjectileBase::NetMulti_SpawnParticleSystem_Implementation(UNiagaraSystem* ParticleSystem, FVector Location)
{
	// Spawn particle system
	// UGameplayStatics::SpawnParticleSystemAtLocation(GetWorld(), ParticleSystem, Location);
}
