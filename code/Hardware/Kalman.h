#ifndef __KALMAN_H
#define __KALMAN_H

typedef struct{
	float LastP;
	float NewP;
	float Out;
	float Kg;
	float Q;
	float R;
} KalmanFilter;

void kalmanfiter(KalmanFilter *EKF,float input);

#endif
