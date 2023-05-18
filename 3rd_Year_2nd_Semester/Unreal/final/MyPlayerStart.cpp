// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerStart.h"

void AMyPlayerStart::SetPlayerStartLocation(FVector p_Vector)
{
	SpawnPoint = p_Vector;
	//SetActorLocation(p_Vector);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, (TEXT("SavePoint")));
}

void AMyPlayerStart::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add("SpawnPoint");
	SpawnPoint = GetActorLocation();
}