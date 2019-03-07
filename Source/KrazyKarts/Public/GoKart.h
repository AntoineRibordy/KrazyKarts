// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

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

	// The number of degrees rotated per second at full control throw
	UPROPERTY(EditAnywhere)
	float MaxDegreesPerSecond = 90;

	FVector Velocity;
	float Throttle;
	float SteeringThrow;
	float AccelerationDueToGravity;

	FVector CalculateForceOnCar();
	void UpdateLocationFromVelocity(float DeltaTime);
	void UpdateRotationFromSteering(float DeltaTime);
	void MoveForward(float InputVelocity);
	void MoveRight(float Value);
};