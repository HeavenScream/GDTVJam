// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BPC_Romba_CPP.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOverlapDelegate);

UCLASS()
class GDTVJAM_API ABPC_Romba_CPP : public AAIController
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CapsuleCollisionMoth, meta = (AllowPrivateAccess = "true"))
	FVector RelativeCapsuleLocation = FVector(30.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CapsuleCollisionMoth, meta = (AllowPrivateAccess = "true"))
	float RadiusCasule = 30.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CapsuleCollisionMoth, meta = (AllowPrivateAccess = "true"))
	float HalfHeightCasule = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	float TimeBeforePursuit = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	float TimeBeforeExplosion = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	float PursuitChance = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TurnMoth, meta = (AllowPrivateAccess = "true"))
	float MinimumTurningDegree = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TurnMoth, meta = (AllowPrivateAccess = "true"))
	float MaximumTurningDegree = 270.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TurnMoth, meta = (AllowPrivateAccess = "true"))
	float TimeBeforeAnotherTurn = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	float DormantTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	float SpeedEscapeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	bool bEscape = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	bool bPursuit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	bool bDormant = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	bool bExplosion = true;

	UPROPERTY(BlueprintReadOnly, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	bool bIsEscape = false;

	UPROPERTY(BlueprintReadOnly, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	bool bIsPursuit = false;

	UPROPERTY(BlueprintReadOnly, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	bool bIsDormant = false;

	UPROPERTY(BlueprintReadOnly, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	bool bIsExplosion = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true"))
	bool bContinueMovingAfterDormant = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CapsuleCollision, meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* CapsuleCollision;

	UPROPERTY()
	bool bEscapeActiv;

	UPROPERTY()
	bool bPursuitActiv;

	UFUNCTION(BlueprintCallable, Category = "Moth")
	void ActivateEscape();

	UFUNCTION(BlueprintCallable, Category = "Moth")
	void ActivationPursuit();

	UFUNCTION(BlueprintCallable, Category = "Moth")
	void Pursuit();

	UFUNCTION(BlueprintCallable, Category = "Moth")
	void ActivationExplosion();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Moth")
	void Explosion();

	virtual void Explosion_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Moth")
	void Dormant();

	FTimerHandle EscapeTimer;
	FTimerHandle PursuitTimer;
	FTimerHandle AnotherTurnTimer;
	FTimerHandle ExplosionTimer;
	FTimerHandle DormantEndTimer;

protected:

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Escape();

	UFUNCTION()
	void AnotherTurn();

};
