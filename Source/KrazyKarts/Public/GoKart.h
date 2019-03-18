// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY()
	float Throttle;
	
	UPROPERTY()
	float SteeringThrow;
	
	UPROPERTY()
	float DeltaTime;
	
	UPROPERTY()
	float Time;
};

USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY()
	FGoKartMove LastMove;
	
	UPROPERTY()
	FTransform Transform;
	
	UPROPERTY()
	FVector Velocity;
	
};

UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	// The mass of the car (in kg)
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	// The force applied to the car whe the throttle is fully down (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// Higher means more drag (kg/m)
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	// Higher means more rolling resistance
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;

	// Minimum radius of the kart turning circle at full lock (m)
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 5;

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

	FVector Velocity;

	UPROPERTY(Replicated)
	float Throttle;

	UPROPERTY(Replicated)
	float SteeringThrow;

	float AccelerationDueToGravity;

	FVector CalculateForceOnCar();
	void UpdateLocationFromVelocity(float DeltaTime);
	
	void UpdateRotationFromSteering(float DeltaTime);

	void MoveForward(float InputVelocity);
	void MoveRight(float Value);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

	
};
