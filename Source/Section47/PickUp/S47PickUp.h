// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S47PickUp.generated.h"

UCLASS()
class SECTION47_API AS47PickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AS47PickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UFUNCTION(Server, Reliable, WithValidation)
	//void DisappearancePickUp_ToServ(class AS47PlayerCharacter* pc);
	//UFUNCTION(NetMulticast, Reliable)
	//void DisappearancePickUp_ToClient(class AS47PlayerCharacter* pc);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnPickUpBeginOverlap(AActor* _OverlappedActor, AActor* _OtherActor);

	virtual void Effect(class AS47PlayerCharacter* target);

	UFUNCTION(BlueprintImplementableEvent)
	void PickUpBP();


	void MovePickUp(float DeltaTime);

	void DisappearancePickUp(class AS47PlayerCharacter* pc);

	/*UFUNCTION(Server, Reliable, WithValidation)
		void DisappearancePickUpRPC(class AS47PlayerCharacter* pc);*/

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	UPROPERTY(Category = S47PickUp, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = S47PickUp)
		class UBoxComponent* BoxComponent;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = S47PickUp)
		class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = S47PickUp)
		float value;

	float RunningTime;
};
