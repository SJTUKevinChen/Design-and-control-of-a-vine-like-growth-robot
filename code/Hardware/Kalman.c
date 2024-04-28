#include "stm32f10x.h"                  // Device header
#include "kalman.h"

void kalmanfiter(KalmanFilter *EKF,float input)
{
	EKF->NewP = EKF->LastP + EKF->Q;
	EKF->Kg = EKF->NewP / (EKF->NewP + EKF->R);
	EKF->Out = EKF->Out + EKF->Kg * (input - EKF->Out);
	EKF->LastP = (1 - EKF->Kg) * EKF->NewP;
}
