// Fill out your copyright notice in the Description page of Project Settings.

#include "S47RocketLauncher.h"
#include "Projectile/S47Rocket.h"
#include "Player/S47PlayerCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AS47RocketLauncher::AS47RocketLauncher()
{
	weightWeapon = 20;
}

void AS47RocketLauncher::AttackBis()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AS47Rocket::StaticClass(), FoundActors);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("kaBOOM")));
	if (Cast<AS47PlayerCharacter>(GetOwner()) != nullptr)
	{
		for (auto rocket : FoundActors)
		{
			if (nullptr != Cast<AS47Rocket>(rocket))
			{
				if (Cast<AS47PlayerCharacter>(GetOwner())->Role < ROLE_Authority)
				{
					Cast<AS47Rocket>(rocket)->ServerExplodeRPC(maxDamage);
				}
				else
				{
					Cast<AS47Rocket>(rocket)->Explode(maxDamage);
				}
			}
		}
	}

}
