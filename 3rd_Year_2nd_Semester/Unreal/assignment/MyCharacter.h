// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "A1.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class A1_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	enum class EControlMode
	{
		GTA,
		DIABLO
	};

	void SetControlMode(EControlMode ControlMode);

	EControlMode CurrentControlMode = EControlMode::GTA;

	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmRotationSpeed = 10.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		USpringArmComponent* springArm;
	UPROPERTY(VisibleAnywhere, Category = Camera)
		UCameraComponent* camera;
	void UpDown(float value);
	void LeftRight(float value);
	void LookUp(float value);
	void Turn(float value);
	UFUNCTION()
		void StartJump();

	UFUNCTION()
		void StopJump();
	//void CustomJump();
	//bool IsInAir;
	void Attack();

	void AttackCheck();

	void GetRespawn();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		bool IsAttacking;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
		void OnMontageEndedHandler(UAnimMontage* Montage, bool bInterruped);

	void AttackStartComboState();
	void AttackEndComboState();

	void OnEndHandler();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;


private:
	FVector direction = FVector::ZeroVector;
	void ViewChange();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		bool CanNextCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		bool IsComboInputOn;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		int32 CurrentCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		int32 MaxCombo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		float AttackRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		float AttackRadius;

	UPROPERTY()
	class UMyAnimInstance* AnimInstance;
};

