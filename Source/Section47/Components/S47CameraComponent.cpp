// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/S47CameraComponent.h"
#include "Kismet/GameplayStatics.h"

void US47CameraComponent::CameraShake(CAMERASHAKE _index )
{
	UGameplayStatics::PlayWorldCameraShake(this, cameraShake[int(_index)], GetComponentLocation(), 0, 100);
}