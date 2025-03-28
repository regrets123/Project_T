// Fill out your copyright notice in the Description page of Project Settings.


#include "States/MovementStates/HangState.h"
#include "Characters/PlayerChar.h"
#include "Characters/PlayerCharAnimInst.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "States/StateMachine.h"


void UHangState::OnStateEnter()
{
	Char->ResetJump();
	Char->IsMoveEnabled = false;
	UCharacterMovementComponent* MovementComp = Char->GetCharacterMovement();
	if (MovementComp)
	{
		Char->SaveCollisionResponses();
		Char->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		MovementComp->SetMovementMode(MOVE_Flying);
		Char->GetAnimInst()->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
		MovementComp->StopMovementImmediately();
		Char->SetWarpPosition(Char->GetEdgePosition(),Char->GetEdgeRotation(),FName("LedgeEdge"));
	}
}

void UHangState::OnStateTick(float DeltaTime)
{
	float InputMagnitude = Char->GetInputMag();
	FVector2D InputDir = Char->GetLastInputDirection();
    
	
	float InputThreshold = 0.2f; 
	if (InputMagnitude > InputThreshold)
	{
		// Normalize Y input to account for diagonal movement
		float NormalizedYInput = InputDir.Y / InputMagnitude;
        
		// Check for up movement (climb)
		if (NormalizedYInput > 0.7f || Char->IsJumpPressed ) // Using 0.7 to require more deliberate up movement
		{
			if(MoveUp())
			{
				TransitionToClimb();
				return;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("MoveUpRaycasting failed!"));
			}
		}
        
		// Check for down movement (release)
		if (NormalizedYInput < -0.7f)
		{
			TransitionToFall();
		}
	}
}


void UHangState::OnStateExit()
{
}

void UHangState::TransitionToClimb()
{
	Char->OnClimbUp(FName("LedgeTop"), FName("LedgeFinal"), FName("ClimbUp"));
	Machine->ChangeState(EMovementState::Ems_Climb);
}

void UHangState::TransitionToFall()
{
	Char->OnClimbDown(FName("Drop"));
	Char->OnMontageExit();
	Char->GetAnimInst()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	FVector BackwardDirection = -Char->GetActorForwardVector();
	Char->LaunchCharacter(BackwardDirection * BackwardsForce, true, true);
	Machine->ChangeState(EMovementState::Ems_Fall);
}

bool UHangState::MoveUp()
{
	FVector LedgeLocation = Char->GetEdgeRaw();
	UGravityComponent* Grav = Char->CustomGravity;
	FVector GravityDirection = Grav->GetCustomGravity().GetSafeNormal();
	FVector UpVector = GravityDirection * -1;
	FVector WallNormal = Char->GetWallNormal();
	FVector ForwardOffset;
	FVector FinalPosition;
	
	if(FindForwardEdge(LedgeLocation, WallNormal, ForwardOffset))
	{
		if(FindFinalPosition(UpVector, FinalPosition, ForwardOffset))
		{
			FVector HighestPosition = FindHighestPosition(LedgeLocation, WallNormal, UpVector);
			Char->SetMantlePositions(FinalPosition, HighestPosition);
			return true;
		}
	}
	return false;
}


bool UHangState::FindForwardEdge(const FVector& StartLocation, const FVector& WallNormal, FVector& OutForwardHitWithOffset)
{
	FVector ForwardDirection = -WallNormal; 
	float ForwardTraceDistance = 150.f;
	float BackoffDistance = ForwardTraceDistance * 0.5f;
	float HalfHeight = Char->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	UWorld* World = Char->GetWorld();
	FVector RaisedStart = StartLocation + (Char->CustomGravity->GetCustomGravity() * -HalfHeight);
	FVector ForwardEnd = RaisedStart + (ForwardDirection * ForwardTraceDistance);
	DrawDebugSphere(World, RaisedStart, 10.f, 12, FColor::Magenta, false, 10.f);
	DrawDebugSphere(World, ForwardEnd, 10.f, 12, FColor::Magenta, false, 10.f);
	DrawDebugLine(World, RaisedStart, ForwardEnd, FColor::Cyan, false, 10.f, 0, 1.0f);
	
	FHitResult ForwardHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Char);
	bool bHitForward = World->LineTraceSingleByChannel(
		ForwardHit,
		RaisedStart,
		ForwardEnd,
		ECC_Pawn,
		QueryParams
	);
	if(bHitForward)
	{
		if(!ForwardHit.bStartPenetrating)
		{
			if (ForwardHit.Distance < MinimumLedgeDepth)
			{
				UE_LOG(LogTemp, Warning, TEXT("Ledge too narrow! Hit distance: %f, Required: %f"), 
					   ForwardHit.Distance, MinimumLedgeDepth);
				return false;
			}
			BackoffDistance = ForwardHit.Distance * 0.5f; 
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FindForwardEdge failed, because bHitForward: %s, bStartPenetrating: %s"), 
				(bHitForward ? TEXT("true") : TEXT("false")),
				(ForwardHit.bStartPenetrating ? TEXT("true") : TEXT("false")));
			return false;
		}
	}
	else
	{
		OutForwardHitWithOffset = RaisedStart + ForwardDirection * BackoffDistance;
		return true;
	}

	OutForwardHitWithOffset = ForwardHit.Location + (WallNormal * BackoffDistance);
	return true;
}

FVector UHangState::FindHighestPosition(const FVector& LedgeLocationRaw, const FVector& WallNormal, const FVector& UpDirection)
{
	float WallOffset = 40.f;
	float HeightOffset = 75.f;
	FVector HighestPos = LedgeLocationRaw + WallNormal * WallOffset - HeightOffset *  UpDirection;
	return HighestPos;
}

bool UHangState::FindFinalPosition(const FVector& UpDirection, FVector& OutFinal, const FVector& ForwardOffset)
{
	if (UWorld* World = Char->GetWorld())
	{
		float RayDistance = 200.f;
		float SphereRadius = Char->GetCapsuleComponent()->GetScaledCapsuleRadius() * 0.5f;
		FVector StartPos = ForwardOffset;
		FVector EndPos = ForwardOffset - UpDirection * RayDistance;
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Char);
		QueryParams.bTraceComplex = true;
		FHitResult FinalResult;
		//bool bHitSurface = World->LineTraceSingleByChannel(FinalResult, StartPos, EndPos, ECC_Pawn, QueryParams);
		bool bHitSurface = World->SweepSingleByChannel(FinalResult, StartPos, EndPos,FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SphereRadius), QueryParams);
		DrawDebugLine(World, StartPos, EndPos, FColor::Black, false, 5.0f, 0, 1.0f);
		DrawDebugSphere(World, FinalResult.ImpactPoint, 10.0f, 12, FColor::Black, false, 5.0f);
		DrawDebugSphere(World, StartPos, 10.0f, 12, FColor::Red, false, 5.0f);
		if (bHitSurface && !FinalResult.bStartPenetrating)
		{
			OutFinal = FinalResult.Location;
			return  true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FinalPosition failed, because bHitSurface: %s, bStartPenetrating: %s"), 
			(bHitSurface ? TEXT("true") : TEXT("false")),
			(FinalResult.bStartPenetrating ? TEXT("true") : TEXT("false")));
		}
	}
	return false;
}


