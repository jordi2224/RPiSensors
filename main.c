#include "wiringPiI2C.h"
#include "wiringPi.h"
#include <stdio.h>
#include <stdint.h>

//Global variables

	char ACC_ADD = 0x53;
		char ACC_X0 = 0x32;
		char ACC_X1 = 0x33;
		char ACC_Y0 = 0x34;
		char ACC_Y1 = 0x35;
		char ACC_Z0 = 0x36;
		char ACC_Z1 = 0x37;
	char GYRO_ADD = 0x68;
		char GYRO_X0 = 0x1D;
		char GYRO_X1 = 0x1E;
		char GYRO_Y0 = 0x1F;
		char GYRO_Y1 = 0x20;
		char GYRO_Z0 = 0x21;
		char GYRO_Z1 = 0x22;
		char GYRO_T0 = 0x1B;
		char GYRO_T1 = 0x1C;
	char COMP_ADD = 0x1E;
		char COMP_X0 = 0x03;
		char COMP_X1 = 0x04;
		char COMP_Y0 = 0x05;
		char COMP_Y1 = 0x06;
		char COMP_Z0 = 0x07;
		char COMP_Z1 = 0x08;

	int gyrox_offset = 0;
	int gyroy_offset = 0;
	int gyroz_offset = 0;




void FetchAcc(short acc_raw[]);	//Fetches raw data from ADXL345 accelerometer

void FetchComp(short comp_raw[]);	//Fetches raw data from HMC5883L magnetometer

void FetchGyro(short gyro_raw[]);	//Fetches raw dara from ITG3200

void CaliGyro();	//Samples and calibrates the gyroscope by modifying offsets (SENSORS MUST REMAIN STILL !!!)

void SetupSensors();





int main (void){

	short acc_raw[3];

	short comp_raw[3];

	short gyro_raw[3];

	SetupSensors();








	return 0;
}










//This will have it's own header in next update

void FetchAcc(short acc_raw[]){
	int fd = wiringPiI2CSetup(ACC_ADD);
	acc_raw[0] = wiringPiI2CReadReg16(fd, ACC_X0);
	acc_raw[1] = wiringPiI2CReadReg16(fd, ACC_Y0);
	acc_raw[2] = wiringPiI2CReadReg16(fd, ACC_Z0);

}

void FetchComp(short comp_raw[]){

	int fd = wiringPiI2CSetup(COMP_ADD);
	comp_raw[0] = wiringPiI2CReadReg16(fd, COMP_X0);
	comp_raw[1] = wiringPiI2CReadReg16(fd, COMP_Y0);
	comp_raw[2] = wiringPiI2CReadReg16(fd, COMP_Z0);

}

void FetchGyro(short gyro_raw[]){

	int fd = wiringPiI2CSetup(GYRO_ADD);
	int meas;
	int i;
	int n=10; //Number of samples to average out


	meas=0;
	for(i=0; i<n; i++){		//X value calculator (average of n samples)
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_X0);
	}
	gyro_raw[0] = (meas/n) + gyrox_offset;

	meas=0;
	for(i=0; i<n; i++){		//Y value calculator (average of n samples)
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_Y0);
	}
	gyro_raw[1] = (meas/n) + gyroy_offset;

	meas=0;
	for(i=0; i<n; i++){		//Z value calculator (average of n samples)
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_Z0);
	}
	gyro_raw[2] = (meas/n) + gyroz_offset;


}

void CaliGyro(){

	int i;
	int n=1000;	//Number of samples to average out
	int meas;

	int fd = wiringPiI2CSetup(GYRO_ADD);

	meas=0;
	for(i=0; i<n; i++){		//X offset calculator (average of n samples)
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_X0);
	}
	gyrox_offset = -(meas/n);

	meas=0;
	for(i=0; i<n; i++){		//Y offset calculator (average of n samples)
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_Y0);
	}
	gyroy_offset = -(meas/n);

	meas=0;
	for(i=0; i<n; i++){		//Z offset calculator (average of n samples)
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_Z0);
	}
	gyroz_offset = -(meas/n);

}

void SetupSensors(){

	//Sensor setup:

		//Acc:
		int fd1 = wiringPiI2CSetup(ACC_ADD);
		wiringPiI2CWriteReg8(fd1, 0x2D, 0x08);	//Sets accelerometer to 2g range

		//Gyro:
		int fd2 = wiringPiI2CSetup(GYRO_ADD);
		wiringPiI2CWriteReg8(fd2, 0x16, 0x18);	//Sets gyro to 8kHz Sample Rate
		CaliGyro();	//Calibrates gyro

		//Comp
		int fd3 = wiringPiI2CSetup(COMP_ADD);
		wiringPiI2CWriteReg8(fd3, 02, 0x00);	//Sets comp to constant measurement


}
