#pragma once

namespace MathUtils {
	static float DegAngleLerp(float Start, float Target, float Alpha) {
		float Delta = Target - Start;

		if(Delta > 180) {
			Delta -= 360;
		} else if(Delta < -180) {
			Delta += 360;
		}

		return Start + Delta * Alpha;
	}	 
};
