// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ShooterPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Interfaces/PlayerInterface.h"

AShooterPlayerController::AShooterPlayerController()
{
	bReplicates = true;
	bPawnAlive = true;
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	bPawnAlive = true;
}

void AShooterPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	OnPlayerStateReplicated.Broadcast();
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(ShooterIMC, 0);
	}
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* ShooterInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	ShooterInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::Input_Move);
	ShooterInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::Input_Look);
	ShooterInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AShooterPlayerController::Input_Crouch);
	ShooterInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShooterPlayerController::Input_Jump);
	
}

void AShooterPlayerController::Input_Move(const FInputActionValue& InputActionValue)
{
	if (!bPawnAlive) return;
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AShooterPlayerController::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	AddYawInput(InputAxisVector.X);
	AddPitchInput(InputAxisVector.Y);
}

void AShooterPlayerController::Input_Crouch()
{
	if (!bPawnAlive) return;
	if (GetPawn() == nullptr || !GetPawn()->Implements<UPlayerInterface>()) return;
	IPlayerInterface::Execute_Initiate_Crouch(GetPawn());
}

void AShooterPlayerController::Input_Jump()
{
	if (!bPawnAlive) return;
	if (GetPawn() == nullptr || !GetPawn()->Implements<UPlayerInterface>()) return;
	IPlayerInterface::Execute_Initiate_Jump(GetPawn());
}


