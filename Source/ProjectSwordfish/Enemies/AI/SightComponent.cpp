#include "SightComponent.h"

#include "DetectionModifier.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectSwordfish/Enemies/EnemyBase.h"


USightComponent::USightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	OnComponentBeginOverlap.AddDynamic(this, &ThisClass::BeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &ThisClass::EndOverlap);
	SetHiddenInGame(true);
	SetDefaultCollisionResponse();
	UpdateVisionMeshScale();
}

bool USightComponent::IsActorVisible(AActor* Actor, float& SignalStrength) const {
	SignalStrength = 0.f;
	if (!Actor || !ActorsInVisionCone.Contains(Actor))
		return false;
	//if(!IsLocationInVisionCone(Actor->GetActorLocation()))
	//	return false;
	FVector ViewTarget = Actor->GetActorLocation();
	if(auto a = Cast<ACharacter>(Actor))
		ViewTarget += FVector::UpVector * a->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2;
	FHitResult Hit = TraceTo(ViewTarget);
	
		
	if(Hit.GetActor()) {
		FColor DebugDrawColor = FColor::Red;
		if (Hit.GetActor() == Actor) {
			SignalStrength = EvaluateSignalStrength(Actor);
			DebugDrawColor = FColor::Green;
		}
		DrawDebugLine(GetWorld(), GetComponentLocation(), Hit.Location, DebugDrawColor);
		DrawDebugPoint(GetWorld(), Hit.Location, 10.f, DebugDrawColor);
		return true;
	}
	else return false;
}

bool USightComponent::IsLocationVisible(FVector Location, float Tolerance, float& SignalStrength) const {
	SignalStrength = 0.f;
	if(!IsLocationInVisionCone(Location))
		return false;

	FHitResult Hit = TraceTo(Location);
	if(!Hit.GetActor() || FVector::Distance(Location, Hit.Location) <= Tolerance) {
		SignalStrength = EvaluateSignalStrength(Location);
		return true;
	} else 
		return false;
}

TArray<AActor*> USightComponent::GetAllVisibleActors() const {
	TArray<AActor*> Result;
	Result.Reserve(ActorsInVisionCone.Num());
	for (auto a : ActorsInVisionCone) {
		float discard;
		if(IsActorVisible(a, discard))
			Result.Add(a);
	}
	return Result;
}

bool USightComponent::IsLocationInVisionCone(FVector Location) const noexcept {
	FVector ToActor = Location;
	auto cosAngle = GetForwardVector().Dot(ToActor.GetSafeNormal());
	if(cosAngle < FMath::Cos(ViewAngle) || ToActor.Length() > ViewDistance)
		return false;
	else
		return true;
}

void USightComponent::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
		return;
	
	if(!OtherActor->GetComponentsCollisionResponseToChannel(TraceChannel) == ECR_Ignore)
		ActorsInVisionCone.AddUnique(OtherActor);
}

void USightComponent::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (ActorsInVisionCone.Contains(OtherActor))
		ActorsInVisionCone.Remove(OtherActor);
}


void USightComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateVisionMeshScale();
	auto OwningAI = Cast<AEnemyBase>(GetOwner());
	if (OwningAI)
		OwningAI->RegisterSight(this);
}

void USightComponent::UpdateVisionMeshScale() {
	float width = FMath::Tan(FMath::DegreesToRadians(ViewAngle));
	SetRelativeScale3D(0.01 * ViewDistance * FVector(1.0, width, width));
}

void USightComponent::SetDefaultCollisionResponse()
{
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(TraceChannel, ECR_Overlap);
	SetCollisionObjectType(ECC_WorldDynamic);

	CanCharacterStepUpOn = ECB_No;
}

float USightComponent::EvaluateSignalStrength(float Distance) const {
	float Result = DetectionRate;
	if(DetectionFalloffPower != 0)
		Result /= FMath::Pow(Distance / 100, DetectionFalloffPower);
	return Result;
}
float USightComponent::EvaluateSignalStrength(FVector Location) const {
	return EvaluateSignalStrength(FVector::Distance(GetComponentLocation(), Location));
}

float USightComponent::EvaluateSignalStrength(AActor* Actor) const {
	float Result = EvaluateSignalStrength(Actor->GetActorLocation());
	if(const auto DetectionModifier = Actor->GetComponentByClass(UDetectionModifier::StaticClass()))
		Result *= Cast<UDetectionModifier>(DetectionModifier)->DefaultSignalModifier;
	return Result;
}

FHitResult USightComponent::TraceTo(FVector Location) const {
	TArray<AActor*> ignores;
	ignores.Init(GetOwner(), 1);
	FHitResult Hit;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetComponentLocation(), Location,
										  UEngineTypes::ConvertToTraceType(TraceChannel), false, ignores,
										  EDrawDebugTrace::None, Hit, true);
	return Hit;
}

#if WITH_EDITOR
void USightComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UpdateVisionMeshScale();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif