// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	AccelerationDueToGravity = GetWorld()->GetGravityZ() / 100;
	bReplicates = true;
	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
}

void AGoKart::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGoKart, ServerState);
	DOREPLIFETIME(AGoKart, Throttle);
	DOREPLIFETIME(AGoKart, SteeringThrow);
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "Error";
	}
}

// Called every time there's an update on the actor's transform on the server
void AGoKart::OnRep_ServerState()
{
	// Compare transform and velocity from server to our record, add the delta
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		FGoKartMove Move;
		Move.DeltaTime = DeltaTime;
		Move.Throttle = Throttle;
		Move.SteeringThrow = SteeringThrow;
		Move.Time = GetWorld()->TimeSeconds;

		Server_SendMove(Move);
	}

	FVector Force = CalculateForceOnCar();
	FVector Acceleration = Force / Mass;

	Velocity += Acceleration * DeltaTime;
	
	UpdateRotationFromSteering(DeltaTime);
	UpdateLocationFromVelocity(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 1), GetEnumText(Role), this, FColor::White, DeltaTime);
	
	if (HasAuthority())
	{
		ServerState.Transform = GetActorTransform();
		ServerState.Velocity = Velocity;
		//TODO: update last move ServerState.LastMove = 
	}	

}

FVector AGoKart::CalculateForceOnCar()
{
	FVector ForwardForce = GetActorForwardVector() * MaxDrivingForce * Throttle;
	float AirResistance = Velocity.SizeSquared() * DragCoefficient;
	FVector DragForce = Velocity.GetSafeNormal() * AirResistance;
	FVector NormalForce = -Velocity.GetSafeNormal() * Mass * RollingResistanceCoefficient * AccelerationDueToGravity;
	// Return throttle force - drag force - normal force
	return ForwardForce - DragForce - NormalForce;
}

void AGoKart::UpdateRotationFromSteering(float DeltaTime)
{
	float RotationAngle = SteeringThrow * FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime / MinTurningRadius;
	
	// dx = dtheta x SteeringCircleRadius
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);

	AddActorWorldRotation(RotationDelta);
	// Rotate Velocity with the RotationDelta
	Velocity = RotationDelta.RotateVector(Velocity);
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = 100 * Velocity * DeltaTime;
	FHitResult ObjectHit;
	AddActorWorldOffset(Translation, true, &ObjectHit);
	if (ObjectHit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value)
{
	Throttle = Value;
}

void AGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
}

void AGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{
	Throttle = Move.Throttle;
	SteeringThrow = Move.SteeringThrow;
}

bool AGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
	//return FMath::Abs(Move.Throttle) <= 1 && FMath::Abs(Move.SteeringThrow) <= 1;
	return true;
}

