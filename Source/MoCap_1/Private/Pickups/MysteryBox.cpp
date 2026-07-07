// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/MysteryBox.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "NiagaraFunctionLibrary.h"

AMysteryBox::AMysteryBox()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>("BoxMesh");
	BoxMesh->SetupAttachment(Root);
	BoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerCollision = CreateDefaultSubobject<UBoxComponent>("TriggerCollision");
	TriggerCollision->SetupAttachment(Root);
	TriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerCollision->SetGenerateOverlapEvents(true);

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>("RotatingMovement");
}

void AMysteryBox::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();

	RotatingMovement->RotationRate = FRotator(0.f, RotationSpeed, 0.f);

	TriggerCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&AMysteryBox::OnOverlap);
}

void AMysteryBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	const float ZOffset = FMath::Sin(RunningTime * FloatFrequency * 2.f * PI) * FloatAmplitude;
	SetActorLocation(InitialLocation + FVector(0.f, 0.f, ZOffset));
}

void AMysteryBox::OnOverlap(
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

	const int32 ItemNumber = FMath::RandRange(MinItemNumber, MaxItemNumber);

	UE_LOG(LogTemp, Warning, TEXT("MysteryBox: %s received item #%d"), *Character->GetName(), ItemNumber);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Yellow,
			FString::Printf(TEXT("Mystery Box: You got item #%d"), ItemNumber));
	}

	if (PopEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			PopEffect,
			GetActorLocation(),
			GetActorRotation());
	}

	Destroy();
}
