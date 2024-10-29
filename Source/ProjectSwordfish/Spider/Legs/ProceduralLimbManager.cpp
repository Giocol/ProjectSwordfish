#include "ProceduralLimbManager.h"

#include "Components/PoseableMeshComponent.h"
#include "ProjectSwordfish/Spider/Spider.h"

UProceduralLimbManager::UProceduralLimbManager() {
	PrimaryComponentTick.bCanEverTick = true;

}

void UProceduralLimbManager::BeginPlay() {
	Super::BeginPlay();

	if(auto o = Cast<ASpider>(GetOwner()))
		Mesh = o->GetMesh();
	FindLegs();
}

void UProceduralLimbManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	for(int i = 0; i < Legs.Num(); ++i) {
		//Legs[i].Rotate(1, FRotator(5 * DeltaTime, 5 * DeltaTime, 5 * DeltaTime));
		for(int j = Legs[i].Bones.Num() - 1; j >= 0; --j) {
			Legs[i].Bones[j].State += FRotator(5 * DeltaTime, 5 * DeltaTime, 5 * DeltaTime);
			//Legs[i].Rotate(j, FRotator(5 * DeltaTime, 5 * DeltaTime, 5 * DeltaTime));
		}
	}
	
	SetBonePositions(true);
}

bool UProceduralLimbManager::FindLegs() {
	if(!Mesh) return false;

	TArray<FName> Bones;
	Mesh->GetBoneNames(Bones);

	for(FName Bone : Bones) {
		auto boneAsString = Bone.ToString();
		auto templateName = EndEffectorsName.ToString();
		FString endName = "_end";
		auto foundTip = boneAsString.Find(*templateName, ESearchCase::CaseSensitive);
		auto foundEnd = boneAsString.Find(*endName, ESearchCase::CaseSensitive);

		if(foundTip != INDEX_NONE && foundEnd != INDEX_NONE) {
			Legs.Add(FLeg());
			Legs.Last().Bones.Add(FBoneTransformPair(Bone, Mesh->GetBoneRotationByName(Bone, EBoneSpaces::ComponentSpace)));
			Legs.Last().Target = Mesh->GetBoneLocationByName(Bone, EBoneSpaces::WorldSpace);
			FindHip(Bone);
		}
	}

	return true;
}

void UProceduralLimbManager::FindHip(FName From) {
	
	FName Parent = Mesh->GetParentBone(From);
	if(Parent.IsNone())
		return;
	Legs.Last().Bones.Add(FBoneTransformPair(Parent, Mesh->GetBoneRotationByName(Parent, EBoneSpaces::ComponentSpace)));
	
	auto boneAsString = Parent.ToString();
	auto templateName = HipJointsName.ToString();
	auto s = boneAsString.Find(*templateName, ESearchCase::CaseSensitive);
	if(s == INDEX_NONE)
		FindHip(Parent);
}

void UProceduralLimbManager::GetBoundsCornersWorld(FVector& BottomLeft, FVector& BottomRight, FVector& TopLeft,
	FVector& TopRight) const
{
	FVector Offset = GetOwner()->GetActorLocation();
	FRotator Rotation = GetOwner()->GetActorRotation();
	
	BottomLeft = Rotation.RotateVector(FVector(Bounds.Min.X, Bounds.Min.Y, 0) + Offset);
	BottomRight = Rotation.RotateVector(FVector(Bounds.Max.X, Bounds.Min.Y, 0) + Offset);
	TopLeft = Rotation.RotateVector(FVector(Bounds.Min.X, Bounds.Max.Y, 0) + Offset);
	TopRight = Rotation.RotateVector(FVector(Bounds.Max.X, Bounds.Max.Y, 0) + Offset);
}

TArray<FVector> UProceduralLimbManager::GetBoundsCornersWorldArray() const {
	FVector BottomLeft, BottomRight, TopLeft, TopRight;
	GetBoundsCornersWorld(BottomLeft, BottomRight, TopLeft, TopRight);
	return { BottomLeft, BottomRight, TopLeft, TopRight };
}

FVector UProceduralLimbManager::GetBoundsScale() const {
	return FVector(Bounds.Max.X - Bounds.Min.X, Bounds.Max.Y - Bounds.Min.Y, 0);;
}


void UProceduralLimbManager::SetBonePositions(bool bDraw) {
	for (auto Leg : Legs) {
		for (int i = 0; i < Leg.Bones.Num(); ++i) {

			Mesh->SetBoneRotationByName(Leg.Bones[i].Name, Leg.Bones[i].State, EBoneSpaces::ComponentSpace);
			
			if(bDraw && i < Leg.Bones.Num() - 1) {
				FTransform T = GetOwner()->GetActorTransform();
				FVector Current = Mesh->GetBoneLocationByName(Leg.Bones[i].Name, EBoneSpaces::WorldSpace);
				FVector Next =  Mesh->GetBoneLocationByName(Leg.Bones[i+1].Name, EBoneSpaces::WorldSpace);
				DrawDebugLine(GetWorld(), Current, Next, FColor::Blue, false, -1, -1);
				
			} else if (bDraw) {
				DrawDebugPoint(GetWorld(), Leg.Target, 15.f, FColor::Green, false, -1, -1);
			}
		}
	}
}

