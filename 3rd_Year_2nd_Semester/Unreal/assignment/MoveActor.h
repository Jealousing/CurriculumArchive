// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MoveActor.generated.h"

UCLASS()
class A1_API AMoveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoveActor();

	float randfloat;

	UPROPERTY(EditAnywhere)
		float minRandnum;
	UPROPERTY(EditAnywhere)
		float maxRandnum;
	UPROPERTY(EditAnywhere)
		float distance;
	UPROPERTY(EditAnywhere)
		float multipleSin;
	UPROPERTY(EditAnywhere)
		int MoveDirection;
	UPROPERTY(EditAnywhere)
		bool IsRandom;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
