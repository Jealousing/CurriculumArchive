// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = AMyCharacter::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
}

void AMyGameModeBase::StartPlay()
{
    Super::StartPlay();

    GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Actor Spawning"));
    

    /*FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	FRotator rotator;
    FVector spawnLocation;
    for (int i = 0; i < 4; i++)
    {
        spawnLocation.X = 300;
        spawnLocation.Y = (i*130);
        spawnLocation.Z = 110;

        GetWorld()->SpawnActor<AMyCharacter>(AMyCharacter::StaticClass(), spawnLocation, rotator, SpawnParams);

    }*/

    
}