#include "Obstales/SwingingHammer.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ASwingingHammer::ASwingingHammer()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Pivot = CreateDefaultSubobject<USceneComponent>("Pivot");
	Pivot->SetupAttachment(Root);

	HammerMesh = CreateDefaultSubobject<UStaticMeshComponent>("HammerMesh");
	HammerMesh->SetupAttachment(Pivot);

	// IMPORTANT
	HammerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DamageCollision = CreateDefaultSubobject<UBoxComponent>("DamageCollision");
	DamageCollision->SetupAttachment(HammerMesh);

	DamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	DamageCollision->SetGenerateOverlapEvents(true);
}

void ASwingingHammer::BeginPlay()
{
	Super::BeginPlay();

	RunningTime = PhaseOffset;
	
	DamageCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&ASwingingHammer::OnOverlap);
}

void ASwingingHammer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	float Angle = FMath::Sin(RunningTime * SwingSpeed) * SwingAngle;

	Pivot->SetRelativeRotation(FRotator(0.f, 0.f, Angle));
}

void ASwingingHammer::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);

	if (!Character)
	{
		return;
	}

	// Direction from hammer to player
	FVector LaunchDirection =
		Character->GetActorLocation() - HammerMesh->GetComponentLocation();

	LaunchDirection.Z = 0.f;
	LaunchDirection.Normalize();

	FVector LaunchVelocity =
		LaunchDirection * HorizontalLaunchStrength +
		FVector::UpVector * VerticalLaunchStrength;

	Character->LaunchCharacter(LaunchVelocity, true, true);
}


