// Fill out your copyright notice in the Description page of Project Settings.

#include "States/StateMachine.h"
#include "Characters/PlayerChar.h"
#include "States/MovementStates/ClimbingState.h"
#include "States/MovementStates/CrouchState.h"
#include "States/MovementStates/DashState.h"
#include "States/MovementStates/DodgeState.h"
#include "States/MovementStates/DoubleJumpState.h"
#include "States/MovementStates/FallState.h"
#include "States/MovementStates/HangState.h"
#include "States/MovementStates/IdleState.h"
#include "States/MovementStates/JogState.h"
#include "States/MovementStates/LandingState.h"
#include "States/MovementStates/RunState.h"
#include "States/MovementStates/ShortStopState.h"
#include "States/MovementStates/StopSlideState.h"
#include "States/MovementStates/VaultState.h"
#include "States/MovementStates/WalkState.h"
#include "States/MovementStates/WallrunState.h"

// Sets default values for this component's properties
UStateMachine::UStateMachine()
{
	PrimaryComponentTick.bCanEverTick = true;
	StartState = EMovementState::Ems_Idle;
	CurrentState = StartState;
}

void UStateMachine::ChangeState(const EMovementState NewState)
{
	FString const NewString =  EnumToString(NewState);
	FString const CurrentString = EnumToString(CurrentState);
	if (NewState != CurrentState)
	{
		if (MoveStates.FindRef(CurrentState) != nullptr)
		{
			MoveStates.FindRef(CurrentState)->OnStateExit();
			if(MoveStates.FindRef(NewState) != nullptr)
			{
				MoveStates.FindRef(NewState)->OnStateEnter();
				CurrentState = NewState;
				Player->SetMoveState(CurrentState);
				UE_LOG(LogTemp, Warning, TEXT("changing from %s to %s"), *CurrentString, *NewString);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("State %s is null"),*NewString); 
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("State %s is null"),*CurrentString); 
		}
	}
}

// Called when the game starts
void UStateMachine::BeginPlay()
{
	Super::BeginPlay();
}

void UStateMachine::Setup(APlayerChar* InPlayer)
{
	StartState = EMovementState::Ems_Idle;
	Player = InPlayer;
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("UStateMachine: Owner is not APlayerChar!"));
	}
	SetupMoveStates();
}

void UStateMachine::SetupMoveStates()
{

	for (int i = 0; i < static_cast<int>(EMovementState::Ems_MaxNum); i++)
	{
		UMoveStateBase* State = GetStateClass(i);
		State->Machine = this;
		EMovementState StateKey = static_cast<EMovementState>(i);
		MoveStates.Add(StateKey, State);
	}
}

TObjectPtr<UMoveStateBase> UStateMachine::GetStateClass(int MoveState)
{
	TObjectPtr<UMoveStateBase> State = nullptr;
	const EMovementState EnumState = static_cast<EMovementState>(MoveState);
	switch (EnumState)
	{
	case EMovementState::Ems_Idle:
		State = NewObject<UIdleState>();
		break;
	case EMovementState::Ems_Crouch:
		State = NewObject<UCrouchState>();
		break;
	case EMovementState::Ems_Climb:
		State = NewObject<UClimbingState>();
		break;
	case EMovementState::Ems_Walk:
		State = NewObject<UWalkState>();
		break;
	case EMovementState::Ems_Jog:
		State = NewObject<UJogState>();
		break;
	case EMovementState::Ems_Run:
		State = NewObject<URunState>();
		break;
	case EMovementState::Ems_Dash:
		State = NewObject<UDashState>();
		break;
	case EMovementState::Ems_StopSlide:
		State = NewObject<UStopSlideState>();
		break;
	case EMovementState::Ems_ShortStop:
		State = NewObject<UShortStopState>();
		break;
	case EMovementState::Ems_Wallrun:
		State = NewObject<UWallrunState>();
		break;
	case EMovementState::Ems_Fall:
		State = NewObject<UFallState>();
		break;
	case EMovementState::Ems_Dodge:
		State = NewObject<UDodgeState>();
		break;
	case EMovementState::Ems_DoubleJump:
		State = NewObject<UDoubleJumpState>();
		break;
	case EMovementState::Ems_Vault:
		State = NewObject<UVaultState>();
		break;
	case EMovementState::Ems_Hang:
		State = NewObject<UHangState>();
		break;
	case EMovementState::Ems_Landing:
		State = NewObject<ULandingState>();
		break;
	case EMovementState::Ems_MaxNum:
		State = NewObject<UIdleState>();
		break;
	}
	State->Init(this, Player, EnumState);
	return State;
}


// Called every frame
void UStateMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GetCurrentState()->OnStateTick(DeltaTime);
}
