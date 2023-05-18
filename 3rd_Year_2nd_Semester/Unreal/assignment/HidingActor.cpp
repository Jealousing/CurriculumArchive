// Fill out your copyright notice in the Description page of Project Settings.


#include "HidingActor.h"

// Sets default values
AHidingActor::AHidingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("MyMesh");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		CubeMesh = CubeMeshAsset.Object;
		Mesh->SetStaticMesh(CubeMesh);
	}

	HideTime = 1;
	isDestroy = false;
}

// Called when the game starts or when spawned
void AHidingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHidingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AHidingActor::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	isDestroy = true;
	GetWorldTimerManager().SetTimer(countdownTimerHandle, this, &AHidingActor::advanceTimer1, 0.5f, true);
}

void AHidingActor::advanceTimer1()
{
	--HideTime;
	if (HideTime < 1)
	{
		HideTime = 1;
		GetWorldTimerManager().ClearTimer(countdownTimerHandle);
		ActorHideFalse();
	}
}

void AHidingActor::advanceTimer2()
{
	--HideTime;
	if (HideTime < 1)
	{
		HideTime = 1;
		GetWorldTimerManager().ClearTimer(countdownTimerHandle);
		ActorHideTrue();
	}
}


void AHidingActor::ActorHideFalse()
{
	//Destroy();
	Mesh->SetStaticMesh(NULL);
	//SetActorHiddenInGame(false);
	SetActorEnableCollision(false);

	GetWorldTimerManager().SetTimer(countdownTimerHandle, this, &AHidingActor::advanceTimer2, 3.0f, true);
}
void AHidingActor::ActorHideTrue()
{
	//Destroy();
	Mesh->SetStaticMesh(CubeMesh);
	//SetActorHiddenInGame(true);
	SetActorEnableCollision(true);
}