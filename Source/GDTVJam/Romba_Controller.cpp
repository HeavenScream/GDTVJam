// Fill out your copyright notice in the Description page of Project Settings.


#include "Romba_Controller.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

void ARomba_Controller::OnUnPossess()
{
	Super::OnUnPossess();

	UKismetSystemLibrary::K2_ClearTimerHandle(this, EscapeTimer);

	UKismetSystemLibrary::K2_ClearTimerHandle(this, PursuitTimer);


	Destroy(false, true);

}

void ARomba_Controller::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UCharacterMovementComponent * CharacterMovementComponent = Cast<ACharacter>(InPawn)->GetCharacterMovement();
	CharacterMovementComponent->MaxWalkSpeed = MaxWalkSpeed;

	UCapsuleComponent *CapsuleComponent = Cast<ACharacter>(InPawn)->GetCapsuleComponent();
    FRotator RelativeRot = CapsuleComponent->GetRelativeRotation();
	FRotator RelativeRotation = CapsuleComponent->GetRelativeRotation();
	FRotator RelativeRottator(RelativeRot.Roll, 90.0f, RelativeRot.Yaw);
	FVector RelativeScale(1.0f, 1.0f, 1.0f);
	FTransform CapsuleTransform(RelativeRotation, RelativeObstacleDetectionCapsuleLocation, RelativeScale);
	UCapsuleComponent *CollisionDetectionCapsule = NewObject<UCapsuleComponent>(InPawn, UCapsuleComponent::StaticClass(), TEXT("CapsuleCollision"));
	
	if (CollisionDetectionCapsule)
	{
		TArray<TEnumAsByte<ECollisionChannel>> CollisionChannel;
		TArray<TEnumAsByte<ECollisionResponse>> CollisionResponse;
		int32 ArrayIndex = 0;

		CollisionEssentialsObstacleDetectionCapsule.GenerateKeyArray(CollisionChannel);
		CollisionEssentialsObstacleDetectionCapsule.GenerateValueArray(CollisionResponse);

		CollisionDetectionCapsule->RegisterComponent();
		CollisionDetectionCapsule->AttachToComponent(CapsuleComponent, FAttachmentTransformRules::KeepRelativeTransform);
		CollisionDetectionCapsule->SetCollisionEnabled(CollisionEnabledObstacleDetectionCapsule);
		CollisionDetectionCapsule->SetCollisionObjectType(CollisionObjectTypeObstacleDetectionCapsule);

		for (TEnumAsByte<ECollisionChannel> ArrayElement : CollisionChannel)
		{
			CollisionDetectionCapsule->SetCollisionResponseToChannel(ArrayElement, CollisionResponse[ArrayIndex]);
			ArrayIndex++;
		}
		
		CollisionDetectionCapsule->SetRelativeTransform(CapsuleTransform);
		CollisionDetectionCapsule->SetCapsuleSize(ObstacleDetectionCapsuleRadius, ObstacleDetectionCapsuleHalfHeight);
		CollisionDetectionCapsule->SetHiddenInGame(!DisplayOfObstacleDetectionCapsule, false);  //Debug
		ObstacleDetectionCapsule = CollisionDetectionCapsule;

		FComponentBeginOverlapSignature BeginOverlapSignature = CollisionDetectionCapsule->OnComponentBeginOverlap;
		FScriptDelegate OverlapDelegate;
		OverlapDelegate.BindUFunction(this, FName("OnCapsuleBeginOverlap"));
		CollisionDetectionCapsule->OnComponentBeginOverlap.Add(OverlapDelegate);
	}

	if (!GetPawn()->Tags.Contains("Moth"))
	{
		GetPawn()->Tags.Add("Moth");
	}
}

void ARomba_Controller::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ensure(ObstacleDetectionCapsule);

  /*if (OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("OVERLAP with: %s"), *OtherActor->GetName());
	}*/

	if (!bIsPursuit)
	{
		if (GetPawn())
		{
			NewRotation = { 0.f, GetPawn()->GetActorRotation().Yaw + FMath::FRandRange(MinimumTurningDegree, MaximumTurningDegree), 0.f };

			if (TurnSmoothly)
			{
				GetWorldTimerManager().SetTimer(SmoothTurnTimer, this, &ARomba_Controller::SmoothTurn, 0.01f, true);
			}
			else
			{
				TArray<AActor*> OverlappingActors;

				GetPawn()->SetActorRotation(NewRotation);

				ObstacleDetectionCapsule->GetOverlappingActors(OverlappingActors);

				if (!OverlappingActors.IsEmpty())
				{
					GetWorldTimerManager().SetTimer(AnotherTurnTimer, this, &ARomba_Controller::AnotherTurn, NewTurnRate, false);
				}
			}
		}
	}
}

void ARomba_Controller::AnotherTurn()
{
	if (!bIsPursuit)
	{
		FHitResult DummyHit;
		OnCapsuleBeginOverlap(nullptr, nullptr, nullptr, 0, false, DummyHit);
	}
}

void ARomba_Controller::SmoothTurn()
{
	if (UKismetMathLibrary::NearlyEqual_FloatFloat(UKismetMathLibrary::ClampAxis(NewRotation.Yaw), UKismetMathLibrary::ClampAxis(GetPawn()->GetActorRotation().Yaw), 0.01f))
	{
		TArray<AActor*> OverlappingActors;
		ObstacleDetectionCapsule->GetOverlappingActors(OverlappingActors);

		if (!OverlappingActors.IsEmpty())
		{
			NewRotation = { 0.f, GetPawn()->GetActorRotation().Yaw + FMath::FRandRange(MinimumTurningDegree, MaximumTurningDegree), 0.f };
		}
		else
		{
			UKismetSystemLibrary::K2_ClearTimerHandle(this, SmoothTurnTimer);
		}
	}
	else
	{
		GetPawn()->SetActorRotation(FMath::RInterpTo(GetPawn()->GetActorRotation(), NewRotation, UGameplayStatics::GetWorldDeltaSeconds(this), SmoothTurningSpeed));
	}
}

void ARomba_Controller::StartEscape()
{
	if (bEscapePossible)
	{
		if (!bIsDormant)
		{
			if (!GetPawn()->Tags.Contains("Escape"))
			{
				GetPawn()->Tags.Add("Escape");
				GetPawn()->Tags.Remove("Pursuit");
			}

			bIsEscape = true;
			bIsPursuit = false;

			StopMovement();
			UKismetSystemLibrary::K2_ClearTimerHandle(this, PursuitTimer);

			GetWorldTimerManager().SetTimer(EscapeTimer, this, &ARomba_Controller::Escape, 0.01f, true);
		}
		else
		{
			bStartEscapeActiv = true;
			bAttemptedPursuitActiv = false;
		}
	}
}

void ARomba_Controller::Escape()
{
	if (bEscapePossible && !bIsDormant)
	{
		GetPawn()->AddMovementInput(GetPawn()->GetActorForwardVector(), SpeedEscapeMultiplier);
	}
}

void ARomba_Controller::ConstantAttemptPursuit()
{
	if (bPursuitPossible)
	{
		bConstantAttemptPursuitActiv = true;

		if (!bIsDormant)
		{
			GetWorldTimerManager().SetTimer(PursuitTimer, this, &ARomba_Controller::AttemptedPursuit, AttemptedPursuitFrequency, true);
		}
	}
}
void ARomba_Controller::AttemptedPursuit()
{
	if (bPursuitPossible)
	{
		if (!bIsDormant)
		{
			if (UKismetMathLibrary::RandomBoolWithWeight(PursuitChance / 100.f) || bStartPursuit)
			{
				if (!GetPawn()->Tags.Contains("Pursuit"))
				{
					GetPawn()->Tags.Add("Pursuit");
					GetPawn()->Tags.Remove("Escape");
				}

				if (ActivateTimerBeforeExplosionDuringPursuit)
				{
					ActivateTimerBeforeExplosion();
					ActivateTimerBeforeExplosionDuringPursuit = false;
				}


				bIsPursuit = true;
				bIsEscape = false;
				bStartPursuit = false;

				UKismetSystemLibrary::K2_ClearTimerHandle(this, EscapeTimer);

				FAIMoveRequest MoveRequest;
				FNavPathSharedPtr NavPath;
				FVector ProjectedLocation = GetPawn()->GetActorLocation();
				ANavigationData* UseNavData = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->GetDefaultNavDataInstance();
				TSubclassOf<UNavigationQueryFilter> NavigationQueryFilter;

				if (UGameplayStatics::GetPlayerPawn(this, 0))
				{
					if (UNavigationSystemV1::K2_ProjectPointToNavigation(this, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(), ProjectedLocation, UseNavData, NavigationQueryFilter))
					{
						UKismetSystemLibrary::K2_ClearTimerHandle(this, PursuitTimer);
						MoveRequest.SetGoalActor(UGameplayStatics::GetPlayerPawn(this, 0));
						MoveRequest.SetAcceptanceRadius(PursuitEndRadius);
						MoveTo(MoveRequest, &NavPath);
					}
				}
			}
		}
		else
		{
			bAttemptedPursuitActiv = true;
			bStartEscapeActiv = false;
		}
	}
}

void ARomba_Controller::StartDormant()
{
	if (bDormantPossible)
	{		
		if (!bIsDormant)
		{
			if (!GetPawn()->Tags.Contains("Dormant"))
			{
				GetPawn()->Tags.Add("Dormant");
			}

			bStartEscapeActiv = bIsEscape;
			bAttemptedPursuitActiv = bIsPursuit;

			bIsDormant = true;

			StopMovement();

			UKismetSystemLibrary::K2_ClearTimerHandle(this, EscapeTimer);
			UKismetSystemLibrary::K2_ClearTimerHandle(this, PursuitTimer);

			GetWorldTimerManager().SetTimer(DormantEndTimer, this, &ARomba_Controller::StartDormant, DormantDuration, false);
		}
		else
		{
			if (GetPawn()->Tags.Contains("Dormant"))
			{
				GetPawn()->Tags.Remove("Dormant");
			}

			bIsDormant = false;

			if (bContinueMovingAfterDormant)
			{

				if (bStartEscapeActiv)
				{
					StartEscape();
				}

				if (bConstantAttemptPursuitActiv)
				{
					ConstantAttemptPursuit();

				}
				else if(bAttemptedPursuitActiv)
				{
					AttemptedPursuit();
				}

				bStartEscapeActiv = false;
				bConstantAttemptPursuitActiv = false;
				bAttemptedPursuitActiv = false;
			}
		}
	}
}

void ARomba_Controller::ActivateTimerBeforeExplosion()
{
	if (bExplosionPossible)
	{
		if (!GetPawn()->Tags.Contains("Explosion"))
		{

			GetPawn()->Tags.Add("Explosion");
		}

		bIsExplosion = true;
		GetWorldTimerManager().SetTimer(ExplosionTimer, this, &ARomba_Controller::Explosion, DelayBeforeExplosion, false);
	}
}

void ARomba_Controller::Explosion_Implementation()
{
	if (bExplosionPossible)
	{
		UE_LOG(LogTemp, Warning, TEXT("Boom"));
		UKismetSystemLibrary::K2_ClearTimerHandle(this, ExplosionTimer);
		GetPawn()->Destroy(false, true);
	}
}

void ARomba_Controller::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.IsSuccess())
	{
		if (ExplodeIfApproached)
		{
			Explosion();
		}
	}
	else if (Result.IsFailure())
	{
		bStartPursuit = true;
		GetWorldTimerManager().SetTimer(PursuitTimer, this, &ARomba_Controller::AttemptedPursuit, AttemptedPursuitFrequency, true);
	}
}