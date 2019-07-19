// Fill out your copyright notice in the Description page of Project Settings.

#include "S47WeaponProjectile.h"
#include "Projectile/S47Projectile.h"
#include "Player/S47PlayerCharacter.h"

#include "Engine.h"

AS47WeaponProjectile::AS47WeaponProjectile()
{
	StartingProjectilePointTag = "ProjSocket";


	NbProjectiles = 1;
}

void AS47WeaponProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AS47WeaponProjectile::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<AS47PlayerCharacter>(GetNetOwningPlayer());


	if (StartingProjectilesPoints.Num() == 0)
	{
		//Get all sceneComponents with the tag StartingProjectilePointTag to get all relative spawning points from the actor

		TArray<UActorComponent*> AllSceneComps = GetComponentsByClass(USceneComponent::StaticClass());

		for (auto ActorComp : AllSceneComps)
		{
			USceneComponent* SceneComp = Cast<USceneComponent>(ActorComp);

			if (nullptr != SceneComp && SceneComp->ComponentHasTag(StartingProjectilePointTag))
			{
				StartingProjectilesPoints.Add(SceneComp->GetRelativeTransform());
			}
		}

		//Get all sockets with the tag for the proj

		TArray<UActorComponent*> AllMeshComps = GetComponentsByClass(UStaticMeshComponent::StaticClass());

		for (auto ActorComp : AllMeshComps)
		{
			USceneComponent* MeshComp = Cast<USceneComponent>(ActorComp);

			if (nullptr != MeshComp)
			{
				TArray<FName> AllSockets = MeshComp->GetAllSocketNames();

				for (auto Socket : AllSockets)
				{
					if (Socket.ToString().Contains(StartingProjectilePointTag.ToString()))
					{
						StartingProjectilesPoints.Add(MeshComp->GetSocketTransform(Socket, ERelativeTransformSpace::RTS_Actor));
					}
				}
			}
		}
	}
}

void AS47WeaponProjectile::Attack()
{
	if (CurrentAmmo > 0)
	{
		Super::Attack();
		if (GetOwner()->Role < ROLE_Authority)
		{
			ServerSpawnProjectile();
		}
		else
		{
			SpawnProjectile();
		}
	}
	else
		OnAttackEmpty();
}

void AS47WeaponProjectile::AttackBis()
{

}

void AS47WeaponProjectile::StopAttackBis()
{

}



TArray<class AS47Projectile*> AS47WeaponProjectile::SpawnProjectile()
{
	TArray<class AS47Projectile*> AllProj;
	int IndexProj = 0;

	for (auto ProjPoint : StartingProjectilesPoints)
	{
		if (nullptr != GetWorld() && IndexProj < NbProjectiles)
		{
			AS47Projectile* CurrentProjectile = nullptr;
			
			ProjPoint = FP_MuzzleLocation->GetComponentTransform();

			ProjPoint.SetScale3D(FVector::OneVector);

			CurrentProjectile = GetWorld()->SpawnActor<AS47Projectile>(ProjectileClass, ProjPoint);
			CurrentProjectile->SetOwner(GetOwner());

			float TrueDamage = maxDamage;
			AS47PlayerCharacter* PlayerOwnerCharacter = Cast<AS47PlayerCharacter>(GetOwner());
			AS47PlayerState* PlayerState = nullptr;
			if (PlayerOwnerCharacter != nullptr)
				PlayerState = PlayerOwnerCharacter->GetPlayerState<AS47PlayerState>();

			if (PlayerState != nullptr && PlayerState->levelDamage > 0)
				TrueDamage *= PlayerState->levelDamage * 1.1f;

			if (OwnerCharacter->hasDoubleDamageActive)
			{
				CurrentProjectile->Damage = TrueDamage * 2.0f;
			}
			else
			{
				CurrentProjectile->Damage = TrueDamage;
			}

			CurrentProjectile->InitProjectile(ProjPoint.GetLocation(), ProjPoint.GetRotation().Vector(), 0); // 0 a changer avec le bon GenericTeamId

			AllProj.Add(CurrentProjectile);
		}

		IndexProj++;
	}

	return AllProj;
}

void AS47WeaponProjectile::ServerSpawnProjectile_Implementation()
{
	SpawnProjectile();
}

bool AS47WeaponProjectile::ServerSpawnProjectile_Validate()
{
	return true;
}

