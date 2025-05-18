// Fill out your copyright notice in the Description page of Project Settings.


#include "BPC_Romba_CPP.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

void ABPC_Romba_CPP::OnUnPossess()
{
	Super::OnUnPossess();

	UKismetSystemLibrary::K2_PauseTimer(Owner, "Escape");

	UKismetSystemLibrary::K2_PauseTimer(Owner, "Pursuit");

	Destroy(false, true);

}

void ABPC_Romba_CPP::OnPossess(APawn* InPawn)
{

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
		//CollisionDetectionCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		CollisionDetectionCapsule->SetRelativeTransform(CapsuleTransform);
		CollisionDetectionCapsule->SetCapsuleSize(RadiusCasule, HalfHeightCasule);
		CollisionDetectionCapsule->SetHiddenInGame(false, false);  //Debug

		FComponentBeginOverlapSignature BeginOverlapSignature = CollisionDetectionCapsule->OnComponentBeginOverlap;
		FScriptDelegate OverlapDelegate;
		OverlapDelegate.BindUFunction(this, FName("OnCapsuleBeginOverlap"));
		CollisionDetectionCapsule->OnComponentBeginOverlap.Add(OverlapDelegate);

	}
}

void ABPC_Romba_CPP::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OVERLAP with: %s"), *OtherActor->GetName());
}