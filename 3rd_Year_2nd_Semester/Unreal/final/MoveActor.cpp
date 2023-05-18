// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveActor.h"

// Sets default values
AMoveActor::AMoveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMoveActor::BeginPlay()
{
	Super::BeginPlay();
	randfloat = FMath::RandRange(minRandnum, maxRandnum);
}

// Called every frame
void AMoveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float multiple;

	if (IsRandom)
	{
		multiple = randfloat;
	}
	else
	{
		multiple = distance;
	}

	if (MoveDirection==0)
	{
		SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f,
			FMath::Sin(GetWorld()->GetTimeSeconds() * multiple) * multipleSin));
	}
	else if (MoveDirection == 1)
	{
		SetActorLocation(GetActorLocation() + FVector(FMath::Sin(GetWorld()->GetTimeSeconds() * multiple) * multipleSin
			, 0.0f,0.0f));
	}
	else
	{
		SetActorLocation(GetActorLocation() + FVector(0.0f, FMath::Sin(GetWorld()->GetTimeSeconds() * multiple) * multipleSin, 0.0f));
	}
	

}

