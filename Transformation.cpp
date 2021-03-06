#include "Transformation.h"
#include <cmath>
#include <vector>

void Transformation::cancelGravity(std::vector<double> realAcceleration) {
	realAcceleration[2] -= 9.81;
}

double Transformation::getRotationAngle(double angularVelocity, double dt) {
	currentAngle += angularVelocity * dt;
	return currentAngle;
}

// use quaternion rotation instead? more efficient
void Transformation::getTransformationMatrix(double gyroXrate, double gyroYrate, double gyroZrate, double dt) {
	double alpha = getRotationAngle(gyroXrate, dt);
	double beta = getRotationAngle(gyroYrate, dt);
	double gamma = getRotationAngle(gyroZrate, dt);
	matrix[0][0] = cos(beta)*cos(gamma);
	matrix[0][1] = -sin(gamma);
	matrix[0][2] = sin(beta);
	matrix[1][0] = sin(gamma);
	matrix[1][1] = cos(alpha)*cos(gamma);
	matrix[1][2] = -sin(alpha);
	matrix[2][0] = -sin(beta);
	matrix[2][1] = sin(alpha);
	matrix[2][2] = cos(alpha)*cos(beta);
}

std::vector<double> Transformation::transformToNormal(double kalAngleX, double kalAngleY, double kalAngleZ, double gyroXrate, double gyroYrate, double gyroZrate, double dt) {
	getTransformationMatrix(gyroXrate, gyroYrate, gyroZrate, dt);
	std::vector<double> absoluteAcceleration(3);
	for (int i = 0; i < 3; i++) {
		absoluteAcceleration[i] = kalAngleX * matrix[0][i]
			+ kalAngleY * matrix[1][i]
			+ kalAngleZ * matrix[2][i];
	}
	cancelGravity(absoluteAcceleration);
	return absoluteAcceleration;
}

double Transformation::getAbsoluteVelocity(double kalAngleX, double kalAngleY, double kalAngleZ, double gyroXrate, double gyroYrate, double gyroZrate, double dt, int axis) {
	std::vector<double> absoluteAcceleration = transformToNormal(kalAngleX, kalAngleY, kalAngleZ, gyroXrate, gyroYrate, gyroZrate, dt);
/*	velocityX += absoluteAcceleration[0] * dt;
	velocityY += absoluteAcceleration[1] * dt;
	velocityZ += absoluteAcceleration[2] * dt;*/
	velocityX = absoluteAcceleration[0] * dt;
	velocityY = absoluteAcceleration[1] * dt;
	velocityZ = absoluteAcceleration[2] * dt;
	
	switch (axis) {
	case 1:
		return velocityX;
	case 2:
		return velocityY;
	case 3:
		return velocityZ;
	default:
		return 0;
	}
}

double Transformation::getAbsolutePosition(double kalAngleX, double kalAngleY, double kalAngleZ, double gyroXrate, double gyroYrate, double gyroZrate, double dt, int axis) {
	std::vector<double> absoluteAcceleration = transformToNormal(kalAngleX, kalAngleY, kalAngleZ, gyroXrate, gyroYrate, gyroZrate, dt);
	velocityX += absoluteAcceleration[0] * dt;
	velocityY += absoluteAcceleration[1] * dt;
	velocityZ += absoluteAcceleration[2] * dt;
/*	posX += velocityX * dt;
	posY += velocityY * dt;
	posZ += velocityZ * dt;*/
	posX = velocityX * dt;
	posY = velocityY * dt;
	posZ = velocityZ * dt;
	
	switch (axis) {
	case 1:
		return posX;
	case 2:
		return posY;
	case 3:
		return posZ;
	default:
		return 0;
	}
}
