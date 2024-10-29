#include "Limb.h"

//
// void FLeg::Rotate(int Id, FRotator Value) {
// 	if(Id < 1 || Id > Bones.Num() - 1) {
// 		UE_LOG(LogTemp, Error, TEXT("Tried rotating bone outside of array"))
// 		return;
// 	}
// 	FVector Offset = Bones[Id-1].State.GetLocation();
// 	Offset -= Bones[Id].State.GetLocation();
// 	Offset = Value.RotateVector(Offset);
// 	Offset += Bones[Id].State.GetLocation();
//
// 	Bones[Id].State.SetRotation((Bones[Id - 1].State.Rotator() + Value).Quaternion());
// 	Bones[Id - 1].State.SetLocation(Offset);
// 	
// 	Rotate(Id - 1, Value);
// }
