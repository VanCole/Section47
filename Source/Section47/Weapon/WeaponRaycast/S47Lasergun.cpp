// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Lasergun.h"
#include "S47Character.h"
#include "Player/S47PlayerCharacter.h"
#include "AI/S47AICharacter.h"
#include "Enemy/S47Enemy.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h" 
#include "Components/S47AttackComponent.h"
#include "Projectile/S47Impact.h"
#include "Components/S47CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

AS47Lasergun::AS47Lasergun()
{
	weightWeapon = 40;
	CanAttack = false;
	originPos = GetActorTransform().GetLocation();
}

void AS47Lasergun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* playerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (nullptr == playerController)
		return;
	if (nullptr == Cast<AS47PlayerCharacter>(GetOwner()))
		return;
	if (!Cast<AS47PlayerCharacter>(GetOwner())->IsLocallyControlled())
		return;
	if (Cast<AS47PlayerCharacter>(GetOwner())->GetAttackComponent()->GetCurrentWeapon() != this)
	{
		CanAttack = false;
		AmmoConsumed = 0;
		timerCharging = timeToCharge;
		return;
	}

	ZoomTo(currentFOV, wantedFOV);

	if (timerCharging > 0)
		timerCharging -= DeltaTime;

	//ShakeWeapon(DeltaTime);

	if (playerController->WasInputKeyJustPressed(EKeys::LeftMouseButton) && CurrentAmmo > 0)
	{
		CanAttack = true;
		OnStartConsumingBP();
	}
	else if (playerController->WasInputKeyJustPressed(EKeys::LeftMouseButton) && CurrentAmmo <= 0)
	{
		OnAttackEmpty();
	}
	if (playerController->IsInputKeyDown(EKeys::LeftMouseButton) && CanAttack)
	{
		OnBeamUpdateBP();

		if (timerCharging <= 0)
		{
			timerCharging = timeToCharge;

			if (CurrentAmmo > 0 && AmmoConsumed < MaxAmmoConsumed)
			{
				AmmoConsumed++;
				ConsumeAmmo();
			}
			OnConsumeBP();
			if (AmmoConsumed == MaxAmmoConsumed)
			{
				ShakeDuration = 0.1f;
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Charging")));
	}
	if (playerController->WasInputKeyJustReleased(EKeys::LeftMouseButton) && AmmoConsumed > 0)
	{
		LaserGunAttack();
		OnAttack();
		AmmoConsumed = 0;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Fire !")));
	}
}

void AS47Lasergun::Attack()
{
	//Super::Attack();
}

void AS47Lasergun::StopAttack()
{

}

void AS47Lasergun::AttackBis()
{
	Super::AttackBis();
	OnAttackBis();
	wantedFOV = 40;
	//Cast<AS47PlayerCharacter>(GetOwner())->FirstPersonCameraComponent->SetFieldOfView(40);
}

void AS47Lasergun::StopAttackBis()
{
	Super::StopAttackBis();
	OnStopAttackBis();
	wantedFOV = 90;
	//Cast<AS47PlayerCharacter>(GetOwner())->FirstPersonCameraComponent->SetFieldOfView(90);
}

void AS47Lasergun::ZoomTo(float _oldFOV, float _newFOV)
{
	currentFOV = UKismetMathLibrary::Lerp(_oldFOV, _newFOV, 0.2f);
	if (nullptr != Cast<AS47PlayerCharacter>(GetOwner()))
	{
		Cast<AS47PlayerCharacter>(GetOwner())->FirstPersonCameraComponent->SetFieldOfView(currentFOV);
	}
}

void AS47Lasergun::ResetZoom()
{
	if (nullptr != Cast<AS47PlayerCharacter>(GetOwner()))
	{
		Cast<AS47PlayerCharacter>(GetOwner())->FirstPersonCameraComponent->SetFieldOfView(90);
		wantedFOV = 90;
		currentFOV = 90;
	}
}

void AS47Lasergun::LaserGunAttack()
{
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{

		FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

		FVector StartLocation;
		FVector RayDirection;

		if (Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		{
			Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->DeprojectScreenPositionToWorld(ViewportCenter.X, ViewportCenter.Y, StartLocation, RayDirection);
		}


		FVector EndRayCast = StartLocation + (RayDirection * 10000000.f);

		if (nullptr != GetWorld())
		{
			FCollisionQueryParams QueryParams;

			TArray<FHitResult> FoundHit;

			GetWorld()->LineTraceMultiByChannel(FoundHit, StartLocation, EndRayCast, ECollisionChannel::ECC_Camera, QueryParams);

			if (FoundHit.Num() == 0)
				PosHit.Add(StartLocation + (RayDirection * 10000.f));

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d"), FoundHit.Num()));

			AActor* actorHitted = nullptr;
			for (auto hit : FoundHit)
			{
				if (Cast<AS47AICharacter>(hit.GetActor()))
				{


					if (actorHitted == hit.GetActor())
					{
						continue;
					}

					actorHitted = hit.GetActor();
					float dmg = maxDamage * AmmoConsumed;
					FKAggregateGeom aggGeom = Cast<USkeletalMeshComponent>(hit.GetComponent())->GetBodyInstance(hit.BoneName)->BodySetup->AggGeom; //Aucune idee de quest ce que cette merde c'est mais je récupère les collisions grace à ça
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

					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Projectile Hit : %s "), *collisionName));

					AS47PlayerCharacter* PlayerOwnerCharacter = Cast<AS47PlayerCharacter>(GetOwner());
					AS47PlayerState* PlayerState = nullptr;
					if (PlayerOwnerCharacter != nullptr)
						PlayerState = PlayerOwnerCharacter->GetPlayerState<AS47PlayerState>();

					if (PlayerState != nullptr && PlayerState->levelDamage > 0)
						dmg *= PlayerState->levelDamage * 1.1f;

					if (collisionName == "DoubleDMG")
					{
						dmg *= 2;
					}
					if (collisionName == "AddDMG")
					{
						dmg *= 1.5f;
					}
					if (collisionName == "HalfDMG")
					{
						dmg *= 0.5f;
					}

					if (Cast<AS47PlayerCharacter>(GetOwner()) != nullptr && Cast<AS47PlayerCharacter>(GetOwner())->hasDoubleDamageActive)
					{
						dmg *= 2;
					}

					if (nullptr != Cast<AS47AICharacter>(hit.GetActor()))
					{
						float BaseDamage = 10;

						if (PlayerOwnerCharacter != nullptr)
							BaseDamage *= PlayerState->levelDamage * 1.1f;

						FVector dir = hit.ImpactPoint - GetOwner()->GetActorLocation();
						dir.Normalize();

						AS47AICharacter* Enemy = Cast<AS47AICharacter>(hit.GetActor());
						AS47Enemy* S47Enemy = Cast<AS47Enemy>(hit.GetActor());

						if (Enemy->HasAuthority())
						{
							if (S47Enemy == nullptr || S47Enemy->m_isAlive)
							{
								HitEnemy_ToClient(Enemy, hit.ImpactPoint, true, UKismetMathLibrary::LinearColorLerp(FColor::Blue, FColor::Red, float(AmmoConsumed) / float(MaxAmmoConsumed)));
								SaveLastShotOnEnemy_ToClient(Enemy, dir * 0.0f, hit.ImpactPoint, hit.BoneName);
							}
							else
							{
								Enemy->SaveLastShot(dir * 0.0f, hit.ImpactPoint, hit.BoneName);
								Enemy->OnHitBP(hit.ImpactPoint, true, UKismetMathLibrary::LinearColorLerp(FColor::Blue, FColor::Red, float(AmmoConsumed) / float(MaxAmmoConsumed)));
							}
						}
						else
						{
							if (S47Enemy == nullptr || S47Enemy->m_isAlive)
							{
								HitEnemy_ToServ(Enemy, hit.ImpactPoint, true, UKismetMathLibrary::LinearColorLerp(FColor::Blue, FColor::Red, float(AmmoConsumed) / float(MaxAmmoConsumed)));
								SaveLastShotOnEnemy_ToServ(Enemy, dir * 0.0f, hit.ImpactPoint, hit.BoneName);
							}
							Enemy->SaveLastShot(dir * 0.0f, hit.ImpactPoint, hit.BoneName);
							Enemy->OnHitBP(hit.ImpactPoint, true, UKismetMathLibrary::LinearColorLerp(FColor::Blue, FColor::Red, float(AmmoConsumed) / float(MaxAmmoConsumed)));
						}

						ServerGetDamage(Cast<AS47AICharacter>(hit.GetActor()), BaseDamage + dmg);
					}
				}
				else
				{
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					if (nullptr != ImpactClass)
					{
						AS47Impact* impact = World->SpawnActor<AS47Impact>(ImpactClass, hit.ImpactPoint, hit.Normal.ToOrientationRotator(), ActorSpawnParams);
						impact->AttachToActor(hit.GetActor(), FAttachmentTransformRules::KeepWorldTransform);
					}
				}
				PosHit.Add(hit.ImpactPoint);
			}
		}
	}
}

void AS47Lasergun::ShakeWeapon(float _deltaTime)
{
	if (ShakeDuration > 0)
	{
		GetParentActor()->GetActorTransform().SetLocation(originPos + FVector(FMath::RandPointInCircle(ShakeAmount).X, 0, FMath::RandPointInCircle(ShakeAmount).Y));

		ShakeDuration -= _deltaTime * ShakeDecrease;
	}
	else
	{
		ShakeDuration = 0.0f;
		GetParentActor()->GetActorTransform().SetLocation(originPos);
	}
}
