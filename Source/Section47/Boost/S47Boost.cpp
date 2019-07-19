// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Boost.h"
#include "S47BoostStruct.h"
#include "UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AS47Boost::AS47Boost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	RootComponent = SceneComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	if (nullptr != MeshComponent && nullptr != SceneComponent)
		MeshComponent->SetupAttachment(SceneComponent);

}

// Called when the game starts or when spawned
void AS47Boost::BeginPlay()
{
	Super::BeginPlay();

	weightBoost = 10.0f;

	if (nullptr != boostClass)
		boostStruct = NewObject<US47BoostStruct>(this, boostClass);
}

// Called every frame
void AS47Boost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 AS47Boost::GetWeight()
{
	return weightBoost;
}

void AS47Boost::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS47Boost, MeshComponent);
	DOREPLIFETIME(AS47Boost, boostClass);
	DOREPLIFETIME(AS47Boost, boostStruct);
}