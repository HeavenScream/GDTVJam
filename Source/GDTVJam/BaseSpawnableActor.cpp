// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpawnableActor.h"

#include "Components/BoxComponent.h"

// Sets default values
ABaseSpawnableActor::ABaseSpawnableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = Mesh;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ABaseSpawnableActor::BeginPlay()
{
	Super::BeginPlay();
	AttemptToSpawn();
	
}

bool ABaseSpawnableActor::AttemptToSpawn()
{
	float Roll = FMath::RandRange(0.f, MaxRoll);
	if (Roll > chanceToSpawn)
	{
		FString SpawnLocation = GetActorLocation().ToCompactString();
		UE_LOG(LogTemp, Warning, TEXT("Spawned at: %s "), *SpawnLocation);
		//FActorSpawnParameters spawnParameters;
		//GetWorld()->SpawnActor(SpawnableClass, GetActorLocation(),GetActorRotation(), spawnParameters);
		return true;
	}
	return false;
}

