// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FStartDelegate);
DECLARE_MULTICAST_DELEGATE(FEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class A1_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UMyAnimInstance();
	virtual void NativeUpdateAnimation(float ds) override;
	UFUNCTION()
	void PlayAttackMontage();
	UFUNCTION()
	void JumpToAttackMontageSection(int32 _Section);

	void SetDeadAnim() { IsDead = true; }

	FStartDelegate OnStartHandler;
	FEndDelegate OnEndHandler;
	FOnAttackHitCheckDelegate OnAttackHitCheck;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		float CurrentPawnSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		bool IsInAir;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		bool IsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		int32 clickCount;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontage;

	FName GetAttackMontageSectionName(int32 Section);

	UFUNCTION()
	void AnimNotify_StartAttack();
	UFUNCTION()
	void AnimNotify_EndAttack();
	UFUNCTION()
	void AnimNotify_AttackHitCheck();
};
