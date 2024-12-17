#pragma once

namespace MathUtils {
	static float AngleLerp(float Current, float Target, float Alpha, float HalfCircleAngle) {
		float Delta = Target - Current;

		if(Delta > HalfCircleAngle) {
			Delta -= HalfCircleAngle * 2;
		} else if(Delta < -HalfCircleAngle) {
			Delta += HalfCircleAngle * 2;
		}

		return Current + Delta * Alpha;
	}
	
	static float DegAngleLerp(float Current, float Target, float Alpha) {
		return AngleLerp(Current, Target, Alpha, 180);
	}
	
	static float RadAngleLerp(float Current, float Target, float Alpha) {
		return AngleLerp(Current, Target, Alpha, PI);
	}
	
	static FVector2d PolarLerp(FVector2d Current, FVector2d Target, float Alpha) {
		float LerpedY = RadAngleLerp(Current.Y, Target.Y, Alpha);
		float LerpedX = RadAngleLerp(Current.X, Target.X, Alpha);
	
		return FVector2D(LerpedX, LerpedY);
	}

	static float GetAngleBetweenVectorsRads(const FVector& Current, const FVector& Target) {
		return acos(FVector::DotProduct(Current, Target));
	}
};
