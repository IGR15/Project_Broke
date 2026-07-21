// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MO_ProjectileBase.h"

#include "Character/MO_BaseCharacter.h"
#include "Components/LaunchComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AMO_ProjectileBase::AMO_ProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>("CollisionComp");
	SetRootComponent(CollisionComp);
	CollisionComp->SetSphereRadius(20.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	CollisionComp->SetGenerateOverlapEvents(true);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetupAttachment(CollisionComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultRocketMesh(
		TEXT("/Game/bazoka/SM_Rocket.SM_Rocket"));
	if (DefaultRocketMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(DefaultRocketMesh.Object);
	}

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	bReplicates = true;
	SetReplicatingMovement(true);
}

void AMO_ProjectileBase::PreInitializeComponents()
{
	// Must happen here, not BeginPlay: UProjectileMovementComponent reads
	// InitialSpeed to set Velocity in its own InitializeComponent(), which
	// runs during InitializeComponents() - well before BeginPlay. Setting it
	// in BeginPlay is too late and leaves Velocity at zero (missile spawns
	// and just sits there).
	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = Speed;
		ProjectileMovement->MaxSpeed = Speed;
	}

	Super::PreInitializeComponents();
}

void AMO_ProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AMO_ProjectileBase::OnSphereOverlap);

	if (AActor* Firer = GetInstigator())
	{
		CollisionComp->IgnoreActorWhenMoving(Firer, true);
	}

	if (HasAuthority())
	{
		SetLifeSpan(LifeSpanSeconds);
	}
}

void AMO_ProjectileBase::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bHit || OtherActor == this || OtherActor == GetInstigator())
	{
		return;
	}

	bHit = true;

	const FVector HitLocation = SweepResult.bBlockingHit ? FVector(SweepResult.ImpactPoint) : GetActorLocation();
	const FVector HitNormal = SweepResult.bBlockingHit ? FVector(SweepResult.ImpactNormal) : -GetActorForwardVector();

	OnImpact(OtherActor, HitLocation, HitNormal);
}

void AMO_ProjectileBase::OnImpact(AActor* HitActor, const FVector& HitLocation, const FVector& HitNormal)
{
	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), ImpactEffect, HitLocation, HitNormal.Rotation());
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitLocation);
	}

	if (!HasAuthority())
	{
		return;
	}

	if (AMO_BaseCharacter* HitCharacter = Cast<AMO_BaseCharacter>(HitActor))
	{
		if (ULaunchComponent* LaunchComp = HitCharacter->LaunchComponent)
		{
			const FVector KnockbackVelocity =
				GetActorForwardVector() * KnockbackForwardStrength + FVector::UpVector * KnockbackUpStrength;
			LaunchComp->Launch(KnockbackVelocity);
		}
	}

	// Don't Destroy() synchronously from inside an overlap callback: other
	// listeners on the same overlap event (e.g. a trigger volume's own
	// OnComponentBeginOverlap, like the Teleporter level Blueprint) may still
	// be mid-dispatch and would then read a pending-kill OtherActor ("X is not
	// valid (pending kill or garbage)" runtime error). Go inert immediately
	// and let a near-zero life span destroy it on the next tick instead.
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetVisibility(false);
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
	}
	SetLifeSpan(0.01f);
}
