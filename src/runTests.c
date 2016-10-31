#include "test.h"
#include "canary.h"

int main(int argc, char** argv)
{
	// Petersen has K_5 and K_3,3
	minorTest("IheA@GUAo", "D~{", TRUE);
	minorTest("IheA@GUAo", "EFz_", TRUE);
	
	// a minor in which one branch set is forced to induce a cycle
	minorTest("WhEK?E@O??g@G???c?G_???CO?GO????CG?@@??????__?@", "L{eCKA@_C?o?_@", TRUE);
	
	minorTest("G^vMNC", "G^vMNC", TRUE);
	
	// 6-regular 15-vertex graph is a minor of itself
	minorTest("N??CBravTmNo~?VgTs?", "NsaCwD?QZJm[NWNKNg?", TRUE);
	
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
	
	return 0;
}
