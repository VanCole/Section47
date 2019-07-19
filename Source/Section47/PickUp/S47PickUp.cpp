// Fill out your copyright notice in the Description page of Project Settings.

#include "S47PickUp.h"
#include "S47Character.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "PickUp/S47Life_PickUp.h"
#include "PickUp/S47Armor_PickUp.h"
#include "Player/S47PlayerCharacter.h"
#include "UnrealNetwork.h"

// Sets default values
AS47PickUp::AS47PickUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	RootComponent = SceneComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	if (nullptr != MeshComponent && nullptr != SceneComponent)
		MeshComponent->SetupAttachment(SceneComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	if (nullptr != BoxComponent && nullptr != SceneComponent)
		BoxComponent->SetupAttachment(SceneComponent);

	RunningTime = 1.0f;
}

// Called when the game starts or when spawned
void AS47PickUp::BeginPlay()
{
	Super::BeginPlay();

	/*rotator = FRotator(0.0f, rotationSpeed, 0.0f);
	translator = FVector(0.0f, 0.0f, translationSpeed);*/

	OnActorBeginOverlap.AddUniqueDynamic(this, &AS47PickUp::OnPickUpBeginOverlap);
}

// Called every frame
void AS47PickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovePickUp(DeltaTime);
}

void AS47PickUp::OnPickUpBeginOverlap(AActor * _OverlappedActor, AActor * _OtherActor)
{
	if (nullptr != _OverlappedActor)
	{
		if (_OverlappedActor->IsA<AS47PickUp>())
		{
			AS47Character* MyCharacter = Cast<AS47Character>(_OtherActor);

			if (nullptr != MyCharacter)
			{
				if (MyCharacter->IsValidActorForCollision(_OtherActor))
				{
					if (nullptr != _OverlappedActor)
					{
						AS47PlayerCharacter* pc = Cast<AS47PlayerCharacter>(MyCharacter);

						if (nullptr != pc)
						{
							if (_OverlappedActor->IsA<AS47Life_PickUp>())
							{
								if (pc->GetHealth() < pc->GetMaxHealth())
								{
									DisappearancePickUp(pc);
								}
							}
							else if (_OverlappedActor->IsA<AS47Armor_PickUp>())
							{
								if (pc->GetCurrentArmor() < pc->GetMaxArmor())
								{
									DisappearancePickUp(pc);
								}
							}
							else
							{
								DisappearancePickUp(pc);
							}
						}
					}
				}
			}
		}
	}
}


void AS47PickUp::Effect(class AS47PlayerCharacter* target)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, TEXT("Effect pick up"));
}

void AS47PickUp::MovePickUp(float DeltaTime)
{
	FVector NewLocation = GetActorLocation();
	float DeltaHeight = FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime);
	NewLocation.Z += DeltaHeight * 10.0f;
	RunningTime += DeltaTime;
	SetActorLocation(NewLocation);

	FRotator NewRotation = GetActorRotation();
	float DeltaAngle = (DeltaTime * 90.0f);
	NewRotation.Yaw += DeltaAngle;
	SetActorRotation(NewRotation.Quaternion());
}

void AS47PickUp::DisappearancePickUp(AS47PlayerCharacter * pc)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, TEXT("Item picked up"));

	PickUpBP();
	if (pc->Role == ROLE_Authority)
	{
		Effect(pc);
		//DisappearancePickUp_ToClient(pc);
		//Destroy();
	}	
}

void AS47PickUp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS47PickUp, MeshComponent);

}
