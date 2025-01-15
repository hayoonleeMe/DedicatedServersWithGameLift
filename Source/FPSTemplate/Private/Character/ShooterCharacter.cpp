// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/ShooterHealthComponent.h"
#include "Data/WeaponData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Combat/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Elimination/EliminationComponent.h"
#include "FPSTemplate/FPSTemplate.h"
#include "Game/ShooterGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/ShooterPlayerController.h"
#include "Weapon/Weapon.h"

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 0.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 15.f;
	SpringArm->bUsePawnControlRotation = true;
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	FirstPersonCamera->SetupAttachment(SpringArm);
	FirstPersonCamera->bUsePawnControlRotation = false;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->SetupAttachment(FirstPersonCamera);
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;

	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;

	Combat = CreateDefaultSubobject<UCombatComponent>("Combat");
	Combat->SetIsReplicated(true);

	HealthComponent = CreateDefaultSubobject<UShooterHealthComponent>("Health");
	HealthComponent->SetIsReplicated(true);

	EliminationComponent = CreateDefaultSubobject<UEliminationComponent>("EliminationComponent");
	EliminationComponent->SetIsReplicated(false);

	DefaultFieldOfView = 90.f;
	TurningStatus = ETurningInPlace::NotTurning;
	bWeaponFirstReplicated = false;
}

void AShooterCharacter::OnDeathStarted(AActor* DyingActor, AActor* DeathInstigator)
{
	int32 Index = FMath::RandRange(0, DeathMontages.Num() - 1);
	UAnimMontage* DeathMontage = DeathMontages[Index];
	AShooterPlayerController* VictimController = Cast<AShooterPlayerController>(GetController());
	if (HasAuthority() && IsValid(VictimController))
	{
		if (IsValid(Combat))
		{
			Combat->DestroyInventory();
		}
		if (AShooterGameModeBase* GameMode = Cast<AShooterGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			ACharacter* InstigatorCharacter = Cast<ACharacter>(DeathInstigator);
			if (IsValid(InstigatorCharacter))
			{
				APlayerController* InstigatorController = Cast<APlayerController>(InstigatorCharacter->GetController());
				if (IsValid(InstigatorController))
				{
					GameMode->StartPlayerElimination(DeathMontage->GetPlayLength(), this, VictimController, InstigatorController);
				}
			}
		}
	}
	if (GetNetMode() != NM_DedicatedServer)
	{
		DeathEffects(DeathInstigator, DeathMontage);
		if (IsValid(VictimController))
		{
			DisableInput(VictimController);
			if (IsLocallyControlled())
			{
				VictimController->bPawnAlive = false;
			}
		}
	}
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Weapon, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Weapon, ECR_Ignore);
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HealthComponent))
	{
		HealthComponent->OnDeathStarted.AddDynamic(this, &AShooterCharacter::OnDeathStarted);
	}
	AShooterPlayerController* VictimController = Cast<AShooterPlayerController>(GetController());
	if (IsLocallyControlled() && IsValid(VictimController))
	{
		VictimController->bPawnAlive = true;
	}
	StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);

	if (IsValid(Combat) && IsValid(EliminationComponent) && HasAuthority())
	{
		Combat->OnRoundReported.AddDynamic(EliminationComponent, &UEliminationComponent::OnRoundReported);
	}
	FTimerDelegate InitializeWidgetsDelegate;
	InitializeWidgetsDelegate.BindLambda([&]
	{
		if (IsValid(Combat) && IsLocallyControlled())
		{
			Combat->InitializeWeaponWidgets();
		}
	});
	GetWorldTimerManager().SetTimer(InitiializeWidgets_Timer, InitializeWidgetsDelegate, 0.5f, false);
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* ShooterInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	ShooterInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::Input_CycleWeapon);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::Input_FireWeapon_Pressed);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &AShooterCharacter::Input_FireWeapon_Released);
	ShooterInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::Input_ReloadWeapon);
	ShooterInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AShooterCharacter::Input_Aim_Pressed);
	ShooterInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AShooterCharacter::Input_Aim_Released);
}

void AShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (IsValid(Combat))
	{
		Combat->SpawnDefaultInventory();
	}
	APlayerController* PlayerController = Cast<APlayerController>(NewController);
	if (IsValid(PlayerController))
	{
		EnableInput(PlayerController);
	}
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningStatus == ETurningInPlace::NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		
		FRotator AimRotation = GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation).Yaw;
		TurningStatus = ETurningInPlace::NotTurning;
	}

	if (IsValid(Combat) && IsValid(Combat->CurrentWeapon) && IsValid(Combat->CurrentWeapon->GetMesh3P()))
	{
		FABRIK_SocketTransform = Combat->CurrentWeapon->GetMesh3P()->GetSocketTransform(FName("FABRIK_Socket"), RTS_World);
		FVector OutLocation;
		FRotator OutRotation;
		GetMesh()->TransformToBoneSpace(FName("hand_r"), FABRIK_SocketTransform.GetLocation(), FABRIK_SocketTransform.GetRotation().Rotator(), OutLocation, OutRotation);
		FABRIK_SocketTransform.SetLocation(OutLocation);
		FABRIK_SocketTransform.SetRotation(OutRotation.Quaternion());
	}

	AO_Yaw *= -1.f;
}

void AShooterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AShooterCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	if (IsValid(Combat))
	{
		Combat->DestroyInventory();
	}
}

void AShooterCharacter::UnPossessed()
{
	if (IsValid(Combat))
	{
		Combat->DestroyInventory();
	}
	Super::UnPossessed();
}

void AShooterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningStatus = ETurningInPlace::Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningStatus = ETurningInPlace::Left;
	}
	if (TurningStatus != ETurningInPlace::NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 5.f)
		{
			TurningStatus = ETurningInPlace::NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

FName AShooterCharacter::GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const
{
	checkf(Combat->WeaponData, TEXT("No Weapon Data Asset - Please fill out BP_ShooterCharacter"));
	return Combat->WeaponData->GripPoints.FindChecked(WeaponType);
}

USkeletalMeshComponent* AShooterCharacter::GetSpecifcPawnMesh_Implementation(bool WantFirstPerson) const
{
	return WantFirstPerson == true ? Mesh1P.Get() : GetMesh();
}

USkeletalMeshComponent* AShooterCharacter::GetPawnMesh_Implementation() const
{
	return IsLocalFirstPerson() ? Mesh1P.Get() : GetMesh();
}

bool AShooterCharacter::IsFirstPerson_Implementation() const
{
	return IsLocalFirstPerson();
}

bool AShooterCharacter::DoDamage_Implementation(float DamageAmount, AActor* DamageInstigator)
{
	if (!IsValid(HealthComponent)) return false;

	const bool bLethal = HealthComponent->ChangeHealthByAmount(-DamageAmount, DamageInstigator);
	const int32 MontageSelection = FMath::RandRange(0, HitReacts.Num() - 1);
	Multicast_HitReact(MontageSelection);
	return bLethal;
}

void AShooterCharacter::AddAmmo_Implementation(const FGameplayTag& WeaponType, int32 AmmoAmount)
{
	if (HasAuthority() && IsValid(Combat))
	{
		Combat->AddAmmo(WeaponType, AmmoAmount);
	}
}

AWeapon* AShooterCharacter::GetCurrentWeapon_Implementation()
{
	if (!IsValid(Combat)) return nullptr;
	return Combat->CurrentWeapon;
}

int32 AShooterCharacter::GetCarriedAmmo_Implementation()
{
	if (!IsValid(Combat)) return -1;
	return Combat->CarriedAmmo;
}

void AShooterCharacter::InitializeWidgets_Implementation()
{
	if (!IsValid(Combat)) return;
	return Combat->InitializeWeaponWidgets();
}

void AShooterCharacter::Multicast_HitReact_Implementation(int32 MontageIndex)
{
	if (GetNetMode() != NM_DedicatedServer && !IsLocallyControlled())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReacts[MontageIndex]);
	}
}

FRotator AShooterCharacter::GetFixedAimRotation() const
{
	FRotator AimRotation = GetBaseAimRotation();
	if (AimRotation.Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		const FVector2D InRange(270.f, 360.f);
		const FVector2D OutRange(-90.f, 0.f);
		AimRotation.Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimRotation.Pitch);
	}
	return AimRotation;
}

bool AShooterCharacter::IsLocalFirstPerson() const
{
	return IsValid(Controller) && Controller->IsLocalPlayerController();
}

void AShooterCharacter::Input_CycleWeapon()
{
	Combat->Initiate_CycleWeapon();
}

void AShooterCharacter::Notify_CycleWeapon_Implementation()
{
	Combat->Notify_CycleWeapon();
}

void AShooterCharacter::Notify_ReloadWeapon_Implementation()
{
	Combat->Notify_ReloadWeapon();
}

void AShooterCharacter::Input_FireWeapon_Pressed()
{
	Combat->Initiate_FireWeapon_Pressed();
}

void AShooterCharacter::Input_FireWeapon_Released()
{
	Combat->Initiate_FireWeapon_Released();
}

void AShooterCharacter::Input_ReloadWeapon()
{
	Combat->Initiate_ReloadWeapon();
}

void AShooterCharacter::Input_Aim_Pressed()
{
	Combat->Initiate_AimPressed();
	OnAim(true);
}

void AShooterCharacter::Input_Aim_Released()
{
	Combat->Initiate_AimReleased();
	OnAim(false);
}

void AShooterCharacter::Initiate_Crouch_Implementation()
{
	GetCharacterMovement()->bWantsToCrouch = !GetCharacterMovement()->bWantsToCrouch;
}

void AShooterCharacter::Initiate_Jump_Implementation()
{
	if (GetCharacterMovement()->bWantsToCrouch)
	{
		GetCharacterMovement()->bWantsToCrouch = false;
	}
	else
	{
		Jump();
	}
}

bool AShooterCharacter::IsDeadOrDying_Implementation()
{
	if (IsValid(HealthComponent))
	{
		return HealthComponent->IsDeadOrDying();
	}
	return true;
}

void AShooterCharacter::WeaponReplicated_Implementation()
{
	if (!bWeaponFirstReplicated)
	{
		bWeaponFirstReplicated = true;
		OnWeaponFirstReplicated.Broadcast(Combat->CurrentWeapon);
	}
}




