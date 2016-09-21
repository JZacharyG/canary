#include "test.h"

#define FALSE 0
#define TRUE !0

int main(int argc, char** argv)
{
	// Petersen has K_5 and K_3,3
	minorTest("IheA@GUAo", "D~{", TRUE);
	minorTest("IheA@GUAo", "EFz_", TRUE);
	
	minorTestFile("tst/4c-K25mf-13.g6", "F]rE?", FALSE);
	minorTestFile("tst/4c-K25mf-14.g6", "F]rE?", FALSE);
	minorTestFile("tst/4c-K25mf-15.g6", "F]rE?", FALSE);
	minorTestFile("tst/4c-K25mf-16.g6", "F]rE?", FALSE);
	minorTestFile("tst/4c-K25mf-17.g6", "F]rE?", FALSE);
	
	minorTestFile("tst/p4cDW6mf20.g6", "G^vMNC", FALSE);
	minorTestFile("tst/p4cDW6mf20.g6", "D~{", FALSE);
	minorTestFile("tst/p4cDW6mf20.g6", "EFz_", FALSE);
	
	minorTestFile("tst/4c-planar-30.g6", "D~{", FALSE);
	minorTestFile("tst/4c-planar-30.g6", "EFz_", FALSE);
	
	minorTestFile("tst/4c-almost-planar-30.g6", "D~{", TRUE);
	minorTestFile("tst/4c-almost-planar-30.g6", "EFz_", TRUE);
	
	minorTestFile("tst/2c-planar-50.g6", "D~{", FALSE);
	minorTestFile("tst/2c-planar-50.g6", "EFz_", FALSE);
	
	minorTestFile("tst/2c-almost-planar-50.g6", "D~{", TRUE);
	minorTestFile("tst/2c-almost-planar-50.g6", "EFz_", TRUE);
}