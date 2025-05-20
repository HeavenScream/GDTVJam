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

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		return true;
	}
	return false;
}

TSubclassOf<AActor> ABaseSpawnableActor::SelectActorToSpawn(TArray<TSubclassOf<AActor>> spawnableActors)
{
	int ArrayLength = spawnableActors.Num()-1;
 
 	int selectedItem = FMath::RandRange (0, ArrayLength);
 
 	TSubclassOf<AActor> ActorToSpawn = spawnableActors[selectedItem];
 	return ActorToSpawn;
}
