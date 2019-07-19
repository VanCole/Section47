// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile/S47Impact.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"

// Sets default values
AS47Impact::AS47Impact()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	decalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	RootComponent = decalComponent;
}

// Called when the game starts or when spawned
void AS47Impact::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AS47Impact::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

