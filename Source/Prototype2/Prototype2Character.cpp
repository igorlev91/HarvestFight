// Copyright Epic Games, Inc. All Rights Reserved.

#include "Prototype2Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InteractInterface.h"
#include "PickUpItem.h"
#include "Prototype2PlayerController.h"
#include "Components/SphereComponent.h"
#include "DynamicMesh/ColliderMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Widget_InteractionPanel.h"
#include "Widgets/Widget_IngameMenu.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Widget_PlayerHUD.h"



APrototype2Character::APrototype2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

void APrototype2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (PlayerHudPrefab)
	{
		if (Controller)
		{
			if (auto* playerController = Cast<APrototype2PlayerController>(Controller))
			{
				PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(playerController, PlayerHudPrefab);

				if (PlayerHUDRef)
					PlayerHUDRef->AddToViewport();
			}
		}
	}
}

void APrototype2Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// Stun
	if (bIsStunned)
	{
		bIsChargingAttack = false;

		// Set animation to stun
		
		StunTimer -= DeltaSeconds;
		if (StunTimer < 0.0f)
		{
			bIsStunned = false;

			// Enable input
		}
	}
	
	// Attack
	if (bIsChargingAttack)
	{
		AttackChargeAmount += DeltaSeconds;
	}

	// Check if anything is around to be interacted with
	CheckForInteractables();
}

void APrototype2Character::ChargeAttack()
{
	bIsChargingAttack = true;
	
	// todo: Set animation to raise hand/hands
}

void APrototype2Character::ReleaseAttack()
{
	// todo: Set animation to execute the attack
	
	// Cap attack charge
	if (AttackChargeAmount > MaxAttackCharge)
	{
		AttackChargeAmount = MaxAttackCharge;
	}

	// Create a sphere collider, check if player hit, call player hit
	int32 attackSphereRadius;
	if (Weapon)
	{
		// Create a larger sphere of effect
		attackSphereRadius = 100.0f + AttackChargeAmount * 10.0f;
	}
	else
	{
		// Create a smaller sphere of effect
		attackSphereRadius = 50.0f + AttackChargeAmount * 10.0f;
	}

	ExecuteAttack(attackSphereRadius);
	
	// Reset Attack variables
	bIsChargingAttack = false;
	AttackChargeAmount = 0.0f;
}

void APrototype2Character::ExecuteAttack(float AttackSphereRadius)
{
	// create tarray for hit results
	TArray<FHitResult> outHits;
	
	// start and end locations
	FVector inFrontOfPlayer = GetActorLocation() + (GetActorForwardVector() * AttackSphereRadius) +  (GetActorForwardVector() * 30.0f);
	FVector sweepStart = inFrontOfPlayer;
	FVector sweepEnd = inFrontOfPlayer;

	// create a collision sphere
	FCollisionShape colSphere = FCollisionShape::MakeSphere(AttackSphereRadius);
	
	UE_LOG(LogTemp, Warning, TEXT("Sphere Radius = %s"), *FString::SanitizeFloat(AttackSphereRadius));
	// draw collision sphere
	DrawDebugSphere(GetWorld(), inFrontOfPlayer, colSphere.GetSphereRadius(), 50, FColor::Purple, false, 3.0f);
	
	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(outHits, sweepStart, sweepEnd, FQuat::Identity, ECC_WorldStatic, colSphere);

	if (isHit)
	{
		// loop through TArray
		for (auto& hit : outHits)
		{
			if (auto* hitPlayer = Cast<APrototype2Character>(hit.GetActor()))
			{
				if (hitPlayer != this)
				{
					// screen log information on what was hit
					UE_LOG(LogTemp, Warning, TEXT(" %s  was hit by an attack!"), *hit.GetActor()->GetName());
					hitPlayer->GetHit(AttackChargeAmount);
				}
			}
		}
	}
}

void APrototype2Character::Interact()
{
	if (HeldItem)
	{
		HeldItem->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
        // HeldItem->EnablePhysics // Needs the ItemComponent stuff
		HeldItem = nullptr;
	}
	else
	{
		// Pickup closest interactable item
		if(ClosestInteractableItem)
		{
			ClosestInteractableItem->Interact();
		}
	}
}

void APrototype2Character::CheckForInteractables()
{
	// create tarray for hit results
	TArray<FHitResult> outHits;
	
	// start and end locations
	FVector sweepStart = GetActorLocation();
	FVector sweepEnd = GetActorLocation();

	// create a collision sphere
	FCollisionShape colSphere = FCollisionShape::MakeSphere(InteractRadius);

	// draw collision sphere
	//DrawDebugSphere(GetWorld(), GetActorLocation(), colSphere.GetSphereRadius(), 50, FColor::Purple, true);
	
	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(outHits, sweepStart, sweepEnd, FQuat::Identity, ECC_WorldStatic, colSphere);

	if (isHit)
	{
		TArray<AActor*> interactableActors;
		
		// loop through TArray
		for (auto& hit : outHits)
		{
			// screen log information on what was hit
			//UE_LOG(LogTemp, Warning, TEXT(" %s "), *hit.GetActor()->GetName());
			
			if (Cast<IInteractInterface>(hit.GetActor()))
			{
				interactableActors.Add(hit.GetActor());
			}
		}

		float distanceToClosest;
		ClosestInteractableItem = Cast<IInteractInterface>(UGameplayStatics::FindNearestActor(GetActorLocation(), interactableActors, distanceToClosest));
	}
	else
	{
		ClosestInteractableItem = nullptr;
	}
}

void APrototype2Character::GetHit(float AttackCharge)
{
	// Disable input
	
	bIsStunned = true;
	StunTimer = AttackCharge;
}

void APrototype2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		////Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APrototype2Character::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APrototype2Character::Look);

		// UI
		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this, &APrototype2Character::OpenIngameMenu);

		// Attack
		EnhancedInputComponent->BindAction(ChargeAttackAction, ETriggerEvent::Triggered, this, &APrototype2Character::ChargeAttack);
		EnhancedInputComponent->BindAction(ReleaseAttackAction, ETriggerEvent::Triggered, this, &APrototype2Character::ReleaseAttack);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APrototype2Character::Interact);
	}
}

void APrototype2Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APrototype2Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APrototype2Character::OpenIngameMenu()
{
	if (auto* playercontroller = Cast<APrototype2PlayerController>(Controller))
	{
		if (PlayerHUDRef)
			PlayerHUDRef->EnableDisableMenu();
	}
}
