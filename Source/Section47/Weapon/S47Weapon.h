// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner/S47SpawnableInterface.h"
#include "Engine/Texture2D.h"
#include "S47Weapon.generated.h"

UCLASS()
class SECTION47_API AS47Weapon : public AActor, public IS47SpawnableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AS47Weapon();

	UFUNCTION()
		virtual void OnHitCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Attack();

	virtual void StopAttack();

	virtual void AttackBis();

	virtual void StopAttackBis();

	virtual void Reload();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitProjectile(FVector Location, FVector ForwardWeapon, FVector ForwardVelocity);

	UFUNCTION(Server, Reliable, WithValidation)
	void ConsumeAmmo();

	void OnAttack();
	void OnAttackEmpty();
	void OnAttackBis();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackCallBP();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackBP(const TArray<FVector>& PosHit);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackEmptyBP();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackBisBP(const TArray<FVector>& PosHit);

	// functions IS47SpawnableInterface
	int32 GetWeight();


protected:
	UFUNCTION(Server, Unreliable, WithValidation)
	void OnAttackBP_ToServ(const TArray<FVector>& PosHit);
	UFUNCTION(NetMulticast, Unreliable)
	void OnAttackBP_ToClient(const TArray<FVector>& PosHit);


	UFUNCTION(Server, Unreliable, WithValidation)
	void OnAttackEmptyBP_ToServ();
	UFUNCTION(NetMulticast, Unreliable)
	void OnAttackEmptyBP_ToClient();


	UFUNCTION(Server, Unreliable, WithValidation)
	void OnAttackBisBP_ToServ(const TArray<FVector>& PosHit);
	UFUNCTION(NetMulticast, Unreliable)
	void OnAttackBisBP_ToClient(const TArray<FVector>& PosHit);


	UFUNCTION(Server, Unreliable, WithValidation)
		void HitEnemy_ToServ(AS47AICharacter* Enemy, FVector _posHit, bool byLazer, FLinearColor ColorFX);
	UFUNCTION(NetMulticast, Unreliable)
		void HitEnemy_ToClient(AS47AICharacter* Enemy, FVector _posHit, bool byLazer, FLinearColor ColorFX);

	UFUNCTION(Server, Unreliable, WithValidation)
		void SaveLastShotOnEnemy_ToServ(AS47AICharacter* Enemy, FVector _force_Saved, FVector _poshit_Saved, FName _boneTouched_Saved);
	UFUNCTION(NetMulticast, Unreliable)
		void SaveLastShotOnEnemy_ToClient(AS47AICharacter* Enemy, FVector _force_Saved, FVector _poshit_Saved, FName _boneTouched_Saved);

	UPROPERTY(Category = S47Weapon, VisibleAnywhere, BlueprintReadWrite)
		TArray<FVector> PosHit;

public:

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	UTexture2D* crosshair;

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	float attackSpeed;

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	bool SemiAuto;

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	float maxDamage;
	UPROPERTY(Category = S47Weapon, EditAnywhere)
	float minDamage;

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	float maxRange;
	UPROPERTY(Category = S47Weapon, EditAnywhere)
	float minRange;

	UPROPERTY(Replicated,Category = S47Weapon, EditAnywhere)
	int TotalAmmo;

	UPROPERTY(Replicated,Category = S47Weapon, EditAnywhere, BlueprintReadOnly)
	int CurrentAmmo;

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	bool ConsumeAmmoDifferently;

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	bool IsNotThrowable;

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	int32 Weight;

	class AS47PlayerCharacter* OwnerCharacter;

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	int32 weightWeapon;

	UPROPERTY(Replicated, Category = OMFProjectile, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class US47ProjectileComponent* ProjectileComponent;

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	FText weaponName;

	UPROPERTY(Category = S47Weapon, EditAnywhere)
	UTexture2D* weaponImage;

protected:

	UPROPERTY(Category = S47Weapon, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;

	UPROPERTY(Replicated, BlueprintReadWrite, VisibleAnywhere, Category = "Weapon")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(Category = S47Weapon, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* FP_MuzzleLocation;
};
