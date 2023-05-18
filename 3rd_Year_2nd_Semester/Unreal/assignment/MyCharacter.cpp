// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "DrawDebugHelpers.h"
#include <A1/MyAnimInstance.h>
#include "MyPlayerStart.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	springArm->SetupAttachment(GetCapsuleComponent());
	camera->SetupAttachment(springArm);
		
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

	if(SK_CARDBOARD.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);

	static ConstructorHelpers::FClassFinder<UAnimInstance> BP_ANIM
	(TEXT("/Game/Animation/BP_Anim.BP_Anim_C"));

	if (BP_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(BP_ANIM.Class);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f,-90.0f,0.0f));


	/*springArm->bUsePawnControlRotation = false;
	springArm->TargetArmLength = 700.0f;
	springArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	springArm->bInheritPitch = false;
	springArm->bInheritRoll = false;
	springArm->bInheritYaw = false;
	springArm->bDoCollisionTest = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);*/


	IsAttacking = false;
	//IsInAir = false;

	SetControlMode(EControlMode::GTA);


	MaxCombo = 4;
	AttackEndComboState();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MyCharacter"));
	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	// 캐릭터 생성자에 추가하면 되는 코드.
	GetCharacterMovement()->JumpZVelocity = 200.0f; 
	JumpMaxCount = 2; 
	JumpMaxHoldTime = 1.5f; 
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

//문제 2번 
void AMyCharacter::SetControlMode(EControlMode ControlMode)
{
	CurrentControlMode = ControlMode;

	if (CurrentControlMode == EControlMode::GTA)
	{
		springArm->bUsePawnControlRotation = true;
		springArm->TargetArmLength = 400.0f;
		springArm->SetRelativeRotation(FRotator::ZeroRotator);
		springArm->bInheritPitch = true;
		springArm->bInheritRoll = true;
		springArm->bInheritYaw = true;
		springArm->bDoCollisionTest = true;


		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	}
	else if (CurrentControlMode == EControlMode::DIABLO)
	{
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);

		springArm->bUsePawnControlRotation = false;
		springArm->TargetArmLength = 800.0f;
		//springArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
		springArm->bInheritPitch = false;
		springArm->bInheritRoll = false;
		springArm->bInheritYaw = false;
		springArm->bDoCollisionTest = false;

		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	}

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	if (CurrentControlMode == EControlMode::DIABLO)
	{
		springArm->SetRelativeRotation(FMath::RInterpTo(
			springArm->GetRelativeRotation(),
			ArmRotationTo,
			DeltaTime,
			ArmRotationSpeed
		));
		if (direction.SizeSquared() > 0.0f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(direction).Rotator());
			AddMovementInput(direction);
		}
	}
	else if (CurrentControlMode == EControlMode::GTA)
	{

	}
	
	
}

void AMyCharacter::StartJump()
{
	bPressedJump = true;
}

void AMyCharacter::StopJump()
{
	bPressedJump = false;
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMyCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMyCharacter::Turn);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMyCharacter::StartJump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &AMyCharacter::StopJump);
	//PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AMyCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AMyCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Respawn"), EInputEvent::IE_Pressed, this, &AMyCharacter::GetRespawn);
}

void AMyCharacter::UpDown(float value)
{
	/*FVector direction = FRotationMatrix(GetController()->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(direction, value);*/
	if (CurrentControlMode == EControlMode::DIABLO)
		direction.X = value;
	else if (CurrentControlMode == EControlMode::GTA)
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), value);
}

void AMyCharacter::LeftRight(float value)
{
	/*FVector direction = FRotationMatrix(GetController()->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(direction, value);*/
	if (CurrentControlMode == EControlMode::DIABLO)
		direction.Y = value;
	else if (CurrentControlMode == EControlMode::GTA)
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), value);
}
void AMyCharacter::LookUp(float value)
{
	//AddControllerPitchInput(value);
	if (CurrentControlMode == EControlMode::GTA)
		AddControllerPitchInput(value);
}

void AMyCharacter::Turn(float value)
{
	//AddControllerYawInput(value);
	if (CurrentControlMode == EControlMode::GTA)
		AddControllerYawInput(value);
}

void AMyCharacter::Attack()
{

	//IsAttacking = true;
	if (IsAttacking)
	{
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		AttackStartComboState();
		AnimInstance->PlayAttackMontage();
		AnimInstance->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
	
}

void AMyCharacter::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.0f,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.0f),
		Params);

#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() + AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;
	DrawDebugCapsule
	(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);
#endif



	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, (TEXT("HitCharacter")));
			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(50.0f, DamageEvent, GetController(), this);
		}
	}

}

void AMyCharacter::GetRespawn()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "SpawnPoint", actors);
	AMyPlayerStart* SpawnPoint = Cast<AMyPlayerStart>(actors[0]);
	if (SpawnPoint)
	{
		SetActorLocation(SpawnPoint[0].SpawnPoint);
	}
}

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;
	AnimInstance->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnMontageEndedHandler);
	AnimInstance->OnEndHandler.AddUObject(this, &AMyCharacter::OnEndHandler);
	AnimInstance->OnAttackHitCheck.AddUObject(this, &AMyCharacter::AttackCheck);

	CanNextCombo = false;
	if (IsComboInputOn)
	{
		AttackStartComboState();
		AnimInstance->JumpToAttackMontageSection(CurrentCombo);
	}
}

void AMyCharacter::OnMontageEndedHandler(UAnimMontage* Montage, bool bInterruped)
{
	IsAttacking = false;
	AttackEndComboState();
}

void AMyCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AMyCharacter::AttackEndComboState()
{
	CanNextCombo = false;
	IsComboInputOn = false;
	CurrentCombo = 0;
}

void AMyCharacter::OnEndHandler()
{
	CanNextCombo = false;
	if (IsComboInputOn)
	{
		AttackStartComboState();
		AnimInstance->JumpToAttackMontageSection(CurrentCombo);
	}
}



void AMyCharacter::ViewChange()
{
	if (CurrentControlMode == EControlMode::GTA)
	{
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIABLO);
	}
	else if (CurrentControlMode == EControlMode::DIABLO)
	{
		GetController()->SetControlRotation(springArm->GetRelativeRotation());
		SetControlMode(EControlMode::GTA);
	}
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (FinalDamage > 0.0f)
	{
		AnimInstance->SetDeadAnim();
		SetActorEnableCollision(false);
		Destroy();
	}
	return FinalDamage;
}



//void AMyCharacter::CustomJump()
//{
//	IsInAir = true;
//}

