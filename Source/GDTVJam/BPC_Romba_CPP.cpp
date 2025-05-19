// Fill out your copyright notice in the Description page of Project Settings.


#include "BPC_Romba_CPP.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

void ABPC_Romba_CPP::OnUnPossess()
{
	Super::OnUnPossess();

	UKismetSystemLibrary::K2_ClearTimerHandle(this, EscapeTimer);

	UKismetSystemLibrary::K2_ClearTimerHandle(this, PursuitTimer);


	Destroy(false, true);

}

void ABPC_Romba_CPP::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UCapsuleComponent *CapsuleComponent = Cast<ACharacter>(InPawn)->GetCapsuleComponent();
    FRotator RelativeRot = CapsuleComponent->GetRelativeRotation();
	FRotator RelativeRotation = CapsuleComponent->GetRelativeRotation();
	FRotator RelativeRottator(RelativeRot.Roll, 90.0f, RelativeRot.Yaw);
	FVector RelativeScale(1.0f, 1.0f, 1.0f);
	FTransform CapsuleTransform(RelativeRotation, RelativeCapsuleLocation, RelativeScale);
	UCapsuleComponent *CollisionDetectionCapsule = NewObject<UCapsuleComponent>(InPawn, UCapsuleComponent::StaticClass(), TEXT("CapsuleCollision"));
	
	if (CollisionDetectionCapsule)
	{
		CollisionDetectionCapsule->RegisterComponent();
		CollisionDetectionCapsule->AttachToComponent(CapsuleComponent, FAttachmentTransformRules::KeepRelativeTransform);
		CollisionDetectionCapsule->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
		CollisionDetectionCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		CollisionDetectionCapsule->SetRelativeTransform(CapsuleTransform);
		CollisionDetectionCapsule->SetCapsuleSize(RadiusCasule, HalfHeightCasule);
		CollisionDetectionCapsule->SetHiddenInGame(false, false);  //Debug
		CapsuleCollision = CollisionDetectionCapsule;

		FComponentBeginOverlapSignature BeginOverlapSignature = CollisionDetectionCapsule->OnComponentBeginOverlap;
		FScriptDelegate OverlapDelegate;
		OverlapDelegate.BindUFunction(this, FName("OnCapsuleBeginOverlap"));
		CollisionDetectionCapsule->OnComponentBeginOverlap.Add(OverlapDelegate);
	}


}

void ABPC_Romba_CPP::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		//UE_LOG(LogTemp, Warning, TEXT("OVERLAP with: %s"), *OtherActor->GetName());
	}

	if (!bIsPursuit)
	{

		if (GetPawn())
		{
			FRotator NewRotation(0.f, GetPawn()->GetActorRotation().Yaw + FMath::FRandRange(MinimumTurningDegree, MaximumTurningDegree), 0.f);
			GetPawn()->SetActorRotation(NewRotation);
			TArray<AActor*> OverlappingActors;
			CapsuleCollision->GetOverlappingActors(OverlappingActors);

			if (!OverlappingActors.IsEmpty())
			{
				GetWorldTimerManager().SetTimer(AnotherTurnTimer, this, &ABPC_Romba_CPP::AnotherTurn, TimeBeforeAnotherTurn, false);
			}
		}
	}
}

void ABPC_Romba_CPP::AnotherTurn()
{
	if (!bIsPursuit)
	{
		FHitResult DummyHit;
		OnCapsuleBeginOverlap(nullptr, nullptr, nullptr, 0, false, DummyHit);
	}
}

void ABPC_Romba_CPP::ActivateEscape()
{
	bIsEscape = true;

	if (bEscape && bIsEscape)
	{
		GetWorldTimerManager().SetTimer(EscapeTimer, this, &ABPC_Romba_CPP::Escape, 0.02f, true);
	}
}

void ABPC_Romba_CPP::Escape()
{
	if (bEscape && bIsEscape)
	{
		GetPawn()->AddMovementInput(GetPawn()->GetActorForwardVector(), SpeedEscapeMultiplier);
	}
}

void ABPC_Romba_CPP::ActivationPursuit()
{
	if (bPursuit)
	{
		GetWorldTimerManager().SetTimer(PursuitTimer, this, &ABPC_Romba_CPP::Pursuit, TimeBeforePursuit, true);
	}
}
void ABPC_Romba_CPP::Pursuit()
{
	if (UKismetMathLibrary::RandomBoolWithWeight(PursuitChance / 100.f))
	{
		UKismetSystemLibrary::K2_ClearTimerHandle(this, EscapeTimer);

		bIsPursuit = true;

		if (bPursuit && bIsPursuit)
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(UGameplayStatics::GetPlayerPawn(this, 0));
			MoveRequest.SetAcceptanceRadius(5.f);

			FNavPathSharedPtr NavPath;
			MoveTo(MoveRequest, &NavPath);

			ActivationExplosion();
		}
	}
}

void ABPC_Romba_CPP::Dormant()
{
	if (bDormant)
	{
		if (!bIsDormant)
		{
			bEscapeActiv = bIsEscape;
			bPursuitActiv = bIsPursuit;

			bIsDormant = true;
			bIsEscape = false;
			bIsPursuit = false;
			StopMovement();

			UKismetSystemLibrary::K2_ClearTimerHandle(this, EscapeTimer);
			UKismetSystemLibrary::K2_ClearTimerHandle(this, PursuitTimer);

			GetWorldTimerManager().SetTimer(DormantEndTimer, this, &ABPC_Romba_CPP::Dormant, DormantTime, false);
		}
		else
		{
			bIsDormant = false;

			if (bContinueMovingAfterDormant)
			{
				if (bEscapeActiv)
				{
					bIsEscape = true;
					ActivateEscape();
				}

				if (bPursuitActiv)
				{
					bIsPursuit = true;
					Pursuit();
				}
				else
				{
					ActivationPursuit();
				}
			}
		}
	}
}

void ABPC_Romba_CPP::ActivationExplosion()
{
	if (bExplosion)
	{
		bIsExplosion = true;
		GetWorldTimerManager().SetTimer(ExplosionTimer, this, &ABPC_Romba_CPP::Explosion, TimeBeforeExplosion, false);
	}
}

void ABPC_Romba_CPP::Explosion_Implementation()
{
	if (bExplosion)
	{
		UE_LOG(LogTemp, Warning, TEXT("Boom"));
		GetPawn()->Destroy(false, true);
	}
}

void ABPC_Romba_CPP::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.IsSuccess())
	{
		Explosion();
	}
	else if (Result.IsFailure())
	{
		PursuitChance = 100.f;
		Pursuit();
	}

}