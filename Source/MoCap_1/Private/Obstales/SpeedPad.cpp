// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstales/SpeedPad.h"
#include "Components/BoxComponent.h"
#include "Components/SpeedBoostComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


ASpeedPad::ASpeedPad()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	PadMesh = CreateDefaultSubobject<UStaticMeshComponent>("PadMesh");
	PadMesh->SetupAttachment(Root);

	PadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerCollision = CreateDefaultSubobject<UBoxComponent>("TriggerCollision");
	TriggerCollision->SetupAttachment(Root);

	TriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerCollision->SetGenerateOverlapEvents(true);
}

void ASpeedPad::BeginPlay()
{
	Super::BeginPlay();

	TriggerCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&ASpeedPad::OnOverlap);
}

void ASpeedPad::OnOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);

	if (!Character)
	{
		return;
	}


	USpeedBoostComponent* SpeedBoostComp =
		Character->FindComponentByClass<USpeedBoostComponent>();

	if (!SpeedBoostComp)
	{
		return;
	}

	SpeedBoostComp->ApplySpeedBoost(SpeedMultiplier, BoostDuration);
}
