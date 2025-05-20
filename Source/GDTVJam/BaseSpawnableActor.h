// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseSpawnableActor.generated.h"

UCLASS()
class GDTVJAM_API ABaseSpawnableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseSpawnableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SpawnableActor")
	float chanceToSpawn = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "SpawnableActor")
	float MaxRoll = 100;
	
	UPROPERTY(EditDefaultsOnly, Category = "SpawnableActor")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnableActor")
	TArray <TSubclassOf<AActor>> SpawnableActorsArray;

	UFUNCTION(BlueprintCallable, Category="SpawnableActor")
	bool AttemptToSpawn();

	UFUNCTION(BlueprintCallable, Category="SpawnableActor")
	TSubclassOf<AActor> SelectActorToSpawn(TArray<TSubclassOf<AActor>> spawnableActors);
};
