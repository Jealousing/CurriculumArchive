// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTriggerBox.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "MyPlayerStart.h"

void AMyTriggerBox::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &AMyTriggerBox::OnOverlapBegin);
	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, -1, 0, 5);

}


void AMyTriggerBox::OnOverlapBegin(AActor* p_Overlap, AActor* p_Other)
{
	AActor* player = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	if (p_Other == player)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "SpawnPoint", actors);
		AMyPlayerStart* SpawnPoint = Cast<AMyPlayerStart>(actors[0]);
		if (SpawnPoint)
		{
			SpawnPoint[0].SetPlayerStartLocation(GetActorLocation());
		}

	}
}