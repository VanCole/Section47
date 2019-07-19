// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Projectile.h"
#include "Weapon/S47Weapon.h"
#include "Components/S47ProjectileComponent.h"
#include "AI/S47AICharacter.h"
#include "Components/SphereComponent.h"
#include "UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AS47Projectile::AS47Projectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AOETrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	MeshComponent->SetIsReplicated(true);

	RootComponent = MeshComponent;

	ProjectileComponent = CreateDefaultSubobject<US47ProjectileComponent>(TEXT("ProjComponent"));
	ProjectileComponent->SetIsReplicated(true);

	S47LifeSpan = 5.f;

	if (nullptr != ProjectileComponent && nullptr != RootComponent)
		ProjectileComponent->SetUpdatedComponent(RootComponent);
	if (nullptr != AOETrigger && nullptr != MeshComponent)
		AOETrigger->SetupAttachment(MeshComponent);

	bReplicates = true;
	bReplicateMovement = true;

	ExplosionForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("ExplosionForceComponent"));

	ExplosionForceComponent->Radius = 0;
	ExplosionForceComponent->ForceStrength = 0;
	ExplosionForceComponent->ImpulseStrength = 0;

}

// Called when the game starts or when spawned
void AS47Projectile::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != MeshComponent)
		MeshComponent->OnComponentHit.AddDynamic(this, &AS47Projectile::OnHitCollision);

	if (S47LifeSpan != 0.f)
		GetWorld()->GetTimerManager().SetTimer(ProjectileTimerHandle, this, &AS47Projectile::OnLifeEnded, S47LifeSpan, false);

}

void AS47Projectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void AS47Projectile::Explode(float forceExplosion)
{
	//ApplyExplosionForce();
	if (dealedDMG)
	{
		return;
	}
	TArray<AActor*> FoundActors;
	TArray<UPrimitiveComponent*> FoundComponents;
	AOETrigger->GetOverlappingActors(FoundActors);
	AOETrigger->GetOverlappingComponents(FoundComponents);
	dealedDMG = true;
	for (auto enemys : FoundActors)
	{
		AS47Character* enemy = Cast<AS47Character>(enemys);
		if (IsValid(enemy))
		{
			if (GetGenericTeamId() != enemy->GetGenericTeamId())
			{
				float range = FVector::Dist(enemy->GetActorLocation(), GetActorLocation());
				float dmg = (((range - 0) * (0 - Damage)) / (AOETrigger->GetScaledSphereRadius() * 10 - 0)) + Damage;

				enemy->GetDamage(dmg, GetOwner());

				if (Cast<AS47AICharacter>(enemys))
				{
					FVector dir = enemys->GetActorLocation() - GetActorLocation();
					dir.Normalize();
					Cast<AS47AICharacter>(enemys)->SaveLastShot_ToClient(dir * Damage * 1000.0f, enemys->GetActorLocation(), "None");
					//Cast<AS47AICharacter>(enemys)
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), dmg));
				}
			}
		}
	}

	for (auto enemys : FoundComponents)
	{
		if (enemys->IsSimulatingPhysics())
		{
			FVector dir = enemys->GetComponentLocation() - GetActorLocation();
			dir.Normalize();
			
			//enemys->AddImpulseAtLocation(dir * Damage * 1000.0f, enemys->GetComponentLocation());
			AddImpulse_ToClient(enemys, dir * Damage * 1000.0f, enemys->GetComponentLocation());

			//AddImpulse_ToClient(enemys, dir * Damage * 1000.0f, enemys->GetComponentLocation());
			//Cast<USkeletalMeshComponent>(ResultHit.GetComponent())->BreakConstraint(dir, ResultHit.ImpactPoint, ResultHit.BoneName);
		}
	}

	OnLifeEndedBP();
	//Destroy();
}

//Can be usefull but not used atm
void AS47Projectile::ApplyExplosionForce()
{
	//DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionForceComponent->Radius, 50, FColor::Cyan, true);

	ExplosionForceComponent->RegisterComponent();
	ExplosionForceComponent->SetWorldLocation(this->GetActorLocation());
	ExplosionForceComponent->SetWorldRotation(this->GetActorLocation().Rotation());
	ExplosionForceComponent->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform/*, NAME_None, EAttachLocation::KeepWorldPosition*/);
	ExplosionForceComponent->FireImpulse();
}

void AS47Projectile::AddImpulse_ToClient_Implementation(UPrimitiveComponent* _Enemy, FVector _Impulse, FVector _Location)
{
	_Enemy->AddImpulseAtLocation(_Impulse , _Location);
}

void AS47Projectile::ServerExplodeRPC_Implementation(float _forceExplosion)
{
	Explode(_forceExplosion);
}

bool AS47Projectile::ServerExplodeRPC_Validate(float _forceExplosion)
{
	return true;
}

// Called every frame
void AS47Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AS47Projectile::InitProjectile(FVector Location, FVector ForwardWeapon, FGenericTeamId TeamId)
{
	if (nullptr != ProjectileComponent)
		ProjectileComponent->Velocity = ForwardWeapon * ProjectileComponent->InitialSpeed;

	SetGenericTeamId(TeamId);
}

void AS47Projectile::OnHitCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if (IsValidActorForCollision(OtherActor))
	{
		if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

			Destroy();
		}
	}

}

bool AS47Projectile::IsValidActorForCollision(AActor* _OtherActor)
{
	if (nullptr != _OtherActor && !_OtherActor->IsA<AS47Projectile>() && !_OtherActor->IsA<AS47Weapon>()/* && GetTeamAttitudeTowards(*_OtherActor) == ETeamAttitude::Hostile*/)
		return true;

	return false;
}

void AS47Projectile::OnLifeEnded()
{
	if (nullptr != ProjectileComponent)
	{
		ProjectileComponent->StopMovementImmediately();
		ProjectileComponent->SetUpdatedComponent(nullptr);
	}

	SetLifeSpan(3.f);
	GetWorld()->GetTimerManager().ClearTimer(ProjectileTimerHandle);
	OnLifeEndedBP();
}

void AS47Projectile::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (S47TeamID != NewTeamID)
	{
		S47TeamID = NewTeamID;
	}
}

void AS47Projectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS47Projectile, ProjectileComponent);
	DOREPLIFETIME(AS47Projectile, MeshComponent);
}
