// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstales/BouncePad.h"
#include "Components/BoxComponent.h"
#include "Components/LaunchComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


ABouncePad::ABouncePad()
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

void ABouncePad::BeginPlay()
{
	Super::BeginPlay();

	TriggerCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&ABouncePad::OnOverlap);
}

void ABouncePad::OnOverlap(
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
	UE_LOG(LogTemp, Warning, TEXT("Bounce!"));

	FVector LaunchVelocity =
		GetActorForwardVector() * ForwardLaunchStrength +
		FVector::UpVector * VerticalLaunchStrength;

	/*Character->LaunchCharacter(
		LaunchVelocity,
		false,   // Override horizontal velocity
		true    // Override vertical velocity
	);*/
	/*FVector Impulse =
	GetActorForwardVector() * ForwardLaunchStrength  +
	FVector::UpVector * VerticalLaunchStrength;

	Character->GetCharacterMovement()->AddImpulse(
		Impulse,
		true
	);*/
	ULaunchComponent* LaunchComp =
	Character->FindComponentByClass<ULaunchComponent>();

	if (LaunchComp)
	{
		LaunchComp->Launch(LaunchVelocity);
	}
}