#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FuzzyBrainComponent.generated.h"


class UBlackboardComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTSWORDFISH_API UFuzzyBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFuzzyBrainComponent();

	void SetBlackboardRef(UBlackboardComponent* Blackboard) { BlackboardComponentRef = Blackboard; };

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBlackboardComponent* BlackboardComponentRef = nullptr;
};
