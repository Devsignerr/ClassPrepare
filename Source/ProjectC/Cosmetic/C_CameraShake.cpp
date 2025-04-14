
#include "C_CameraShake.h"

UC_CameraShake::UC_CameraShake()
{
	OscillationDuration = 0.2f;     
	OscillationBlendInTime = 0.05f;  
	OscillationBlendOutTime = 0.05f;
	
	LocOscillation.X.Amplitude = 5.0f; 
	LocOscillation.X.Frequency = 50.0f;
	LocOscillation.Y.Amplitude = 5.0f; 
	LocOscillation.Y.Frequency = 50.0f;
	LocOscillation.Z.Amplitude = 5.0f; 
	LocOscillation.Z.Frequency = 50.0f;
}
