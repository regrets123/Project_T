// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Characters/PlayerStates.h"
#include "Components/ActorComponent.h"
#include "States/MovementStates/MoveStateBase.h"
#include "StateMachine.generated.h"

enum class EMovementState : uint8;
class APlayerChar;

UCLASS(ClassGroup = (Custom), Blueprintable, BlueprintType)
class PROJECTT_API UStateMachine : public UActorComponent
{
	GENERATED_BODY()

public:	

	UStateMachine();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Setup(APlayerChar* InPlayer);
	UFUNCTION(BlueprintCallable)
	void ChangeState(EMovementState NewState);
	UFUNCTION(BlueprintCallable)
	UMoveStateBase* GetCurrentState() { return MoveStates.FindRef(CurrentState); }
	EMovementState GetCurrentState() const { return CurrentState;}
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	EMovementState GetPreviousState() const { return PreviousState;}
	TMap<EMovementState, TObjectPtr<UMoveStateBase>>& GetStates() { return MoveStates;}
	
protected:

	virtual void BeginPlay() override;
private:
	
	void SetupMoveStates();
	TObjectPtr<UMoveStateBase> GetStateClass (int MoveState);

	UPROPERTY()
	APlayerChar* Player;
	UPROPERTY()
	TMap<EMovementState, TObjectPtr<UMoveStateBase>> MoveStates;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	EMovementState StartState = EMovementState::Ems_Idle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	EMovementState CurrentState = EMovementState::Ems_Idle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	EMovementState PreviousState;
	
};
