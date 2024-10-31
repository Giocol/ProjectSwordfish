// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Limb.h"

#include "ProceduralLimbManager.generated.h"


UCLASS(ClassGroup=(Locomotion), meta=(BlueprintSpawnableComponent))
class PROJECTSWORDFISH_API UProceduralLimbManager : public UActorComponent
{
	GENERATED_BODY()


public:
	UProceduralLimbManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void FindLegs();

	void temp();
private:
	void RecurseToHip(FName From);

	//bool TryFindNewLimbTarget(FVector From, FVector& To);
	

protected:
	
	UPROPERTY(EditDefaultsOnly)
		FName HipJointsName = "Hip";
	UPROPERTY(EditDefaultsOnly)
		FName EndEffectorsName = "Tip";

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TArray<FLeg> Legs;
	
	class UPoseableMeshComponent* Mesh;
};


