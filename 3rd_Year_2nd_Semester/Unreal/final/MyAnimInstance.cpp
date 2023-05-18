// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include <A1/MyCharacter.h>

UMyAnimInstance::UMyAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Animation/Attack_Montage.Attack_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		UE_LOG(LogTemp, Log, TEXT("Log Message1"));
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float ds)
{
	Super::NativeUpdateAnimation(ds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

		auto Character = Cast<AMyCharacter>(Pawn);
		//문제1번 키입력은 AMyCharacter::SetupPlayerInputComponent 에서 받음
		IsInAir = Character->GetMovementComponent()->IsFalling();
		//IsAttacking = Character->IsAttacking;

		/*if (!Montage_IsPlaying(AttackMontage))
			Character->IsAttacking = false;*/
	}
}

void UMyAnimInstance::PlayAttackMontage()
{
	//if(!Montage_IsPlaying(AttackMontage))
	Montage_Play(AttackMontage, 1.0f);
	/*clickCount++;
	if (clickCount >= 5)
		clickCount = 1;*/
	

}

void UMyAnimInstance::JumpToAttackMontageSection(int32 _Section)
{
	Montage_JumpToSection(GetAttackMontageSectionName(_Section),AttackMontage);
}


FName UMyAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void UMyAnimInstance::AnimNotify_StartAttack()
{
	OnStartHandler.Broadcast();
}

void UMyAnimInstance::AnimNotify_EndAttack()
{
	OnEndHandler.Broadcast();
}

void UMyAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}
