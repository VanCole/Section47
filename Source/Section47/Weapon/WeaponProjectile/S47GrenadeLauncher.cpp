// Fill out your copyright notice in the Description page of Project Settings.

#include "S47GrenadeLauncher.h"
#include "Player/S47PlayerCharacter.h"
#include "Projectile/S47Projectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "AI/S47AICharacter.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/S47AttackComponent.h"

AS47GrenadeLauncher::AS47GrenadeLauncher()
{
	weightWeapon = 30;
	AOETrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	if (nullptr != AOETrigger && nullptr != MeshComponent)
		AOETrigger->SetupAttachment(MeshComponent);

}

void AS47GrenadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (nullptr == Cast<AS47PlayerCharacter>(GetOwner()))
		return;

	if (isCharging && chargeTimer > 0)
		chargeTimer -= DeltaTime;
	if (chargeTimer <= timeToCharge * 0.66f && chargeTimer > timeToCharge * 0.33f)
	{
		OnCharge2();
		Cast<AS47PlayerCharacter>(GetOwner())->FirstPersonCameraComponent->CameraShake(CAMERASHAKE::GrenadeLauncherLight);
	}
	if (chargeTimer <= timeToCharge * 0.33f)
	{
		OnCharge3();
		Cast<AS47PlayerCharacter>(GetOwner())->FirstPersonCameraComponent->CameraShake(CAMERASHAKE::GrenadeLauncherHard);
	}
	if (chargeTimer <= 0)
	{
		if (Cast<AS47PlayerCharacter>(GetOwner())->Role < ROLE_Authority)
		{
			Cast<AS47PlayerCharacter>(GetOwner())->GetAttackComponent()->ServerThrowCurrentWeapon();

		}
		else
		{
			Cast<AS47PlayerCharacter>(GetOwner())->GetAttackComponent()->ThrowCurrentWeapon(true);

		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, *FString::Printf(TEXT("TimeBefore Explod %f"), explodeTimer));

	if (isExploding && explodeTimer > 0)	
		explodeTimer -= DeltaTime;	
	if (explodeTimer <= 0)	
		Explode();
	
}


void AS47GrenadeLauncher::AttackBis()
{
	isCharging = true;
	OnCharge1();
}

void AS47GrenadeLauncher::StopAttackBis()
{
	isCharging = false;
	chargeTimer = timeToCharge;
	OnDischarge();
}

void AS47GrenadeLauncher::Explode()
{
	if (HasAuthority())
	{
		TArray<AActor*> FoundActors;
		TArray<UPrimitiveComponent*> FoundComponents;
		AOETrigger->GetOverlappingActors(FoundActors);
		AOETrigger->GetOverlappingComponents(FoundComponents);

		for (auto enemys : FoundActors)
		{
			AS47AICharacter* enemy = Cast<AS47AICharacter>(enemys);
			if (IsValid(enemy))
			{
				if (GetGenericTeamId() != Cast<AS47Character>(enemys)->GetGenericTeamId())
				{
					
					float range = FVector::Dist(enemy->GetActorLocation(), GetActorLocation());
					float dmg = (((range - 0) * (0 - (maxDamage * 2.0f))) / (AOETrigger->GetScaledSphereRadius() - 0)) + (maxDamage * 2.0f);

					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Explode %f"), dmg));

					if (dmg > 0)
						enemy->GetDamage(dmg, GetOwner());
					FVector dir = enemys->GetActorLocation() - GetActorLocation();
					dir.Normalize();
					enemy->SaveLastShot_ToClient(dir * dmg * 1000.0f, enemys->GetActorLocation(), "None");
				}
			}
		}

		for (auto enemys : FoundComponents)
		{
			if (enemys->IsSimulatingPhysics())
			{
				FVector dir = enemys->GetComponentLocation() - GetActorLocation();
				dir.Normalize();
				AddImpulse_ToClient(enemys, dir * maxDamage * 1000.0f, enemys->GetComponentLocation());
				//Cast<USkeletalMeshComponent>(ResultHit.GetComponent())->BreakConstraint(dir, ResultHit.ImpactPoint, ResultHit.BoneName);
			}
		}

		for (int32 i = 0; i < CurrentAmmo; i++)
		{
			AS47Projectile* CurrentProjectile = nullptr;

			FTransform ProjPoint;
			ProjPoint.SetLocation(FVector(FMath::RandPointInCircle(5).X, FMath::RandPointInCircle(5).Y, FMath::RandPointInCircle(5).Y) + MeshComponent->GetComponentLocation());
			ProjPoint.SetRotation(UKismetMathLibrary::FindLookAtRotation(ProjPoint.GetLocation(), MeshComponent->GetComponentLocation()).Quaternion());

			CurrentProjectile = GetWorld()->SpawnActor<AS47Projectile>(ProjectileClass, ProjPoint);
			CurrentProjectile->InitProjectile(ProjPoint.GetLocation(), ProjPoint.GetRotation().Inverse().Vector(), 0); // 0 a changer avec le bon GenericTeamId
			CurrentProjectile->SetOwner(GetOwner());
			CurrentProjectile->Damage = maxDamage;
		}
	
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Explode "));
		OnExplodingBP_ToClient();
	}

	//OnExplodingBP();
	//Destroy();
}

void AS47GrenadeLauncher::SetGenericTeamId(const FGenericTeamId & NewTeamID)
{
	if (S47TeamID != NewTeamID)
	{
		S47TeamID = NewTeamID;
	}
}

void AS47GrenadeLauncher::OnExplodingBP_ToClient_Implementation()
{
	OnExplodingBP();
}

void AS47GrenadeLauncher::AddImpulse_ToClient_Implementation(UPrimitiveComponent* _Enemy, FVector _Impulse, FVector _Location)
{
	_Enemy->AddImpulseAtLocation(_Impulse, _Location);
}

