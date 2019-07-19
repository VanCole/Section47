// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "S47Types.generated.h"

// Allow to know if running on server or client
#define GET_NETMODE_TEXT() (GetNetMode() == ENetMode::NM_ListenServer ? TEXT("Server") : TEXT("Client"))

UENUM(BlueprintType)
enum ES47Weapon
{
	Weapon_None				UMETA(DisplayName = "None"),
	Weapon_Pistol			UMETA(DisplayName = "Pistol"),
	Weapon_Shotgun			UMETA(DisplayName = "Shotgun"),
	Weapon_Rifle			UMETA(DisplayName = "Rifle"),
	Weapon_Minigun			UMETA(DisplayName = "Minigun"),
	Weapon_Lasergun			UMETA(DisplayName = "Lasergun"),
	Weapon_GrenadeLauncher	UMETA(DisplayName = "GrenadeLauncher"),
	Weapon_RocketLauncher	UMETA(DisplayName = "RocketLauncher"),
	Weapon_MAX				UMETA(Hidden),
};

UENUM(BlueprintType)
enum ES47WeaponIndex
{
	Weapon_1			UMETA(DisplayName = "Weapon_1"),
	Weapon_2			UMETA(DisplayName = "Weapon_2"),
	Weapon_3			UMETA(DisplayName = "Weapon_3"),
	WeaponIndex_Max 	UMETA(Hidden)
};

UENUM(BlueprintType)
enum ES47DiedFrom
{
	DiedFrom_Shot			UMETA(DisplayName = "Shot"),
	DiedFrom_Explosion		UMETA(DisplayName = "Explosion"),
	DiedFrom_Other			UMETA(DisplayName = "Other"),
	DiedFrom_Died_FromMax	UMETA(Hidden),
};


UENUM(BlueprintType)
enum ES47CustomMove
{
	CustomMove_None			UMETA(DisplayName = "None"),
	CustomMove_Dash			UMETA(DisplayName = "Dash"),
	CustomMove_SuperJump	UMETA(DisplayName = "SuperJump"),
	CustomMove_Hover		UMETA(DisplayName = "Hover"),
	CustomMove_LedgeClimbing UMETA(DisplayName = "LedgeClimbing"),
	CustomMove_Max			UMETA(Hidden),

};

UENUM(BlueprintType)
enum ES47Spawner
{
	Spawner_None			UMETA(DisplayName = "None"),
	Spawner_Weapon			UMETA(DisplayName = "Weapon"),
	Spawner_Ammo			UMETA(DisplayName = "Ammo"),
	Spawner_Enemy			UMETA(DisplayName = "Enemy"),
	Spawner_Bonus			UMETA(DisplayName = "Bonus"),
	Spawner_MAX				UMETA(Hidden),
};


USTRUCT(BlueprintType)
struct FS47SpawnEnemy
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, Category = "S47SpawnEnemy")
		TSubclassOf<class AS47Enemy> EnemyClass;

	UPROPERTY(EditAnywhere, Category = "S47SpawnEnemy")
		int32 weight;
};

UENUM(BlueprintType)
enum ES47BoostEnum
{
	Boost_None			UMETA(DisplayName = "None"),
	Boost_SpeedUp		UMETA(DisplayName = "Boost_SpeedUp"),
	Boost_DoubleDamage	UMETA(DisplayName = "Boost_DoubleDamage"),
	Boost_FullArmor		UMETA(DisplayName = "Boost_FullArmor"),
	Boost_Invincibility	UMETA(DisplayName = "Boost_Invincibility"),
	Boost_RefillAmmo	UMETA(DisplayName = "Boost_RefillAmmo"),
	Boost_MAX			UMETA(Hidden),
};

UENUM(BlueprintType)
enum CAMERASHAKE
{
	Shotgun,
	Minigun,
	GrenadeLauncherLight,
	GrenadeLauncherHard,
	Explosion
};

ETeamAttitude::Type S47TeamAttitudeSolver(FGenericTeamId A, FGenericTeamId B);


/*
 *	=============   Types for Procedural Level   ===============
 */

UENUM(BlueprintType)
enum class ES47Orientation : uint8
{
	North = 0 				UMETA(DisplayName = "North"), // rotation = 0 (world forward)
	East = 255 				UMETA(DisplayName = "East"),  // rotation = -90 (world right)
	West = 1 				UMETA(DisplayName = "West"),  // rotation = 90 (world left)
	South = 2 				UMETA(DisplayName = "South"), // rotation = 180 (world backward)
	NbOrientation = 4 		UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ERoomType : uint8
{
	Normal					UMETA(DisplayName = "Normal"),
	Spawn					UMETA(DisplayName = "Spawn"),
	Exit					UMETA(DisplayName = "Exit"),
	Boss					UMETA(DisplayName = "Boss"),
	Heal					UMETA(DisplayName = "Heal"),
	SAS						UMETA(DisplayName = "SAS"),
	NbRoomType				UMETA(Hidden)
};

USTRUCT()
struct FS47DoorDef {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FIntVector position;
	UPROPERTY(EditAnywhere)
	ES47Orientation orientation;
};

UENUM(BlueprintType)
enum class ES47DEPENDANCEMODE : uint8
{
	NBSPAWN						UMETA(DisplayName = "NBSpawn"),
	TIME						UMETA(DisplayName = "Time"),
	ALLMOBDEATHFROMSPAWNER		UMETA(DisplayName = "AllMobDeathFromSpawner"),
	ALLMOBDEATHFROMACTIVE		UMETA(DisplayName = "AllMobDeathFromActive"),
	ALLMOBDEATHFROMDEPENDANCE	UMETA(DisplayName = "AllMobDeathFromDependance"),
	Nbdependance				UMETA(Hidden)
};

USTRUCT()
struct FS47DependanceSpawner
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere)
	ES47DEPENDANCEMODE dependanceMode;
	/** Only if Dependance Mode is Time */
	UPROPERTY(EditAnywhere)
	float timeBetweenEachSpawner;
	/** Only if Dependance Mode is AllMobDeath*/
	UPROPERTY(EditAnywhere)
		float percentFromMobDeath = 100;
	UPROPERTY(EditAnywhere)
	class AS47SpawnerEnemy* spawnerEnemy;
};