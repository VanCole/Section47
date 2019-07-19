// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Weapon.h"
#include "Components/S47ProjectileComponent.h"
#include "Weapon/WeaponRaycast/S47Rifle.h"
#include "Player/S47PlayerCharacter.h"
#include "AI/S47AICharacter.h"
#include "Components/S47MovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "UnrealNetwork.h"

// Sets default values
AS47Weapon::AS47Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));

	RootComponent = SceneComponent;


	ProjectileComponent = CreateDefaultSubobject<US47ProjectileComponent>(TEXT("ProjComponent"));
	ProjectileComponent->SetUpdatedComponent(MeshComponent);
	ProjectileComponent->SetIsReplicated(true);

	MeshComponent->bCastDynamicShadow = false;
	MeshComponent->CastShadow = false;
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetIsReplicated(true);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(MeshComponent);


	bReplicates = true;

	SemiAuto = false;

	weightWeapon = 10;
}

// Called when the game starts or when spawned
void AS47Weapon::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentAmmo = TotalAmmo;
	if (nullptr != MeshComponent)
	{
		MeshComponent->OnComponentHit.AddDynamic(this, &AS47Weapon::OnHitCollision);
		MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

// Called every frame
void AS47Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentAmmo < 0)
		CurrentAmmo = 0;
	if (this->IsA<AS47Rifle>())
	{
		AS47Rifle* Rifle = Cast<AS47Rifle>(this);
		if (Rifle->currentAmmoBis < 0)
			Rifle->currentAmmoBis = 0;
	}
}

void AS47Weapon::Attack()
{
	if (!ConsumeAmmoDifferently)
	{
		ConsumeAmmo();
		OnAttack();
	}
}

void AS47Weapon::StopAttack()
{
	
}

void AS47Weapon::AttackBis()
{
}

void AS47Weapon::StopAttackBis()
{
}

void AS47Weapon::Reload()
{
	float trueTotalAmmo = TotalAmmo;
	AS47PlayerCharacter* PlayerOwnerCharacter = ((AS47PlayerCharacter*)OwnerCharacter);
	AS47PlayerState* PlayerState = PlayerOwnerCharacter->GetPlayerState<AS47PlayerState>();
	if (PlayerState != nullptr && PlayerState->levelAmmo > 0)
	{
		trueTotalAmmo *= PlayerState->levelAmmo * 1.1f;
	}
	CurrentAmmo = trueTotalAmmo;
}

void AS47Weapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS47Weapon, MeshComponent);
	DOREPLIFETIME(AS47Weapon, ProjectileComponent);
	DOREPLIFETIME(AS47Weapon, TotalAmmo);
	DOREPLIFETIME(AS47Weapon, CurrentAmmo);
}

void AS47Weapon::InitProjectile(FVector Location, FVector ForwardWeapon, FVector ForwardVelocity)
{
	if (nullptr != ProjectileComponent)
	{
		ProjectileComponent->Activate();
		ProjectileComponent->InitialSpeed = 1000.0f - Weight;
		ProjectileComponent->bShouldBounce = true;
		ProjectileComponent->Bounciness = 0.1f;
		ProjectileComponent->Friction = 0.8f;
		ProjectileComponent->bRotationFollowsVelocity = true;
		SetLifeSpan(5.0f);

		if (nullptr != MeshComponent)
		{
			MeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepWorldTransform);
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AS47PlayerCharacter::StaticClass(), FoundActors);

			for (auto player : FoundActors)
			{
				MeshComponent->IgnoreActorWhenMoving(Cast<AS47PlayerCharacter>(player), true);
			}

			
			MeshComponent->SetCollisionProfileName(TEXT("Projectile"));
		}
		ProjectileComponent->Velocity =  ForwardWeapon * ProjectileComponent->InitialSpeed + ForwardVelocity;
	}
}

/*void AS47Weapon::ConsumeAmmo()
{
	CurrentAmmo--;
}*/

void AS47Weapon::OnAttack()
{
	//OnAttackCallBP();
	if (HasAuthority())
	{
		OnAttackBP_ToClient(PosHit);
	}
	else
	{
		OnAttackBP_ToServ(PosHit);
		OnAttackBP(PosHit);
	}
	PosHit.Empty();
}

void AS47Weapon::OnAttackBis()
{
	if (HasAuthority())
	{
		OnAttackBisBP_ToClient(PosHit);
	}
	else
	{
		OnAttackBisBP_ToServ(PosHit);
		OnAttackBisBP(PosHit);
	}
	PosHit.Empty();
}

void AS47Weapon::OnAttackEmpty()
{
	if (HasAuthority())
	{
		OnAttackEmptyBP_ToClient();
	}
	else
	{
		OnAttackEmptyBP_ToServ();
		OnAttackEmptyBP();
	}
}

int32 AS47Weapon::GetWeight()
{
	return weightWeapon;
}

void AS47Weapon::OnAttackBP_ToServ_Implementation(const TArray<FVector>& Poshit)
{
	OnAttackBP(Poshit);
}

bool AS47Weapon::OnAttackBP_ToServ_Validate(const TArray<FVector>& Poshit)
{
	return true;
}

void AS47Weapon::OnAttackBP_ToClient_Implementation(const TArray<FVector>& Poshit)
{
	OnAttackBP(Poshit);
}

void AS47Weapon::OnAttackEmptyBP_ToServ_Implementation()
{
	OnAttackEmptyBP();
}

bool AS47Weapon::OnAttackEmptyBP_ToServ_Validate()
{
	return true;
}

void AS47Weapon::OnAttackEmptyBP_ToClient_Implementation()
{
	OnAttackEmptyBP();
}


void AS47Weapon::OnAttackBisBP_ToServ_Implementation(const TArray<FVector>& PosHit)
{
	OnAttackBisBP(PosHit);
}

bool AS47Weapon::OnAttackBisBP_ToServ_Validate(const TArray<FVector>& PosHit)
{
	return true;
}

void AS47Weapon::OnAttackBisBP_ToClient_Implementation(const TArray<FVector>& PosHit)
{
	OnAttackBisBP(PosHit);
}

void AS47Weapon::HitEnemy_ToServ_Implementation(AS47AICharacter* Enemy, FVector _posHit, bool byLazer, FLinearColor ColorFX)
{
	Enemy->OnHitBP(_posHit, byLazer, ColorFX);
}

bool AS47Weapon::HitEnemy_ToServ_Validate(AS47AICharacter* Enemy, FVector _posHit, bool byLazer, FLinearColor ColorFX)
{
	return true;
}

void AS47Weapon::HitEnemy_ToClient_Implementation(AS47AICharacter* Enemy, FVector _posHit, bool byLazer, FLinearColor ColorFX)
{
	Enemy->OnHitBP(_posHit, byLazer, ColorFX);
}

void AS47Weapon::SaveLastShotOnEnemy_ToServ_Implementation(AS47AICharacter* Enemy, FVector _force_Saved, FVector _poshit_Saved, FName _boneTouched_Saved)
{
	Enemy->SaveLastShot(_force_Saved, _poshit_Saved, _boneTouched_Saved);
}

bool AS47Weapon::SaveLastShotOnEnemy_ToServ_Validate(AS47AICharacter* Enemy, FVector _force_Saved, FVector _poshit_Saved, FName _boneTouched_Saved)
{
	return true;
}

void AS47Weapon::SaveLastShotOnEnemy_ToClient_Implementation(AS47AICharacter* Enemy, FVector _force_Saved, FVector _poshit_Saved, FName _boneTouched_Saved)
{
	Enemy->SaveLastShot(_force_Saved, _poshit_Saved, _boneTouched_Saved);
}

void AS47Weapon::OnHitCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Touch"));
	ProjectileComponent->bRotationFollowsVelocity = false;

	if (OtherActor->IsA<AS47AICharacter>())
	{

	}

}

void AS47Weapon::ConsumeAmmo_Implementation()
{
	CurrentAmmo--;
}

bool AS47Weapon::ConsumeAmmo_Validate()
{
	return true;
}