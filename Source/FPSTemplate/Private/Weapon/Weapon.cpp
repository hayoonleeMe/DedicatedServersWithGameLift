// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "Interfaces/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"


AWeapon::AWeapon()
{
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetHiddenInGame(true);
	SetRootComponent(Mesh1P);

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh3P");
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->SetupAttachment(Mesh1P);
	Mesh3P->SetHiddenInGame(true);
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	AimFieldOfView = 65.f;
}

USkeletalMeshComponent* AWeapon::GetMesh1P() const
{
	return Mesh1P;
}

USkeletalMeshComponent* AWeapon::GetMesh3P() const
{
	return Mesh3P;
}

UMaterialInstanceDynamic* AWeapon::GetReticleDynamicMaterialInstance()
{
	if (!IsValid(DynMatInst_Reticle))
	{
		DynMatInst_Reticle = UMaterialInstanceDynamic::Create(ReticleMaterial, this);
	}
	return DynMatInst_Reticle;
}

UMaterialInstanceDynamic* AWeapon::GetAmmoCounterDynamicMaterialInstance()
{
	if (!IsValid(DynMatInst_AmmoCounter))
	{
		DynMatInst_AmmoCounter = UMaterialInstanceDynamic::Create(AmmoCounterMaterial, this);
	}
	return DynMatInst_AmmoCounter;
}

UMaterialInstanceDynamic* AWeapon::GetWeaponIconDynamicMaterialInstance()
{
	if (!IsValid(DynMatInst_WeaponIcon))
	{
		DynMatInst_WeaponIcon = UMaterialInstanceDynamic::Create(WeaponIconMaterial, this);
	}
	return DynMatInst_WeaponIcon;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::AttachMeshToPawn()
{
	APawn* MyPawn = GetInstigator();
	if (!IsValid(MyPawn) || !MyPawn->Implements<UPlayerInterface>()) return;
	
	// Remove and hide both first and third person meshes
	DetachMeshFromPawn();

	// For locally controlled players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
	const FName AttachPoint = IPlayerInterface::Execute_GetWeaponAttachPoint(MyPawn, WeaponType);
	
	if (MyPawn->IsLocallyControlled())
	{
		USkeletalMeshComponent* PawnMesh1p = IPlayerInterface::Execute_GetSpecifcPawnMesh(MyPawn, true); 
		USkeletalMeshComponent* PawnMesh3p = IPlayerInterface::Execute_GetSpecifcPawnMesh(MyPawn, false);
		Mesh1P->SetHiddenInGame(false);
		Mesh3P->SetHiddenInGame(true);
		Mesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		Mesh3P->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	}
	else
	{
		USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
		USkeletalMeshComponent* UsePawnMesh = IPlayerInterface::Execute_GetPawnMesh(MyPawn);
		UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		UseWeaponMesh->SetHiddenInGame(false);
	}
	
}

void AWeapon::OnEnterInventory(APawn* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void AWeapon::OnUnEquip()
{
	DetachMeshFromPawn();
}

void AWeapon::OnEquip(const AWeapon* LastWeapon)
{
	AttachMeshToPawn();
}

void AWeapon::SetOwningPawn(APawn* NewOwningPawn)
{
	if (GetOwner() != NewOwningPawn)
	{
		SetInstigator(NewOwningPawn);
		
		// net owner for RPC calls
		SetOwner(NewOwningPawn);
	}

	if (IsValid(NewOwningPawn))
	{
		if (NewOwningPawn->IsLocallyControlled())
		{
			Mesh1P->SetHiddenInGame(false);
			Mesh3P->SetHiddenInGame(true);
		}
		else
		{
			Mesh1P->SetHiddenInGame(true);
			Mesh3P->SetHiddenInGame(false);
		}
	}
}

USkeletalMeshComponent* AWeapon::GetWeaponMesh() const
{
	if (GetOwner() == nullptr) return nullptr;
	if (!GetOwner()->Implements<UPlayerInterface>()) return nullptr;

	return IPlayerInterface::Execute_IsFirstPerson(GetOwner()) ? Mesh1P : Mesh3P;
}

void AWeapon::SetWeaponState(EWeaponState::Type NewState)
{
	CurrentState = NewState;
}

EWeaponState::Type AWeapon::GetWeaponState() const
{
	return CurrentState;
}

void AWeapon::DetachMeshFromPawn()
{
	Mesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh1P->SetHiddenInGame(true);

	Mesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh3P->SetHiddenInGame(true);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::OnRep_Instigator()
{
	Super::OnRep_Instigator();
	
	APawn* MyPawn = GetInstigator();
	if (IsValid(MyPawn))
	{
		if (MyPawn->IsLocallyControlled())
		{
			Mesh1P->SetHiddenInGame(false);
			Mesh3P->SetHiddenInGame(true);
		}
		else
		{
			Mesh1P->SetHiddenInGame(true);
			Mesh3P->SetHiddenInGame(false);
		}
	}
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	DetachMeshFromPawn();
}

void AWeapon::Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> SurfaceType, bool bIsFirstPerson)
{

	FireEffects(ImpactPoint, ImpactNormal, SurfaceType, bIsFirstPerson);
	
	
	if (GetInstigator()->IsLocallyControlled())
	{
		Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
		++Sequence;
	}
	
}

int32 AWeapon::Auth_Fire()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	return Ammo;
}

void AWeapon::Rep_Fire(int32 AuthAmmo)
{
	if (GetInstigator()->IsLocallyControlled())
	{
		Ammo = AuthAmmo;
		--Sequence;
		Ammo -= Sequence;
	}
	
   

}

