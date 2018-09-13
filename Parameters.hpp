#pragma once

class Parameters
{
public:

	bool Threshold = false;

	bool EPX = false;
	bool EPXB = false;
	bool EPXC = false;
	bool Scale3X = false;
	bool Magnify = false;
	bool Nearest = false;
	bool Bilinear = false;
	bool BilinearPlus = false;
	bool BilinearPP = false;
	bool HQX = false;
	bool HQ2XN = false;
	bool LQX = false;
	bool LQ2XN = false;
	bool Eagle = false;
	bool Eagle3XB = false;
	bool Super2X = false;
	bool Ultra2X = false;
	bool DES = false;
	bool DES2X = false;
	bool TV = false;
	bool RGB = false;
	bool Horiz = false;
	bool VertScan = false;
	bool AdvInterp = false;
	bool AmScale = false;
	bool XBR = false;
	bool SCL2X = false;
	bool SAI2X = false;
	bool XBRZ = false;
	bool SuperSAI = false;
	bool SuperEagle = false;
	bool ReverseAA = false;
	bool Kuwahara = false;
	bool Flip = false;
	bool Rotate = false;

	int Magnification = 1;
	
	int FlipH = 0;
	int FlipV = 1;
	int FlipDir = 0;

	int Rot90 = 0;
	int Rot180 = 1;
	int Rot270 = 2;
	int RotAngle = 0;
	
	int Window = 5;
};
