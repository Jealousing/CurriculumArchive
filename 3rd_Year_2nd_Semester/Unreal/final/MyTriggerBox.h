// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "MyTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class A1_API AMyTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

public:

	UFUNCTION()
		void OnOverlapBegin(AActor* p_Overlap, AActor* p_Other);
};
