// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "S47Projectile.generated.h"

UCLASS()
class SECTION47_API AS47Projectile : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AS47Projectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(Category = OMFProjectile, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* AOETrigger;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Explode(float forceExplosion);
	virtual void ApplyExplosionForce();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerExplodeRPC(float _forceExplosion);
	UFUNCTION(NetMulticast, Unreliable)
	void AddImpulse_ToClient(UPrimitiveComponent* _Enemy, FVector _Impulse, FVector _Location);

	float Damage;

	//Methods
public:

	virtual void InitProjectile(FVector Location, FVector ForwardWeapon, FGenericTeamId TeamId);

	/*UFUNCTION()
		virtual void OnProjectileHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);*/

	/** called when projectile hits something */
	UFUNCTION()
		virtual void OnHitCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	UFUNCTION()
		virtual void OnLifeEnded();

	UFUNCTION(BlueprintImplementableEvent)
		void OnLifeEndedBP();

	virtual bool IsValidActorForCollision(AActor* _OtherActor);

	virtual FGenericTeamId GetGenericTeamId() const override { return S47TeamID; }
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	//Attributes
public:

	UPROPERTY(Replicated,Category = OMFProjectile, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* MeshComponent;

	UPROPERTY(Replicated,Category = OMFProjectile, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class US47ProjectileComponent* ProjectileComponent;

	//FProjectileLifeEnded EndLifeDel;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Attributes
protected:

	UPROPERTY(Category = S47Projectile, EditAnywhere)
		float S47LifeSpan;

	bool dealedDMG = false;

	FTimerHandle ProjectileTimerHandle;

	UPROPERTY(Category = S47Projectile, EditAnywhere)
	FGenericTeamId S47TeamID;

	UPROPERTY(Category = BOOM, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class URadialForceComponent* ExplosionForceComponent;
};
