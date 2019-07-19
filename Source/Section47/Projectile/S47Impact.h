// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S47Impact.generated.h"

UCLASS()
class SECTION47_API AS47Impact : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AS47Impact();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Decal, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* decalComponent;
};