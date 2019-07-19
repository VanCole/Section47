// Fill out your copyright notice in the Description page of Project Settings.

#include "S47BossDistance.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Projectile/S47Projectile.h"
#include "Components/SphereComponent.h"
#include "S47GameState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"



void AS47BossDistance::BeginPlay()
{
	Super::BeginPlay();
	currentHealth = m_lifeMax;
	MyController = Cast<AS47AIController>(GetController());

	TArray<USceneComponent*> StaticComps;
	GetComponents<USceneComponent>(StaticComps);
	for (USceneComponent* comp : StaticComps)
	{
		if (comp->GetName() == "PivotBras1")
		{
			Bras1 = comp;
		}

		if (comp->GetName().Contains(TEXT("JetColliderRight")))
		{
			JetCollidersRight.Add(comp);
		}
		if (comp->GetName().Contains(TEXT("JetColliderLeft")))
		{
			JetCollidersLeft.Add(comp);
		}
	}

	fireLeft = GetMesh()->GetSocketByName("Fire_SocketL");
	fireRight = GetMesh()->GetSocketByName("Fire_SocketR");
	//Bras1 = StaticComps[3];
	//Bras2 = StaticComps[6];
	if (nullptr != Bras1)
	{
		Bras1->AddLocalRotation(FRotator(0, -90, 0));
	}

	AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
	if (nullptr != GameState)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, TEXT("LockerBase BeginPlay"));
		GameState->OnBossStart.AddUniqueDynamic(this, &AS47BossDistance::StartBoss);
	}
	//Bras2->AddLocalRotation(FRotator(0, 80, 0));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Bras  = %s"), *Bras1->GetName()));

	onEndDeathAnimation.AddUniqueDynamic(this, &AS47BossDistance::AnimationEnd);
}

void AS47BossDistance::SpawnProjectile(FVector _InitLocation, FVector _Direction)
{
	if (GetNetMode() == ENetMode::NM_Client)
	{
		return;
	}
	/*if (!IsLocallyControlled())
	{
		return;
	}*/
	AS47Projectile* CurrentProjectile = nullptr;

	FTransform ProjPoint = GetTransform();
	ProjPoint.SetScale3D(FVector::OneVector);

	CurrentProjectile = GetWorld()->SpawnActor<AS47Projectile>(ProjectileClass);
	CurrentProjectile->SetActorLocation(_InitLocation);
	CurrentProjectile->SetActorRotation(_Direction.ToOrientationQuat());
	CurrentProjectile->Damage = ProjectileDmg;
	CurrentProjectile->InitProjectile(GetActorLocation(), _Direction, GetGenericTeamId());
	CurrentProjectile->SetOwner(this);
}

void AS47BossDistance::SpawnProjectileBP(FVector _InitLocation, FVector _Direction)
{
	AS47Projectile* CurrentProjectile = nullptr;

	FTransform ProjPoint = GetTransform();
	ProjPoint.SetScale3D(FVector::OneVector);

	CurrentProjectile = GetWorld()->SpawnActor<AS47Projectile>(ProjectileClassBP);
	CurrentProjectile->SetActorLocation(_InitLocation);
	CurrentProjectile->SetActorRotation(_Direction.ToOrientationQuat());
	CurrentProjectile->Damage = ProjectileDmg;
	CurrentProjectile->InitProjectile(GetActorLocation(), _Direction, GetGenericTeamId());
	CurrentProjectile->SetOwner(this);
}


//int AS47BossDistance::SetPhase()
//{
//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("TimePhase  = %f"), TimeSinceLastPhase));
//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("DelataSecond  = %f"), GetWorld()->GetDeltaSeconds()));
//	if (!init)
//	{
//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Coucou")));
//		InitArmMovement(120, 60, true);
//	}
//	TimeSinceLastPhase -= GetWorld()->GetDeltaSeconds();
//	if (TimeSinceLastPhase < 0)
//	{
//		NewPhaseBP();
//		TimeSinceLastPhase = timeBeetweenPhase;
//		float randomMultipleProjectile = 0;
//		float randomJetAcide = 0;
//		if (multiProjectilePhasePoids > 0)
//		{
//			randomMultipleProjectile = FMath::RandRange(1, multiProjectilePhasePoids);
//		}
//		if (JetAcidePhasePoids > 0)
//		{
//			randomJetAcide = FMath::RandRange(1, JetAcidePhasePoids);
//		}
//		if (randomMultipleProjectile > randomJetAcide)
//		{
//
//			return 1;
//		}
//		//NewPhaseBP();
//		return 2;
//
//	}
//	else
//	{
//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("TimePhase  = %f"), timeBeetweenPhase - TimeSinceLastPhase));
//		if (nullptr != Bras1)
//		{
//			Bras1->SetRelativeRotation(FMath::Lerp(originQuat, destinationQuat, (timeBeetweenPhase - TimeSinceLastPhase) / timeBeetweenPhase));
//		}
//		//NewPhaseBP();
//		return 0;
//	}
//}

void AS47BossDistance::OnFireRight()
{
	if (fireRight != nullptr)
	{
		FVector dir = fireRight->GetSocketTransform(GetMesh()).GetRotation().GetForwardVector();
		dir.Normalize();
		SpawnProjectile(fireRight->GetSocketLocation(GetMesh()), dir);
	}
}

void AS47BossDistance::OnFireLeft()
{
	if (fireRight != nullptr)
	{
		FVector dir = fireLeft->GetSocketTransform(GetMesh()).GetRotation().GetForwardVector();
		dir.Normalize();
		SpawnProjectile(fireLeft->GetSocketLocation(GetMesh()), dir);
	}
}

//Angle max = angle normal, angle min use if _setRandomAngle is True, _FocusPlayer to rotate in direction of player, startExtremite : -1 ignore, 0 closest to the boss's arm, 1 le plus loin du bras du boss
//void AS47BossDistance::InitArmMovement(int _angleMovementMax, int _angleMovementMin, bool _setRandomAngle, bool _focusPlayer, int _startExtremite)
//{
//	originQuat = Bras1->GetRelativeRotationFromWorld(Bras1->GetComponentQuat());
//	float Yaw = Bras1->GetRelativeRotationFromWorld(Bras1->GetComponentQuat()).Rotator().Yaw;
//	int angleMovement = _angleMovementMax;
//	if (_setRandomAngle)
//	{
//		angleMovement = FMath::RandRange(_angleMovementMin, _angleMovementMax);
//	}
//
//	if (_startExtremite > -1)
//	{
//
//		if (_startExtremite == 0)
//		{
//			if (Yaw < -70)
//			{
//				Yaw = -140;
//				colliderRight = true;
//			}
//			else
//			{
//				Yaw = -10;
//				colliderRight = false;
//			}
//		}
//		else
//		{
//			if (Yaw < -70)
//			{
//				Yaw = -10;
//			}
//			else
//			{
//				Yaw = -140;
//			}
//		}
//	}
//	else if (_focusPlayer)
//	{
//		FVector playerVector = Bras1->GetForwardVector();
//		if (nullptr != m_player)
//		{
//			playerVector = m_player->GetActorLocation() - Bras1->GetComponentLocation();
//		}
//		TArray<USceneComponent*> StaticComps;
//		Bras1->GetChildrenComponents(true, StaticComps);
//		USceneComponent* ChildBras = nullptr;
//		for (USceneComponent* childComp : StaticComps)
//		{
//			if (childComp->GetName() == "Bras1")
//			{
//				ChildBras = childComp;
//			}
//		}
//		FVector playerArm = m_player->GetActorLocation() - ChildBras->GetComponentLocation();
//
//		//bool right = (FVector::DotProduct(GetActorRightVector(), playerVector) < 0);
//		bool right = ((playerVector.X * playerArm.Y - playerVector.Y * playerArm.X) < 0);
//		if (right)
//		{
//			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Coucou")));
//			Yaw -= angleMovement;
//		}
//		else
//		{
//			Yaw += angleMovement;
//		}
//	}
//	else if (FMath::RandBool())
//	{
//		if (Yaw < -110)
//		{
//			Yaw += angleMovement;
//		}
//		else
//		{
//			Yaw -= angleMovement;
//		}
//	}
//	else
//	{
//		if (Yaw > -40)
//		{
//			Yaw -= angleMovement;
//		}
//		else
//		{
//			Yaw += angleMovement;
//		}
//	}
//	if (Yaw > -10)
//	{
//		Yaw = -10;
//	}
//	if (Yaw < -140)
//	{
//		Yaw = -140;
//	}
//
//	FRotator destinationRot = FRotator(Bras1->GetRelativeRotationFromWorld(Bras1->GetComponentQuat()).Rotator().Pitch, Yaw, Bras1->GetRelativeRotationFromWorld(Bras1->GetComponentQuat()).Rotator().Roll);
//	destinationQuat = destinationRot.Quaternion();
//}

//void AS47BossDistance::MultiProjectilePhase(float DeltaTime)
//{
//	if (init)
//	{
//		InitArmMovement(100, 70, true, true);
//
//		init = false;
//	}
//	TArray<USceneComponent*> StaticComps;
//	Bras1->GetChildrenComponents(true, StaticComps);
//	USceneComponent* ChildSocketTir = nullptr;
//	for (USceneComponent* childComp : StaticComps)
//	{
//		if (childComp->GetName() == "SocketTir")
//		{
//			ChildSocketTir = childComp;
//		}
//	}
//	timeInPhase += DeltaTime;
//	if (timeInPhase < 2)
//	{
//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("IN")));
//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Ending %f"), Yaw));
//		//Bras1->GetComponentQuat() = FMath::Lerp(originQuat, destinationQuat, 0.6f);
//		Bras1->SetRelativeRotation(FMath::Lerp(originQuat, destinationQuat, timeInPhase / 2));
//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("name 0 : %s"), *Bras1->GetChildComponent(0)->GetName()));
//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("name 1 : %s"), *ChildSocketTir->GetName()));
//
//		if (timeInPhase < 0.2f)
//		{
//			if (!projectile[0])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[0] = true;
//			}
//
//		}
//		else if (timeInPhase > 0.2f && timeInPhase < 0.4f)
//		{
//			if (!projectile[1])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[1] = true;
//			}
//		}
//		else if (timeInPhase > 0.4f && timeInPhase < 0.6f)
//		{
//			if (!projectile[2])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[2] = true;
//			}
//		}
//		else if (timeInPhase > 0.6f && timeInPhase < 0.8f)
//		{
//			if (!projectile[3])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[3] = true;
//			}
//		}
//		else if (timeInPhase > 0.8f && timeInPhase < 1.0f)
//		{
//			if (!projectile[4])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[4] = true;
//			}
//		}
//		else if (timeInPhase > 1.0f && timeInPhase < 1.2f)
//		{
//			if (!projectile[5])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[5] = true;
//			}
//		}
//		else if (timeInPhase > 1.2f && timeInPhase < 1.4f)
//		{
//			if (!projectile[6])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[6] = true;
//			}
//		}
//		else if (timeInPhase > 1.4f && timeInPhase < 1.6f)
//		{
//			if (!projectile[7])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[7] = true;
//			}
//		}
//		else if (timeInPhase > 1.6f && timeInPhase < 1.8f)
//		{
//			if (!projectile[8])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[8] = true;
//			}
//		}
//		else if (timeInPhase > 1.8f && timeInPhase < 2.0f)
//		{
//			if (!projectile[9])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[9] = true;
//				for (int i = 0; i < 9; i++)
//				{
//					projectile[i] = false;
//				}
//			}
//
//		}
//	}
//	else if (timeInPhase < 4)
//	{
//		Bras1->SetRelativeRotation(FMath::Lerp(destinationQuat, originQuat, (timeInPhase - 2) / 2));
//		if (timeInPhase < 2.2f)
//		{
//			if (!projectile[0])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[0] = true;
//				projectile[9] = false;
//
//			}
//
//		}
//		else if (timeInPhase > 2.2f && timeInPhase < 2.4f)
//		{
//			if (!projectile[1])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[1] = true;
//			}
//		}
//		else if (timeInPhase > 2.4f && timeInPhase < 2.6f)
//		{
//			if (!projectile[2])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[2] = true;
//			}
//		}
//		else if (timeInPhase > 2.6f && timeInPhase < 2.8f)
//		{
//			if (!projectile[3])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[3] = true;
//			}
//		}
//		else if (timeInPhase > 2.8f && timeInPhase < 3.0f)
//		{
//			if (!projectile[4])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[4] = true;
//			}
//		}
//		else if (timeInPhase > 3.0f && timeInPhase < 3.2f)
//		{
//			if (!projectile[5])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[5] = true;
//			}
//		}
//		else if (timeInPhase > 3.2f && timeInPhase < 3.4f)
//		{
//			if (!projectile[6])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[6] = true;
//			}
//		}
//		else if (timeInPhase > 3.4f && timeInPhase < 3.6f)
//		{
//			if (!projectile[7])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[7] = true;
//			}
//		}
//		else if (timeInPhase > 3.6f && timeInPhase < 3.8f)
//		{
//			if (!projectile[8])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[8] = true;
//			}
//		}
//		else if (timeInPhase > 3.8f && timeInPhase < 4.0f)
//		{
//			if (!projectile[9])
//			{
//				FVector dir = (ChildSocketTir->GetComponentLocation() - Bras1->GetComponentLocation());
//				dir.Normalize();
//				SpawnProjectile(ChildSocketTir->GetComponentLocation(), dir);
//				projectile[9] = true;
//			}
//		}
//
//
//
//	}
//	else
//	{
//		for (int i = 0; i < 10; i++)
//		{
//			projectile[i] = false;
//		}
//		isInPhase = false;
//		multiProjectilePhase = false;
//		timeInPhase = 0;
//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("END")));
//	}
//}

//void AS47BossDistance::JetAcidePhase(float DeltaTime)
//{
//	if (init)
//	{
//		InitArmMovement(0, 0, false, false, 0);
//		tempDestinationQuat = destinationQuat;
//		InitArmMovement(0, 0, false, false, 1);
//		init = false;
//	}
//	timeInPhase += DeltaTime;
//	if (timeInPhase < 2)
//	{
//		//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Phase1")));
//		Bras1->SetRelativeRotation(FMath::Lerp(originQuat, tempDestinationQuat, timeInPhase / 2));
//	}
//	else if (timeInPhase < 6)
//	{
//		if (!spawnJet)
//		{
//			OnJetBP();
//			/*for (USceneComponent* JetCollider : JetColliders)
//			{
//				Cast<USphereComponent>(JetCollider)->OnComponentBeginOverlap.AddUniqueDynamic(this, &AS47BossDistance::OnTriggerBeginOverlap);
//			}*/
//			spawnJet = true;
//		}
//		//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Phase2")));
//		Bras1->SetRelativeRotation(FMath::Lerp(tempDestinationQuat, destinationQuat, (timeInPhase - 2) / 4));
//	}
//	else
//	{
//		isInPhase = false;
//		jetAcidePhase = false;
//		timeInPhase = 0;
//		spawnJet = false;
//		OnJetEndBP();
//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("END")));
//		/*for (USceneComponent* JetCollider : JetColliders)
//		{
//			JetCollider->bIsActive = false;
//		}*/
//	}
//}

void AS47BossDistance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!start)
	{
		return;
	}

	if (m_player == nullptr)
	{
		return;
	}


	
	if (GetHealth() < 0 && isAlive)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Boss Die")));
		InvokeDeath(); // trigger the death animation of the boss
		isAlive = false;
	}

}

// Run at the end of the boss death animation
void AS47BossDistance::AnimationEnd(AController* _controller, AActor* _killer)
{
	AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
	if (GameState != nullptr)
	{
		GameState->InvokeBossEnd();
	}
	Destroy();
}

void AS47BossDistance::StartBoss()
{
	StartBossBP();
}

void AS47BossDistance::EndStartBoss()
{
	SetHealth(m_lifeMax);
	start = true;
	AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
	if (GameState != nullptr)
	{
		GameState->InvokeEndStartBoss();
	}
	EndStartBossBP();
}





