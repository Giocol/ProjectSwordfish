#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.generated.h"

class UWaypointHolderComponent;
class AEnemyAIController;
enum EEnemyState : uint8;
class UFuzzyBrainComponent;
class UHearingComponent;
class USightComponent;

UCLASS()
class PROJECTSWORDFISH_API AEnemyBase : public APawn {
	GENERATED_BODY()

public:
	AEnemyBase();

	FORCEINLINE void RegisterSight(USightComponent* Component) { SightComponents.AddUnique(Component); }

	FORCEINLINE void RegisterHearing(UHearingComponent* Component) {
		if(!HearingComponent)
			HearingComponent = Component;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf=Target), Category = "AI|Perception")
		static bool IsActorInView(AEnemyBase* Target, AActor* Actor, float& SignalStrength);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf=Target), Category = "AI|Perception")
		static bool IsLocationInView(AEnemyBase* Target, FVector Location, float Tolerance, float& SignalStrength);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf=Target),  Category = "AI|Perception") 
		static struct FPerceptionSignal GetVisionSignalToPlayer(AEnemyBase* Target);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf=Target), Category = "AI|Perception") 
		static TArray<struct FPerceptionSignal> GetVisionSignalsOfClass(AEnemyBase* Target, TSubclassOf<AActor> Class);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf=Target), Category = "AI|Perception") 
		static TArray<struct FPerceptionSignal> GetVisionSignals(AEnemyBase* Target);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf=Target), Category = "AI|Perception") 
		static FPerceptionSignal GetLastHearingSignal(AEnemyBase* Target);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf=Target), Category = "AI|Perception") 
		static bool HasNewSignalBeenHeard(AEnemyBase* Target);

	UFUNCTION(BlueprintCallable)
		void SaveState();
	UFUNCTION(BlueprintCallable)
		void LoadState();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|State")
		EEnemyState GetCurrentState() const { return CurrentState; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
		UFuzzyBrainComponent* GetBrain() const;

	UFUNCTION(BlueprintCallable, Category = "AI|State")
		void SetCurrentState(const TEnumAsByte<EEnemyState> NewState) { CurrentState = NewState; }
	
	UFUNCTION(BlueprintCallable)
		void SetSpeedMultiplier(float Multiplier) { /*GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * Multiplier; OnSpeedChanged(BaseSpeed * Multiplier);*/}
	
	UFUNCTION(BlueprintCallable)
		void ResetToBaseSpeed() { /*GetCharacterMovement()->MaxWalkSpeed = BaseSpeed; OnSpeedChanged(BaseSpeed);*/}

	UFUNCTION(BlueprintImplementableEvent)
		void OnSpeedChanged(float speed);
	
	UFUNCTION(BlueprintCallable)
		void Die(const AActor* Killer);
	//UFUNCTION(BlueprintNativeEvent)
	//	void OnDeath();
	
	UHearingComponent* GetHearingComponent() const { return HearingComponent; };

	//UFUNCTION(BlueprintCallable)
	//	FVector GetNextWaypointLocation();
protected:
	TArray<AActor*> GetVisibleActorCandidatesOfClass(TSubclassOf<AActor> Class) const;
	TArray<AActor*> GetVisibleActorCandidates() const;
	
	virtual void BeginPlay() override;

private:	//EDITOR ONLY functions
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Waypoints")
		void UpdateNavigationArrays() const;

	UFUNCTION(CallInEditor, Category = "Waypoints")
		void DeleteAllWaypoints() const;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; };
#endif
public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waypoints")
		int NumberOfIdleWaypoints;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waypoints")
		UWaypointHolderComponent* IdleWaypointHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waypoints")
		int NumberOfSuspiciousWaypoints;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waypoints")
		UWaypointHolderComponent* SuspiciousWaypointHolder;
	
	UPROPERTY()
		TArray<USightComponent*> SightComponents;

	UPROPERTY()
		UHearingComponent* HearingComponent;

	UPROPERTY(EditInstanceOnly)
		AActor* DebugActor;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
		TEnumAsByte<EEnemyState> CurrentState;

	UPROPERTY()
		AEnemyAIController* EnemyController;

	float BaseSpeed;
};
