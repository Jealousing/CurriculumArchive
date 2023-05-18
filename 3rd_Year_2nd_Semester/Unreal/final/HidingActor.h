// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "A1.h"
#include "GameFramework/Actor.h"
#include "HidingActor.generated.h"

UCLASS()
class A1_API AHidingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHidingActor();

	bool isDestroy;
	int HideTime;
	FTimerHandle countdownTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);
	void advanceTimer1();
	void advanceTimer2();
	void ActorHideFalse();
	void ActorHideTrue();
	UStaticMesh* CubeMesh;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;
	
};
