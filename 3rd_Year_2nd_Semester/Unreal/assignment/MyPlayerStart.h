// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "MyPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class A1_API AMyPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:

	FVector SpawnPoint;

	virtual void BeginPlay() override;
	void SetPlayerStartLocation(FVector p_Vector);
};
