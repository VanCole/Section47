// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Spawner/S47SpawnerWeapon.h"
#include "S47Types.h"
#include "S47AttackComponent.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackComponentEvent);
UCLASS()
class SECTION47_API US47AttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	US47AttackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Attack();
	void StopAttack();

	void AttackBis();
	void StopAttackBis();
	void Kick();

	void ChangeWeapon(float Value);

	void ChooseWeapon(ES47WeaponIndex WeaponIndex);
	template<ES47WeaponIndex WeaponIndex>
	void ChooseWeapon() { ChooseWeapon(WeaponIndex); }

	UFUNCTION(BlueprintCallable)
	class AS47Weapon* GetCurrentWeapon() { return CurrentWeapon; }

	void ChangeCurrentWeapon();

	void ThrowCurrentWeapon(bool isGrenadeLauncherExploding = false);

	void SpawnWeapon(int32 index, TSubclassOf<class AS47Weapon> weapon);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerThrowCurrentWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnWeapon(int32 index, TSubclassOf<class AS47Weapon> weapon);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerChangeCurrentWeapon(int32 _index);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAttackBisRPC();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStopAttackBisRPC();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAttackRPC();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStopAttackRPC();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
		void OnSpawnerBeginOverlap(AActor* _OverlappedActor, AActor* _OtherActor);

	void WeaponGetOutOfInventory();

protected:

	

	UPROPERTY(Replicated, VisibleAnywhere, Category = S47Attack)
	class AS47Weapon* CurrentWeapon;

	TEnumAsByte<enum ES47Weapon> CurrentWeaponType;

	UPROPERTY(Replicated)
	bool isAttacking;

	UPROPERTY(Replicated)
	bool isAttackingBis;

	float timeSinceLastAttack;

	UPROPERTY(EditAnywhere, Category = S47Attack)
	TSubclassOf<class AS47Weapon> StartWeapon;

public:

	UFUNCTION(NetMulticast, Reliable)
	void OnWeaponTaken();

	UPROPERTY(BlueprintAssignable)
	FAttackComponentEvent OnWeaponTaken_BP;

	UFUNCTION(NetMulticast, Reliable)
	void OnWeaponSwitched();

	UPROPERTY(BlueprintAssignable)
	FAttackComponentEvent OnWeaponSwitched_BP;

	UPROPERTY(Replicated)
	class AS47PlayerCharacter* OwnerCharacter;

	UPROPERTY(Replicated,VisibleAnywhere, Category = S47Attack)
		TArray<class AS47Weapon*> WeaponOnPlayer;

	UPROPERTY(Replicated)
	int32 indexWeapon;

};