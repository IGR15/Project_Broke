// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstales/FallingPlatform.h"

#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"


AFallingPlatform::AFallingPlatform()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlatformMesh");
	PlatformMesh->SetupAttachment(Root);

	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlatformMesh->SetCollisionResponseToAllChannels(ECR_Block);

	TriggerCollision = CreateDefaultSubobject<UBoxComponent>("TriggerCollision");
	TriggerCollision->SetupAttachment(Root);

	TriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerCollision->SetGenerateOverlapEvents(true);
}

void AFallingPlatform::BeginPlay()
{
	Super::BeginPlay();

	TriggerCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&AFallingPlatform::OnOverlap);
}

void AFallingPlatform::OnOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bHasTriggered)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);

	if (!Character)
	{
		return;
	}

	bHasTriggered = true;

	GetWorldTimerManager().SetTimer(
		FallTimerHandle,
		this,
		&AFallingPlatform::StartFalling,
		FallDelay,
		false);

	GetWorldTimerManager().SetTimer(
		DestroyTimerHandle,
		this,
		&AFallingPlatform::DestroySelf,
		DestroyDelay,
		false);
}

void AFallingPlatform::StartFalling()
{
	PlatformMesh->SetSimulatePhysics(true);
}

void AFallingPlatform::DestroySelf()
{
	Destroy();
}
