// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/S47AttackComponent.h"
#include "S47MovementComponent.h"
#include "Components/S47CameraComponent.h"
#include "Weapon/S47Weapon.h"
#include "Weapon/WeaponRaycast/S47WeaponRaycast.h"
#include "Weapon/WeaponRaycast/S47Lasergun.h"
#include "Weapon/WeaponProjectile/S47GrenadeLauncher.h"
#include "Player/S47PlayerCharacter.h"
#include "S47Character.h"
#include "Weapon/WeaponRaycast/S47Rifle.h"
#include "Weapon/WeaponRaycast/S47Pistol.h"
#include "AI/S47AICharacter.h"
#include "Spawner/S47Spawner.h"
#include "Spawner/S47SpawnerEnemy.h"
#include "UnrealNetwork.h"
#include "Perception/AISense_Hearing.h"
#include "Engine.h"

// Sets default values
US47AttackComponent::US47AttackComponent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;

	indexWeapon = 0;
	timeSinceLastAttack = 0.f;
	WeaponOnPlayer.SetNum(3);
}

void US47AttackComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		OwnerCharacter = Cast<AS47PlayerCharacter>(GetOwner());
		SpawnWeapon(0, StartWeapon);
		ChangeCurrentWeapon();
		OwnerCharacter->OnActorBeginOverlap.AddUniqueDynamic(this, &US47AttackComponent::OnSpawnerBeginOverlap);
		//CurrentWeapon->SetOwner(OwnerCharacter);
	}
}

void US47AttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (nullptr != OwnerCharacter && CurrentWeapon != nullptr && nullptr != OwnerCharacter->GetController())
	{
		CurrentWeapon->SetActorRotation(OwnerCharacter->GetControlRotation());
	}

	if (!OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	if (timeSinceLastAttack > 0.f)
	{
		timeSinceLastAttack -= DeltaTime;
	}

	else if (isAttacking && nullptr != CurrentWeapon)
	{
		if (CurrentWeapon->CurrentAmmo > 0 || CurrentWeapon->IsA<AS47Pistol>())
			OwnerCharacter->OnAttack();
		CurrentWeapon->Attack();
		timeSinceLastAttack = CurrentWeapon->attackSpeed;

		if (CurrentWeapon->SemiAuto)
		{
			isAttacking = false;
		}
	}
	
}

void US47AttackComponent::Attack()
{
	isAttacking = true;
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), OwnerCharacter->GetActorLocation(), 1, OwnerCharacter, 4000);
}

void US47AttackComponent::StopAttack()
{
	isAttacking = false;
}

void US47AttackComponent::AttackBis()
{
	/*if (GetOwnerRole() < ROLE_Authority)
	{
		ServerAttackBisRPC();
	}
	else*/
	{
		CurrentWeapon->AttackBis();
	}
}

void US47AttackComponent::StopAttackBis()
{
	/*if (GetOwnerRole() < ROLE_Authority)
	{
		ServerStopAttackBisRPC();
	
	else*/
	{
		CurrentWeapon->StopAttackBis();
	}
}

void US47AttackComponent::Kick()
{
	OwnerCharacter->OnKick();
}

void US47AttackComponent::ChangeWeapon(float Value)
{
	if (Value == 0)
		return;

	int dir = Value > 0 ? 1 : -1;

	int nextIndex = indexWeapon;

	do
	{
		nextIndex += dir;
		if (nextIndex >= WeaponOnPlayer.Num()) nextIndex = 0;
		if (nextIndex < 0) nextIndex = WeaponOnPlayer.Num() - 1;
	} while (WeaponOnPlayer[nextIndex] == nullptr);

	if (nextIndex != indexWeapon)
	{
		indexWeapon = nextIndex;
		if (GetOwnerRole() < ROLE_Authority)
		{
			ServerChangeCurrentWeapon(indexWeapon);
		}
		else
		{
			ChangeCurrentWeapon();
		}
	}

}

void US47AttackComponent::ChooseWeapon(ES47WeaponIndex WeaponIndex)
{
	if (indexWeapon != WeaponIndex && WeaponOnPlayer[WeaponIndex] != nullptr)
	{
		indexWeapon = WeaponIndex;

		ChangeCurrentWeapon();
	}
}

void US47AttackComponent::ChangeCurrentWeapon()
{
	if (nullptr != GetWorld() && WeaponOnPlayer.Num() > indexWeapon)
	{
		if (nullptr != CurrentWeapon)
		{
			CurrentWeapon->SetActorHiddenInGame(true);
			CurrentWeapon->StopAttackBis();
		}

		CurrentWeapon = WeaponOnPlayer[indexWeapon];
		OwnerCharacter->ChangeWeaponBP();

		if (nullptr != CurrentWeapon)
		{
			CurrentWeapon->SetOwner(OwnerCharacter);
			CurrentWeapon->SetActorHiddenInGame(false);
		}
		OnWeaponSwitched();
	}
}

void US47AttackComponent::ThrowCurrentWeapon(bool isGrenadeLauncherExploding)
{
	if (CurrentWeapon != nullptr)
	{
		if (!CurrentWeapon->IsNotThrowable)
		{
			AS47Weapon* WeaponThrew = nullptr;

			FTransform ProjPoint = OwnerCharacter->FirstPersonCameraComponent->GetComponentTransform();
			ProjPoint.SetLocation(ProjPoint.GetLocation() + ProjPoint.GetRotation().Vector() * 50 + FVector(0, 0, -20));
			ProjPoint.SetScale3D(FVector::OneVector);

			WeaponThrew = GetWorld()->SpawnActor<AS47Weapon>(CurrentWeapon->GetClass(), ProjPoint);

			WeaponThrew->InitProjectile(ProjPoint.GetLocation(), ProjPoint.GetRotation().Vector(), OwnerCharacter->moveComponent->Velocity);
			WeaponThrew->SetOwner(GetOwner());

			if (CurrentWeapon->IsA<AS47Lasergun>())
			{
				OwnerCharacter->FirstPersonCameraComponent->SetFieldOfView(90);
			}

			if (isGrenadeLauncherExploding)
			{
				WeaponThrew->CurrentAmmo = CurrentWeapon->CurrentAmmo;
				if (Cast<AS47GrenadeLauncher>(WeaponThrew) != nullptr)
				{
					float TrueDamage = CurrentWeapon->maxDamage * 2.0f;
					AS47PlayerCharacter* PlayerOwnerCharacter = Cast<AS47PlayerCharacter>(GetOwner());
					AS47PlayerState* PlayerState = nullptr;
					if (PlayerOwnerCharacter != nullptr)
						PlayerState = PlayerOwnerCharacter->GetPlayerState<AS47PlayerState>();

					if (PlayerState != nullptr && PlayerState->levelDamage > 0)
						TrueDamage *= PlayerState->levelDamage * 1.1f;
					Cast<AS47GrenadeLauncher>(WeaponThrew)->SetGenericTeamId(0);
					Cast<AS47GrenadeLauncher>(WeaponThrew)->isExploding = true;
					Cast<AS47GrenadeLauncher>(WeaponThrew)->OnThrow();
					Cast<AS47GrenadeLauncher>(WeaponThrew)->maxDamage = TrueDamage;
				}
			}

			CurrentWeapon->StopAttackBis();
			WeaponGetOutOfInventory();

			// Increase the stat of thrown weapon
			AS47PlayerCharacter* Player = Cast<AS47PlayerCharacter>(GetOwner());
			if (nullptr != Player)
			{
				AS47PlayerState* State = Player->GetPlayerState<AS47PlayerState>();
				if (nullptr != State)
				{
					State->nbWeaponThrown++;
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, *FString::Printf(TEXT("Nb Weapon Thrown: %d"), State->nbWeaponThrown));
				}
			}
		}
	}
}



void US47AttackComponent::SpawnWeapon(int32 index, TSubclassOf<AS47Weapon> weapon)
{
	WeaponOnPlayer[index] = GetWorld()->SpawnActor<AS47Weapon>(weapon);

	if (nullptr != WeaponOnPlayer[index] && nullptr != WeaponOnPlayer[index]->GetRootComponent() && nullptr != OwnerCharacter && nullptr != OwnerCharacter->GetMesh())
	{
		AS47PlayerCharacter* PlayerOwnerCharacter = ((AS47PlayerCharacter*)OwnerCharacter);
		WeaponOnPlayer[index]->AttachToComponent(PlayerOwnerCharacter->GetMesh1P(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::SnapToTarget, false), TEXT("GripPoint"));
		WeaponOnPlayer[index]->OwnerCharacter = OwnerCharacter;
		WeaponOnPlayer[index]->SetOwner(OwnerCharacter);
		timeSinceLastAttack = WeaponOnPlayer[index]->attackSpeed;
		WeaponOnPlayer[index]->SetActorHiddenInGame(true);

		AS47PlayerState* PlayerState = PlayerOwnerCharacter->GetPlayerState<AS47PlayerState>();
		if (PlayerState != nullptr && PlayerState->levelAmmo > 0)
		{
			float trueTotalAmmo = WeaponOnPlayer[index]->TotalAmmo;
			trueTotalAmmo *= PlayerState->levelAmmo * 1.1f;
			WeaponOnPlayer[index]->CurrentAmmo = trueTotalAmmo;
			if (WeaponOnPlayer[index]->IsA<AS47Rifle>())
			{
				AS47Rifle* rifle = Cast<AS47Rifle>(WeaponOnPlayer[index]);				
				rifle->currentAmmoBis = trueTotalAmmo;
			}
		}
	}
}

void US47AttackComponent::ServerThrowCurrentWeapon_Implementation()
{
	ThrowCurrentWeapon(true);
}
bool US47AttackComponent::ServerThrowCurrentWeapon_Validate()
{
	return true;
}

void US47AttackComponent::ServerSpawnWeapon_Implementation(int32 index, TSubclassOf<AS47Weapon> weapon)
{
	SpawnWeapon(index, weapon);
}

bool US47AttackComponent::ServerSpawnWeapon_Validate(int32 index, TSubclassOf<AS47Weapon> weapon)
{
	return true;
}

void US47AttackComponent::OnSpawnerBeginOverlap_Implementation(AActor * _OverlappedActor, AActor * _OtherActor)
{
	if (nullptr != _OtherActor)
	{
		if (_OtherActor->IsA<AS47SpawnerWeapon>())
		{
			AS47SpawnerWeapon* MySpawner = Cast<AS47SpawnerWeapon>(_OtherActor);

			if (nullptr != _OverlappedActor && _OverlappedActor->IsA<AS47Character>() && !MySpawner->isEmpty)
			{
				AS47Character* MyCharacter = Cast<AS47Character>(_OverlappedActor);

				if (MyCharacter->IsValidActorForCollision(_OverlappedActor))
				{
					bool hasAlreadyWeapon = false;
					bool weaponTaken = false;

					for (int j = 0; j < WeaponOnPlayer.Num(); ++j)
					{
						if (nullptr != WeaponOnPlayer[j] && WeaponOnPlayer[j]->GetClass() == MySpawner->GetActorFromSpawner<AS47Weapon>())
						{
							hasAlreadyWeapon = true;

							if (WeaponOnPlayer[j]->CurrentAmmo != WeaponOnPlayer[j]->TotalAmmo)
							{
								WeaponOnPlayer[j]->Reload();
								MySpawner->TakeWeapon();
								weaponTaken = true;
							}
							else if (WeaponOnPlayer[j]->IsA<AS47Rifle>())
							{
								AS47Rifle* rifle = Cast<AS47Rifle>(WeaponOnPlayer[j]);

								if (rifle->currentAmmoBis != rifle->TotalAmmo)
								{
									rifle->Reload();
									MySpawner->TakeWeapon();
								}
							}
						}
					}

					if (!hasAlreadyWeapon)
					{
						for (int i = 0; i < WeaponOnPlayer.Num(); ++i)
						{
							if (WeaponOnPlayer[i] == nullptr)
							{
								indexWeapon = i;
								SpawnWeapon(i, MySpawner->TakeWeapon());
								ChangeCurrentWeapon();
								weaponTaken = true;
								break;
							}
						}
					}

					if (weaponTaken)
					{
						OnWeaponTaken();
						// Increase the stat of taken weapon
						AS47PlayerCharacter* Player = Cast<AS47PlayerCharacter>(GetOwner());
						if (nullptr != Player)
						{
							AS47PlayerState* State = Player->GetPlayerState<AS47PlayerState>();
							if (nullptr != State)
							{
								State->nbWeaponTaken++;
								//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, *FString::Printf(TEXT("Nb Weapon Taken: %d"), State->nbWeaponTaken));
							}
						}
					}
				}
			}
		}
		else if (_OtherActor->IsA<AS47SpawnerEnemy>())
		{
			AS47SpawnerEnemy* MySpawner = Cast<AS47SpawnerEnemy>(_OtherActor);

			if (nullptr != _OverlappedActor && _OverlappedActor->IsA<AS47Character>() && !MySpawner->hasBeenActivated  && MySpawner->triggerSpawn)
			{
				AS47Character* MyCharacter = Cast<AS47Character>(_OverlappedActor);

				if (MyCharacter->IsValidActorForCollision(_OverlappedActor))
				{
					MySpawner->hasBeenActivated = true;
				}
			}
		}
	}
}

bool US47AttackComponent::OnSpawnerBeginOverlap_Validate(AActor * _OverlappedActor_OverlappedActor, AActor * _OtherActor)
{
	return true;
}


void US47AttackComponent::WeaponGetOutOfInventory()
{
	CurrentWeapon->Destroy();
	CurrentWeapon = nullptr;
	WeaponOnPlayer[indexWeapon] = nullptr;

	//if previous exist
	if (WeaponOnPlayer[indexWeapon - 1] != nullptr)
	{
		indexWeapon--;
	}
	else
	{
		int32 tmpCounter = -1;

		//go to the previous which exist
		while (WeaponOnPlayer[WeaponOnPlayer.Num() - 1 + tmpCounter] == nullptr)
		{
			tmpCounter--;
		}

		//if there is no previous weapon
		if (indexWeapon + tmpCounter < 0)
		{
			indexWeapon = 0;
			tmpCounter = 0;
		}
		else
		{
			indexWeapon = WeaponOnPlayer.Num() - 1 + tmpCounter;
		}
	}

	ChangeCurrentWeapon();
}

void US47AttackComponent::OnWeaponSwitched_Implementation()
{
	OnWeaponSwitched_BP.Broadcast();
}

void US47AttackComponent::OnWeaponTaken_Implementation()
{
	OnWeaponTaken_BP.Broadcast();
}

void US47AttackComponent::ServerAttackBisRPC_Implementation()
{
	CurrentWeapon->AttackBis();
}

bool US47AttackComponent::ServerAttackBisRPC_Validate()
{
	return true;
}

void US47AttackComponent::ServerStopAttackBisRPC_Implementation()
{
	CurrentWeapon->StopAttackBis();
}

bool US47AttackComponent::ServerStopAttackBisRPC_Validate()
{
	return true;
}

void US47AttackComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(US47AttackComponent, CurrentWeapon);
	DOREPLIFETIME(US47AttackComponent, indexWeapon);
	//DOREPLIFETIME(US47AttackComponent, StartWeapon);
	DOREPLIFETIME(US47AttackComponent, WeaponOnPlayer);
	DOREPLIFETIME(US47AttackComponent, isAttacking);
	DOREPLIFETIME(US47AttackComponent, OwnerCharacter);
}

void US47AttackComponent::ServerAttackRPC_Implementation()
{
	isAttacking = true;
}

bool US47AttackComponent::ServerAttackRPC_Validate()
{
	return true;
}

void US47AttackComponent::ServerChangeCurrentWeapon_Implementation(int32 _index)
{
	indexWeapon = _index;
	ChangeCurrentWeapon();
}

bool US47AttackComponent::ServerChangeCurrentWeapon_Validate(int32 _index)
{
	return true;
}

void US47AttackComponent::ServerStopAttackRPC_Implementation()
{
	isAttacking = false;
}

bool US47AttackComponent::ServerStopAttackRPC_Validate()
{
	return true;
}

