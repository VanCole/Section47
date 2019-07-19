// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Room.h"
#include "S47Door.h"
#include "Engine/World.h"
#include "Engine.h"
#include "S47RoomData.h"
#include "S47RoomLevel.h"
#include "Section47.h"
#include <stdexcept>

bool US47Room::occlusionCulling = true;
FVector US47Room::unit(1000, 1000, 400);

US47Room::US47Room()
{

}

void US47Room::Init(TSubclassOf<US47RoomData> _prefabClass, US47Room* _parent)
{
	prefabClass = _prefabClass;
	instance = nullptr;
	parent = _parent;
	position = FIntVector(0,0,0);
	orientation = ES47Orientation::North;

	prefab = _prefabClass.GetDefaultObject();
	if (nullptr != prefab)
	{
		for (int i = 0; i < prefab->GetNbDoor(); i++)
		{
			connections.Add(nullptr);
		}
	}
}

US47Room::~US47Room()
{
}

bool US47Room::IsConnected(int _index)
{
	if (_index < 0 || _index >= connections.Num())
		throw std::out_of_range("Index is out of range of Connection Count.");
	return connections[_index] != nullptr;
}

void US47Room::SetConnection(int _index, US47Room* _room)
{
	if (_index < 0 || _index >= connections.Num())
		throw std::out_of_range("Index is out of range of Connection Count.");
	connections[_index] = _room;
}

TWeakObjectPtr<US47Room> US47Room::GetConnection(int _index)
{
	if (_index < 0 || _index >= connections.Num())
		throw std::out_of_range("Index is out of range of Connection Count.");
	return connections[_index];
}

// return -1 if _room = null or room is not connected
int US47Room::GetConnectionIndex(US47Room& _room)
{
	int index = -1;
	if (!connections.Find(&_room, index))
	{
		return -1;
	}
	return index;
}

void US47Room::Instantiate(UWorld* world)
{
	if (instance == nullptr)
	{
		instance = US47LevelStreamingDynamic::Load(world, prefab, US47Room::unit * FVector(position), FRotator(0, -90 * (int)orientation, 0));
		UE_LOG(LogS47Level, Log, TEXT("Load room instance: %s"), nullptr != instance ? *instance->GetWorldAssetPackageName() : TEXT("Null"));
	}
	else
	{
		UE_LOG(LogS47Level, Error, TEXT("Can't instantiate an already instanciated room"));
	}
}

void US47Room::Destroy(UWorld * world)
{
	if (instance != nullptr)
	{
		UE_LOG(LogS47Level, Log, TEXT("Unload room instance: %s"), nullptr != instance ? *instance->GetWorldAssetPackageName() : TEXT("Null"));

		AS47RoomLevel* script = GetLevelScript();
		if (script != nullptr)
		{
			script->Room = nullptr;
			script->Destroy();
		}

		US47LevelStreamingDynamic::Unload(world, instance);
	}
}


AS47RoomLevel* US47Room::GetLevelScript()
{
	if (instance == nullptr || !IsValid(instance))
	{
		return nullptr;
	}
	return Cast<AS47RoomLevel>(instance->GetLevelScriptActor());
}

ES47Orientation US47Room::GetDoorWorldOrientation(int doorIndex)
{
	if (doorIndex < 0 || doorIndex >= prefab->doors.Num())
		throw std::out_of_range("Index is out of range of Door Count.");
	return Add(prefab->doors[doorIndex].orientation, orientation);
}

FIntVector US47Room::GetDoorWorldPosition(int doorIndex)
{
	if (doorIndex < 0 || doorIndex >= prefab->doors.Num())
		throw std::out_of_range("Index is out of range of Door Count.");
	return RoomToWorld(prefab->doors[doorIndex].position);
}

FIntVector US47Room::WorldToRoom(FIntVector worldPos)
{
	return Rotate(worldPos - position, Sub(ES47Orientation::North, orientation));
}

FIntVector US47Room::RoomToWorld(FIntVector roomPos)
{
	return Rotate(roomPos, orientation) + position;
}

void US47Room::SetRotationFromDoor(int doorIndex, ES47Orientation worldRot)
{
	if (doorIndex < 0 || doorIndex >= prefab->doors.Num())
		throw std::out_of_range("Index is out of range of Door Count.");
	orientation = Add(Sub(worldRot, prefab->doors[doorIndex].orientation), ES47Orientation::South);
}

void US47Room::SetPositionFromDoor(int doorIndex, FIntVector worldPos)
{
	if (doorIndex < 0 || doorIndex >= prefab->doors.Num())
		throw std::out_of_range("Index is out of range of Door Count.");
	position = worldPos - RoomToWorld(prefab->doors[doorIndex].position);
}

void US47Room::SetPositionAndRotationFromDoor(int doorIndex, FIntVector worldPos, ES47Orientation worldRot)
{
	if (doorIndex < 0 || doorIndex >= prefab->doors.Num())
		throw std::out_of_range("Index is out of range of Door Count.");


	orientation = Add(Sub(worldRot, prefab->doors[doorIndex].orientation), ES47Orientation::South);
	position = worldPos - RoomToWorld(prefab->doors[doorIndex].position);
}


bool US47Room::IsOccupied(FIntVector cell)
{
	return cell.X >= position.X && cell.X < position.X + prefab->size.X
		&& cell.Y >= position.Y && cell.Y < position.Y + prefab->size.Y
		&& cell.Z >= position.Z && cell.Z < position.Z + prefab->size.Z;
}

void US47Room::ConnectTo(int _thisDoorIndex, US47Room & _otherRoom, int _otherDoorIndex)
{
	// Connect room between them
	_otherRoom.SetConnection(_otherDoorIndex, this);
	this->SetConnection(_thisDoorIndex, &_otherRoom);

	// Set position and rotation of new room in world
	ES47Orientation otherDoorOrientation = _otherRoom.GetDoorWorldOrientation(_otherDoorIndex);
	this->SetRotationFromDoor(_thisDoorIndex, otherDoorOrientation);
	this->SetPositionFromDoor(_thisDoorIndex, _otherRoom.GetDoorWorldPosition(_otherDoorIndex) + US47Room::GetDirection(otherDoorOrientation));
}

FIntVector Max(const FIntVector& _a, const FIntVector& _b)
{
	return FIntVector(FMath::Max(_a.X, _b.X), FMath::Max(_a.Y, _b.Y), FMath::Max(_a.Z, _b.Z));
}

FIntVector Min(const FIntVector& _a, const FIntVector& _b)
{
	return FIntVector(FMath::Min(_a.X, _b.X), FMath::Min(_a.Y, _b.Y), FMath::Min(_a.Z, _b.Z));
}

// AABB Overlapping
bool US47Room::Overlap(US47Room& A, US47Room& B)
{
	FIntVector A_firstPoint = A.position;
	FIntVector B_firstPoint = B.position;
	FIntVector A_secondPoint = A.RoomToWorld(A.prefab->size - FIntVector(1,1,1));
	FIntVector B_secondPoint = B.RoomToWorld(B.prefab->size - FIntVector(1,1,1));

	FIntVector A_min = Min(A_firstPoint, A_secondPoint);
	FIntVector A_max = Max(A_firstPoint, A_secondPoint);
	FIntVector B_min = Min(B_firstPoint, B_secondPoint);
	FIntVector B_max = Max(B_firstPoint, B_secondPoint);

	if (A_min.X > B_max.X) return false;
	if (A_max.X < B_min.X) return false;
	if (A_min.Y > B_max.Y) return false;
	if (A_max.Y < B_min.Y) return false;
	if (A_min.Z > B_max.Z) return false;
	if (A_max.Z < B_min.Z) return false;
	return true;
}

bool US47Room::Overlap(US47Room & _room, TArray<US47Room*>& _roomList)
{
	bool overlap = false;
	for (int i = 0; i < _roomList.Num() && !overlap; i++)
	{
		if (Overlap(_room, *_roomList[i]))
		{
			overlap = true;
		}
	}
	return overlap;
}

ES47Orientation US47Room::Add(ES47Orientation A, ES47Orientation B)
{
	int8 o = (int8)A + (int8)B;
	while (o > 2) o -= 4;
	while (o <= -2) o += 4;
	return (ES47Orientation)o;
}

ES47Orientation US47Room::Sub(ES47Orientation A, ES47Orientation B)
{
	int8 o = (int8)A - (int8)B;
	while (o > 2) o -= 4;
	while (o <= -2) o += 4;
	return (ES47Orientation)o;
}

FIntVector US47Room::GetDirection(ES47Orientation O)
{
	FIntVector dir = FIntVector(0,0,0);
	switch (O)
	{
	case ES47Orientation::North:
		dir.X = 1;
		break;
	case ES47Orientation::East:
		dir.Y = 1;
		break;
	case ES47Orientation::West:
		dir.Y = -1;
		break;
	case ES47Orientation::South:
		dir.X = -1;
		break;
	}
	return dir;
}

FIntVector US47Room::Rotate(FIntVector pos, ES47Orientation rot)
{
	FIntVector newPos = pos;
	switch (rot)
	{
	case ES47Orientation::North:
		newPos = pos;
		break;
	case ES47Orientation::West:
		newPos.Y = -pos.X;
		newPos.X = pos.Y;
		break;
	case ES47Orientation::East:
		newPos.Y = pos.X;
		newPos.X = -pos.Y;
		break;
	case ES47Orientation::South:
		newPos.Y = -pos.Y;
		newPos.X = -pos.X;
		break;
	}
	return newPos;
}
