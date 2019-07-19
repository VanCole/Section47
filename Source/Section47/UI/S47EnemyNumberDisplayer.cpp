// Fill out your copyright notice in the Description page of Project Settings.

#include "S47EnemyNumberDisplayer.h"

#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AS47EnemyNumberDisplayer::AS47EnemyNumberDisplayer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AS47EnemyNumberDisplayer::BeginPlay()
{
	Super::BeginPlay();
	
	textOnDisplayer = "999";
}

// Called every frame
void AS47EnemyNumberDisplayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

