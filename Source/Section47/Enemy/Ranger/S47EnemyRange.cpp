// Fill out your copyright notice in the Description page of Project Settings.

#include "S47EnemyRange.h"
#include "S47Ranger.h"
#include "Projectile/S47Projectile.h"

#include "Engine.h"

AS47EnemyRange::AS47EnemyRange()
{
}

void AS47EnemyRange::BeginPlay()
{
	Super::BeginPlay();
}

void AS47EnemyRange::AttackRange()
{
	//Super::Attack();  
	Super::OnAttackBP();
	SpawnProjectile();
}

void AS47EnemyRange::Attack()
{
	Super::OnAttackBP();
	SpawnProjectile();
}

TArray<class AS47Projectile*> AS47EnemyRange::SpawnProjectile()
{
	TArray<class AS47Projectile*> AllProj;
	//int IndexProj = 0;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SPAWN PROJECTILE"));


	if (nullptr != GetWorld())
	{
		AS47Projectile* CurrentProjectile = nullptr;

		FTransform ProjPoint;

		//ProjPoint = FP_MuzzleLocation->GetComponentTransform();

		//ProjPoint = ProjPoint * GetActorTransform();
		//ProjPoint = FP_MuzzleLocation->GetSocketTransform("ProjSocket", ERelativeTransformSpace::RTS_World) /** FRotationMatrix::MakeFromZX*/;
		//ProjPoint = FP_MuzzleLocation->GetRelativeTransform();
		//ProjPoint = ProjPoint * FP_MuzzleLocation->GetComponentTransform();
		ProjPoint = GetTransform();
		ProjPoint.SetLocation(GetMesh()->GetSocketLocation("FireSocket"));

		//ProjPoint.SetLocation(GetActorLocation() + FVector::UpVector * 140);
		ProjPoint.SetScale3D(FVector::OneVector);

		CurrentProjectile = GetWorld()->SpawnActor<AS47Projectile>(ProjectileClass, ProjPoint);

		/*findPlayer = true;
		Bullet bu = Instantiate(bullet, transform.position, transform.rotation, null);
		bu.creator = this;
		bu.GetComponent<Rigidbody>().velocity = bu.transform.forward * 50;
		Vector3 anticipation = Player.GetComponent<Rigidbody>().velocity;
		Vector3 zone = new Vector3(Random.Range(-1.0f, 1.0f), Random.Range(-1.0f, 1.0f), Random.Range(-1.0f, 1.0f));
		if (Random.Range(0, 100.0f) < 30)
		{
			bu.GetComponent<Rigidbody>().velocity += anticipation;
		}
		else if (Random.Range(0, 100.0f) < 50)
		{
			bu.GetComponent<Rigidbody>().velocity -= anticipation;
		}*/
		if (m_player != nullptr)
		{
			FVector direction = m_player->GetActorLocation() - ProjPoint.GetLocation();
			FVector anticipation = m_player->GetVelocity();
			FVector zone = FVector(FMath::RandRange(-100.0f, 100.0f), FMath::RandRange(-100.0f, 100.0f), FMath::RandRange(-100.0f, 100.0f));
			if (FMath::RandRange(0.0f, 100.0f) < 30.0f)
			{
				direction += anticipation;
			}
			if (FMath::RandRange(0.0f, 100.0f) < 50.0f)
			{
				direction -= anticipation / 10;
			}
			direction += zone;
			direction.Normalize();
			CurrentProjectile->Damage = m_dmg;
			CurrentProjectile->InitProjectile(ProjPoint.GetLocation(), direction, GetGenericTeamId()); // 0 a changer avec le bon GenericTeamId
			CurrentProjectile->SetOwner(this);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Projectile spawned : %s "), *GetNameSafe(CurrentProjectile)));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Projectile Position : %s "), *CurrentProjectile->GetActorLocation().ToString()));

			AllProj.Add(CurrentProjectile);
		}

	}

	//IndexProj++;


	return AllProj;
}