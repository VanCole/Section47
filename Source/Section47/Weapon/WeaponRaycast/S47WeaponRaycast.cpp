// Fill out your copyright notice in the Description page of Project Settings.

#include "S47WeaponRaycast.h"
#include "Weapon/S47Weapon.h"
#include "S47Character.h"
#include "Player/S47PlayerCharacter.h"
#include "AI/S47AICharacter.h"
#include "Enemy/S47Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Projectile/S47Impact.h"
#include "SceneView.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"
#include "Engine.h"
#include "UnrealNetwork.h"

#include "DrawDebugHelpers.h"


AS47WeaponRaycast::AS47WeaponRaycast()
{

}

void AS47WeaponRaycast::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AS47WeaponRaycast::BeginPlay()
{
	Super::BeginPlay();
}

//#pragma optimize("",off)
void AS47WeaponRaycast::Attack()
{

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{

		//const FRotator SpawnRotation = GetControlRotation();
		//// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		//const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		FVector2D ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

		FVector StartLocation;
		FVector RayDirection;
		if (precision > 0)
		{
			FVector2D offset = FMath::RandPointInCircle(precision);
			ViewportCenter += offset;
		}

		if (rifleDecalage != 0)
			ViewportCenter.X += rifleDecalage;

		if (nullptr != Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		{
			Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->DeprojectScreenPositionToWorld(ViewportCenter.X, ViewportCenter.Y, StartLocation, RayDirection);
		}

		//AActor* MyOwner = Cast<AActor>(OwnerCharacter);

		ShootRaycast(StartLocation, RayDirection);

	}
	Super::Attack();
}

void AS47WeaponRaycast::StopAttack()
{
	Super::StopAttack();
}


////#pragma optimize("",on)


void AS47WeaponRaycast::ShootRaycast(FVector StartLocation, FVector RayDirection)
{
	FVector EndRayCast = StartLocation + (RayDirection * 10000000.f);
	FHitResult ResultHit;

	if (nullptr != GetWorld())
	{
		FCollisionQueryParams QueryParams;
		QueryParams.bTraceComplex = true;
		GetWorld()->LineTraceSingleByChannel(ResultHit, StartLocation, EndRayCast, ECollisionChannel::ECC_Visibility, QueryParams);

		

		//DrawDebugLine(GetWorld(), StartLocation, EndRayCast, FColor::Red, false, 5.f);

		//if (ResultHit.bBlockingHit && nullptr != ResultHit.GetActor() && ResultHit.GetActor()->ActorHasTag(TEXT("TrapGround")))
		if (ResultHit.bBlockingHit)
		{
			float range = FVector::Dist(ResultHit.ImpactPoint, GetOwner()->GetActorLocation());

			float dmg = (((range - minRange) * (minDamage - maxDamage)) / (maxRange - minRange)) + maxDamage;
			if (dmg >= maxDamage)
				dmg = maxDamage;
			if (dmg <= minDamage)
				dmg = minDamage;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), dmg));

			if (Cast<AS47AICharacter>(ResultHit.GetActor()))
			{
				//ResultHit.GetComponent();
				float dmgWithLocalizedDamage = dmg;
				FKAggregateGeom aggGeom = Cast<USkeletalMeshComponent>(ResultHit.GetComponent())->GetBodyInstance(ResultHit.BoneName)->BodySetup->AggGeom; //Aucune idee de quest ce que cette merde c'est mais je récupère les collisions grace à ça
				FString collisionName = "None";
				if (aggGeom.BoxElems.Num() > 0)
				{
					collisionName = aggGeom.BoxElems[0].GetName().ToString();
				}
				else if (aggGeom.ConvexElems.Num() > 0)
				{
					collisionName = aggGeom.ConvexElems[0].GetName().ToString();
				}
				else if (aggGeom.SphereElems.Num() > 0)
				{
					collisionName = aggGeom.SphereElems[0].GetName().ToString();
				}
				else if (aggGeom.SphylElems.Num() > 0)
				{
					collisionName = aggGeom.SphylElems[0].GetName().ToString();
				}
				else if (aggGeom.TaperedCapsuleElems.Num() > 0)
				{
					collisionName = aggGeom.TaperedCapsuleElems[0].GetName().ToString();
				}

				AS47PlayerCharacter* PlayerOwnerCharacter = Cast<AS47PlayerCharacter>(GetOwner());
				AS47PlayerState* PlayerState = nullptr;
				if (PlayerOwnerCharacter != nullptr)
					PlayerState = PlayerOwnerCharacter->GetPlayerState<AS47PlayerState>();

				if (PlayerState != nullptr && PlayerState->levelDamage > 0)
					dmgWithLocalizedDamage *= PlayerState->levelDamage * 1.1f;

				if (collisionName == "DoubleDMG")
				{
					dmgWithLocalizedDamage *= 2;
				}
				if (collisionName == "AddDMG")
				{
					dmgWithLocalizedDamage *= 1.5f;
				}
				if (collisionName == "HalfDMG")
				{
					dmgWithLocalizedDamage *= 0.5f;
				}

				if (nullptr != Cast<AS47PlayerCharacter>(GetOwner()) && Cast<AS47PlayerCharacter>(GetOwner())->hasDoubleDamageActive)
				{
					dmgWithLocalizedDamage *= 2;
				}
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Damage : %f "), dmgWithLocalizedDamage));

				if (Cast<AS47AICharacter>(ResultHit.GetActor()) != nullptr)
				{
					FVector dir = ResultHit.ImpactPoint - GetOwner()->GetActorLocation();
					dir.Normalize();

					AS47AICharacter* Enemy = Cast<AS47AICharacter>(ResultHit.GetActor());
					AS47Enemy* S47Enemy = Cast<AS47Enemy>(ResultHit.GetActor());

					if (Enemy->HasAuthority())
					{
						if (S47Enemy == nullptr || S47Enemy->m_isAlive)
						{
							HitEnemy_ToClient(Enemy, ResultHit.ImpactPoint, false, FLinearColor::Black);
							SaveLastShotOnEnemy_ToClient(Enemy, dir * dmg * 1000.0f, ResultHit.ImpactPoint, ResultHit.BoneName);
						}
						else
						{
							Enemy->SaveLastShot(dir * dmg * 1000.0f, ResultHit.ImpactPoint, ResultHit.BoneName);
							Enemy->OnHitBP(ResultHit.ImpactPoint);
						}
					}
					else
					{
						if (S47Enemy == nullptr || S47Enemy->m_isAlive)
						{
							HitEnemy_ToServ(Enemy, ResultHit.ImpactPoint, false, FLinearColor::Black);
							SaveLastShotOnEnemy_ToServ(Enemy, dir * dmg * 1000.0f, ResultHit.ImpactPoint, ResultHit.BoneName);
						}
						Enemy->SaveLastShot(dir * dmg * 1000.0f, ResultHit.ImpactPoint, ResultHit.BoneName);
						Enemy->OnHitBP(ResultHit.ImpactPoint);
					}

					ServerGetDamage(Cast<AS47AICharacter>(ResultHit.GetActor()), dmgWithLocalizedDamage);
				}
			}
			else
			{
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				if (nullptr != ImpactClass)
				{
					AS47Impact* impact = GetWorld()->SpawnActor<AS47Impact>(ImpactClass, ResultHit.ImpactPoint, ResultHit.Normal.ToOrientationRotator(), ActorSpawnParams);
					impact->AttachToActor(ResultHit.GetActor(), FAttachmentTransformRules::KeepWorldTransform);
				}
			}

			if (ResultHit.GetComponent()->IsSimulatingPhysics())
			{
				FVector dir = ResultHit.ImpactPoint - GetOwner()->GetActorLocation();
				dir.Normalize();
				ResultHit.GetComponent()->AddImpulseAtLocation(dir * dmg * 1000.0f, ResultHit.ImpactPoint, ResultHit.BoneName);
				//Cast<USkeletalMeshComponent>(ResultHit.GetComponent())->BreakConstraint(dir, ResultHit.ImpactPoint, ResultHit.BoneName);
			}
			//impact->SetOwner(ResultHit.GetActor());
			PosHit.Add(ResultHit.ImpactPoint);
		}
		else
		{
			PosHit.Add(StartLocation + (RayDirection * 1000.f));
		}
	}
}

void AS47WeaponRaycast::ServerGetDamage_Implementation(AS47AICharacter* _ResultHit, float _Damage)
{
	if (_ResultHit)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *FString::Printf(TEXT("Damage Dealed: %f"), _Damage));
		_ResultHit->GetDamage(_Damage, GetOwner());
	}
}
bool AS47WeaponRaycast::ServerGetDamage_Validate(AS47AICharacter* _ResultHit, float _Damage)
{
	return true;
}

void AS47WeaponRaycast::AttackRayCast()
{
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		FVector2D ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

		FVector StartLocation;
		FVector RayDirection;

		if (precision > 0)
		{
			FVector2D offset = FMath::RandPointInCircle(precision);
			ViewportCenter += offset;
		}

		if (rifleDecalage != 0)
			ViewportCenter.X += rifleDecalage;

		if (nullptr != Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		{
			Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->DeprojectScreenPositionToWorld(ViewportCenter.X, ViewportCenter.Y, StartLocation, RayDirection);
		}

		ShootRaycast(StartLocation, RayDirection);
	}
}

void AS47WeaponRaycast::AttackBis()
{
}

void AS47WeaponRaycast::StopAttackBis()
{
}

void AS47WeaponRaycast::Reload()
{
	Super::Reload();
}

