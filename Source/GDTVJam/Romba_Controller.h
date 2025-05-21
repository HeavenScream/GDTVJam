// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Romba_Controller.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOverlapDelegate);

UCLASS()
class GDTVJAM_API ARomba_Controller : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ObstacleDetectionCapsule, meta = (AllowPrivateAccess = "true", ToolTip = "Relative Obstacle Detection Capsule Location"))
	FVector RelativeObstacleDetectionCapsuleLocation = FVector(30.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ObstacleDetectionCapsule, meta = (AllowPrivateAccess = "true", ToolTip = "Obstacle Detection Capsule Radius"))
	float ObstacleDetectionCapsuleRadius = 30.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ObstacleDetectionCapsule, meta = (AllowPrivateAccess = "true", ToolTip = "Obstacle Detection Capsule HalfHeight"))
	float ObstacleDetectionCapsuleHalfHeight = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true", ToolTip = "The frequency (in seconds) at which 'Constant Attempt Pursuit' will summon 'Attempted Pursuit'."))
	float AttemptedPursuitFrequency = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "100.0", ToolTip = "Chance the bot will initiate pursuit when 'Attempted Pursuit' is called."))
	float PursuitChance = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TurnMoth, meta = (AllowPrivateAccess = "true", ToolTip = "The minimum degree the bot will turn when colliding with an obstacle."))
	float MinimumTurningDegree = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TurnMoth, meta = (AllowPrivateAccess = "true", ToolTip = "The maximum degree the bot will turn when colliding with an obstacle."))
	float MaximumTurningDegree = 270.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TurnMoth, meta = (AllowPrivateAccess = "true", ToolTip = "The rate (in seconds) at which new turns occur if, when turning from an obstacle, the 'Obstacle Detection Capsule' still collides with the obstacle."))
	float NewTurnRate = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true", ToolTip = "Dormant duration."))
	float DormantDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", ToolTip = ""))
	float SpeedEscapeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StateMoth, meta = (AllowPrivateAccess = "true", ToolTip = "Is it possible to activate the escape state. If false the function 'StartEscape' will not work."))
	bool bEscapePossible = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StateMoth, meta = (AllowPrivateAccess = "true", ToolTip = "Is it possible to activate the pursuit state. If false the functions 'ConstantAttemptPursuit' and 'AttemptPursuit' will not work."))
	bool bPursuitPossible = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StateMoth, meta = (AllowPrivateAccess = "true", ToolTip = "Is it possible to activate the dormant state. If false the function 'StartDormant' will not work."))
	bool bDormantPossible = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StateMoth, meta = (AllowPrivateAccess = "true", ToolTip = "Is it possible to activate the explosion state. If false the functions 'ActivateTimerBeforeExplosion' and 'Explosion' will not work."))
	bool bExplosionPossible = true;

	UPROPERTY(BlueprintReadOnly, Category = StateMoth, meta = (AllowPrivateAccess = "true", ToolTip = "Bot in escape state?"))
	bool bIsEscape = false;

	UPROPERTY(BlueprintReadOnly, Category = StateMoth, meta = (AllowPrivateAccess = "true", ToolTip = "Bot in pursuit state?"))
	bool bIsPursuit = false;

	UPROPERTY(BlueprintReadOnly, Category = StateMoth, meta = (AllowPrivateAccess = "true", ToolTip = "Bot in dormant state?"))
	bool bIsDormant = false;

	UPROPERTY(BlueprintReadOnly, Category = StateMoth, meta = (AllowPrivateAccess = "true", ToolTip = "Bot in explosion state?"))
	bool bIsExplosion = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveMoth, meta = (AllowPrivateAccess = "true", ToolTip = "If true, the bot will continue with Escape or Pursuit after Dormant."))
	bool bContinueMovingAfterDormant = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ExplosionMoth, meta = (AllowPrivateAccess = "true", ToolTip = "The time (in seconds) after which the 'ActivateTimer Before Explosion' function will call 'Explosion'."))
	float DelayBeforeExplosion = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ExplosionMoth, meta = (AllowPrivateAccess = "true", ToolTip = "If true, when the pursuit begins the 'Activate Timer Before Explosion' function will be called."))
	bool ActivateTimerBeforeExplosionDuringPursuit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ExplosionMoth, meta = (AllowPrivateAccess = "true", ToolTip = "If true, when the bot chases the player and catches up with him, the 'Explosion' function will be called."))
	bool ExplodeIfCaught = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ObstacleDetectionCapsule, meta = (AllowPrivateAccess = "true", ToolTip = "Collision capsule created when moving into a bot. The overlay of this capsule is the trigger for turning."))
	UPrimitiveComponent* ObstacleDetectionCapsule;

	UPROPERTY()
	bool bStartEscapeActiv;

	UPROPERTY()
	bool bConstantAttemptPursuitActiv;

	UPROPERTY()
	bool bAttemptedPursuitActiv;

	UFUNCTION(BlueprintCallable, Category = "Moth", meta = (ToolTip = "The bot starts moving straight, turning when it hits an obstacle. Related Variables: Relative Capsule Location, Radius Casule, Half Height Casule, Speed Escape Multiplier, Minimum Turning Degree, Maximum Turning Degree, New Turn Rate, Escape Possible, Is Escape."))
	void StartEscape();

	UFUNCTION(BlueprintCallable, Category = "Moth", meta = (ToolTip = "Calls the 'Attempt Pursuit' function at regular intervals. Related Variables: Attempted Pursuit Frequency, Pursuit Possible, Is Pursuit."))
	void ConstantAttemptPursuit();

	UFUNCTION(BlueprintCallable, Category = "Moth", meta = (ToolTip = "Has a certain chance of triggering a pursuit. When pursuing, the bot will move towards the player. Related Variables: Pursuit Chance, Pursuit Possible, Is Pursuit."))
	void AttemptedPursuit();

	UFUNCTION(BlueprintCallable, Category = "Moth", meta = (ToolTip = "When called, stops the bot for a certain amount of time. Until the dormant time expires, Escape or Pursuit will not be performed. Related Variables: Dormant Duration, Continue Moving After Dormant, Dormant Possible, Is Dormant."))
	void StartDormant();

	UFUNCTION(BlueprintCallable, Category = "Moth", meta = (ToolTip = "When called, activates a timer after which the 'Explosion' function will be called. Related Variables: Time Before Explosion, Activate Timer Before Explosion During Pursuit, Explode If Caught, Explosion Possible, Is Explosion."))
	void ActivateTimerBeforeExplosion();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Moth", meta = (ToolTip = "When called, by default, logs 'Boom' and destroys the bot. You can override this functionality with 'Event Explosion'.Related Variables: Explosion Possible, Is Explosion."))
	void Explosion();

	virtual void Explosion_Implementation();

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
