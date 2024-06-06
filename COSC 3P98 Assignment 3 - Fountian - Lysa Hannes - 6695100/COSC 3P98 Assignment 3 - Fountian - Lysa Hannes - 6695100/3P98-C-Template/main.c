/* 
* Lysa Hannes
* 6695100
* COSC 3P98 Assignemnt 3 Question 1
* I used this example from the course website to help me with perspective, and button pressing: http://www.cosc.brocku.ca/Offerings/3P98/course/lectures/3d_perspective/cube_persp.c
* I used this example from the course website to help me with making the cube: http://www.cosc.brocku.ca/Offerings/3P98/course/OpenGL/3P98Examples/OpenGLExamples/rotate.c
* I used the first post from this website to find the clock function in c: https://stackoverflow.com/questions/10192903/time-in-milliseconds-in-c
* I used this website fo figure out how to do randome double values: https://stackoverflow.com/questions/63981013/generating-random-double-between-1-and-100-in-c
* I used this website to help me get the single fire mode to not press the key over and over: https://stackoverflow.com/questions/39561997/glut-holding-a-key-down
* 
* What the keys do:
* q - quits the program
* w - resets everything
* e - toggles the hole in the ground
* r - friction on the ground
* t - toggles gouraud shading
* y - toggles polygon, wire, and points for the cubes
* 
* a - toggles slow motion
* s - toggles number of particles
* d - turns off the fountain
* f - when pressed particles fire, when released particles stop firing
* g - single shot mode
* 
* x - toggles different sizes
* c - toggles different colours
* v - "spray" mode, toggles between low spray (default, slightly randomness to initial direction) to high spray (wildly random)
*/
#define FILENAME "img.tif"

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <glut.h>
#include <FreeImage.h>
#include <time.h>
#include <windows.h> 

int max = 35; //the x and z values for the floor
int counter = 0; //for knowing which particle number we are on
double currentGravity = 0.00005; //for the gravity
clock_t start, stop, nextParticle, startNextParticle; //the timers
int firstClock = 0; //used for tracking if it time to restart the animation speed clocks (start and stop)
int secondClock = 0; //used for tracking if it time to restart the next particle speed clocks (nextParticle and startNextParticle)
int numParticles = -1; //used for spacing out the time between each particles creation
int speed = 0; //speed of the animation
int fireingSpeed = 1; //for toggling number of partices, 1-lots, 50-medium, 50-a little
int colourToggle = 0; //for toggling the colorful mode
int sizeToggle = 0; //for toggling the different size mode
int nextSize = 0; //0-normal, 1-medium, 2-large
int frictionToggle = 0; //for toggling the friction mode 
int nextColor = 0; //for knowing what the next color to use is
int totalNumParticles = 1000; //used for if I want to change the number of max particles easier, there are still some other things that need to be changed though
int fountainOff = 0; // for turning off and on the fountain, 1 - fountain off, 0 - fountain on
int toggleHole = 0; //for toggling the hole being open
double spreyModeVal = 0.01;//for spreyMode
int singleFire = 0; //for fireing a single cube
int temppCounter = 0; //for fireing a single cube
int trackSingle = 0; //for fireing a single cube
int gouraud = 0; //for toggling gouraud shading
int togglePlotting = 0; //for toggling polygon, lines, and points drawing, 0 - polygon, 1 - wireframe, 2 - points
int toggleObsticles = 0; //toggles obsticles

/*for particle list
* max 25 particles
* 0 - state - 0 if not on screen, 1 if moving, 2 if its done moving, 3 if it needs to delete
* 1 - fireing speed
* 2 - divide bounce
* 3 - r1 value (darkest)
* 4 - g1 value (darkest)
* 5 - b1 value (darkest)
* 6 - r2 value 
* 7 - g2 value
* 8 - b2 value
* 9 - r3 value (lightest)
* 10 - g3 value (lightest)
* 11 - b3 value (lightest)
*/
int particles[1000][14] = {0};

double particlesDir[1000][3] = {0}; //For particle direction, [0] - x, [1] - y, [2] - z

//values for the fountain
double a[3] = { 2,4,2 }, b[3] = { 2,0,2 }, c[3] = { -2,0,2 }, d[3] = { -2,4,2 },
e[3] = { 2,4,-2 }, f[3] = { 2,0,-2 }, g[3] = { -2,0,-2 }, h[3] = { -2,4,-2 };
double aa[3] = { 1,8,1 }, bb[3] = { 1,4,1 }, cc[3] = { -1,4,1 }, dd[3] = { -1,8,1 },
ee[3] = { 1,8,-1 }, ff[3] = { 1,4,-1 }, gg[3] = { -1,4,-1 }, hh[3] = { -1,8,-1 };

//values for the floor
double FloorA[3] = { -35,0,35 }, FloorB[3] = { -35,0,-35 }, FloorC[3] = { 35,0,-35 }, FloorD[3] = { 35,0,35 },
FloorE[3] = { 35,-50,-35 }, FloorF[3] = { 35,-50,35 }, FloorG[3] = { -35,-50,35 };

//values for floor with hole in the ground
double FloorA1[3] = { -35,0,35 }, FloorB1[3] = { 0,0,35 }, FloorC1[3] = { 0,0,25 }, FloorD1[3] = { -35,0,25 },
FloorE1[3] = { -35,-50,35 }, FloorF1[3] = { 0,-50,35 }, FloorG1[3] = { 0,-50,25 }, FloorH1[3] = { -35,-50,25 },

FloorA2[3] = { -35,0,25 }, FloorB2[3] = { -35,0,-35 }, FloorC2[3] = { -17,0,-35 }, FloorD2[3] = { -17,0,25 },
FloorE2[3] = { -35,-50,25 }, FloorF2[3] = { -35,-50,-35 }, FloorG2[3] = { -17,-50,-35 }, FloorH2[3] = { -17,-50,25 },

FloorA3[3] = { -17,0,-35 }, FloorB3[3] = { 35,0,-35 }, FloorC3[3] = { 35,0,10 }, FloorD3[3] = { -17,0,10 },
FloorE3[3] = { -17,-50,-35 }, FloorF3[3] = { 35,-50,-35 }, FloorG3[3] = { 35,-50,10 }, FloorH3[3] = { -17,-50,10 },

FloorA4[3] = { 35,0,10 }, FloorB4[3] = { 35,0,35 }, FloorC4[3] = { 0,0,35 }, FloorD4[3] = { 0,0,10 },
FloorE4[3] = { 35,-50,10 }, FloorF4[3] = { 35,-50,35 }, FloorG4[3] = { 0,-50,35 }, FloorH4[3] = { 0,-50,10 };

//values for new cube particle
double newCubeA[3] = { 1,1,1 }, newCubeB[3] = { 1,-1,1 }, newCubeC[3] = { -1,-1,1 }, newCubeD[3] = { -1,1,1 },
newCubeE[3] = { 1,1,-1 }, newCubeF[3] = { 1.0,-1.0,-1.0 }, newCubeG[3] = { -1,-1,-1 }, newCubeH[3] = { -1,1,-1 };

//values for medium cubes
double newCubeAM[3] = { 2,2,2 }, newCubeBM[3] = { 2,-2,2 }, newCubeCM[3] = { -2,-2,2 }, newCubeDM[3] = { -2,2,2 },
newCubeEM[3] = { 2,2,-2 }, newCubeFM[3] = { 2.0,-2.0,-2.0 }, newCubeGM[3] = { -2,-2,-2 }, newCubeHM[3] = { -2,2,-2 };

//values for large cubes
double newCubeAL[3] = { 3,3,3 }, newCubeBL[3] = { 3,-3,3 }, newCubeCL[3] = { -3,-3,3 }, newCubeDL[3] = { -3,3,3 },
newCubeEL[3] = { 3,3,-3 }, newCubeFL[3] = { 3.0,-3.0,-3.0 }, newCubeGL[3] = { -3,-3,-3 }, newCubeHL[3] = { -3,3,-3 };

// actual values for new cube particle, [0] - x, [1] - y, [2] - z
double newCubeAVec[1000][3] = { 1,1,1 }, newCubeBVec[1000][3] = { 1,-1,1 }, newCubeCVec[1000][3] = { -1,-1,1 }, newCubeDVec[1000][3] = { -1,1,1 },
newCubeEVec[1000][3] = { 1,1,-1 }, newCubeFVec[1000][3] = { 1.0,-1.0,-1.0 }, newCubeGVec[1000][3] = { -1,-1,-1 }, newCubeHVec[1000][3] = { -1,1,-1 };

//for the point mode
double pointTemp[3] = { 0,0,0 }; //[0] - x, [1] - y, [2] - z

//used for bouncing to set the y back to original value
double originalY = 0.05;

//initializes the particles direction
void particleDirection() {
	particlesDir[counter][0] = ((double)rand() / RAND_MAX) * (spreyModeVal - (-spreyModeVal)) + (-spreyModeVal);
	particlesDir[counter][1] = originalY;
	particlesDir[counter][2] = ((double)rand() / RAND_MAX) * (spreyModeVal - (-spreyModeVal)) + (-spreyModeVal);
}

//deletes the disapeared cubes from the array to make room for more to get shot
void deleteParticle() {
	for (int i = counter; i < totalNumParticles-1;i++) {
		for (int j = 0; j < 14;j++) {
			particles[i][j] = particles[i+1][j];
		}
	}

	for (int i = counter; i < totalNumParticles-1;i++) {
		for (int j = 0; j < 3;j++) {
			particlesDir[i][j] = particlesDir[i+1][j];
		}
	}

	for (int i = counter; i < totalNumParticles - 1;i++) {
		for (int j = 0; j < 3;j++) {
			newCubeAVec[i][j] = newCubeAVec[i + 1][j];
			newCubeBVec[i][j] = newCubeBVec[i + 1][j];
			newCubeCVec[i][j] = newCubeCVec[i + 1][j];
			newCubeDVec[i][j] = newCubeDVec[i + 1][j];
			newCubeEVec[i][j] = newCubeEVec[i + 1][j];
			newCubeFVec[i][j] = newCubeFVec[i + 1][j];
			newCubeGVec[i][j] = newCubeGVec[i + 1][j];
			newCubeHVec[i][j] = newCubeHVec[i + 1][j];
		}
	}

	counter--;
}

//draws the cube using polygons(it wont let me use drawPlane() for some reason here)
void drawCube() {
	glColor3ub(particles[counter][9], particles[counter][10], particles[counter][11]);
	glBegin(GL_POLYGON);
	glVertex3dv(newCubeFVec[counter]);
	glVertex3dv(newCubeGVec[counter]);
	glVertex3dv(newCubeCVec[counter]);
	glVertex3dv(newCubeBVec[counter]);
	glEnd();
	//drawPlane(newCubeFVec[counter], newCubeGVec[counter], newCubeCVec[counter], newCubeBVec[counter]);

	glColor3ub((particles[counter][6]), (particles[counter][7]), particles[counter][8]);
	glBegin(GL_POLYGON);
	glVertex3dv(newCubeDVec[counter]);
	glVertex3dv(newCubeHVec[counter]);
	glVertex3dv(newCubeGVec[counter]);
	glVertex3dv(newCubeCVec[counter]);
	glEnd();
	//drawPlane(newCubeD, newCubeH, newCubeG, newCubeC);

	glColor3ub(particles[counter][3], particles[counter][4], particles[counter][5]);
	glBegin(GL_POLYGON);
	glVertex3dv(newCubeEVec[counter]);
	glVertex3dv(newCubeFVec[counter]);
	glVertex3dv(newCubeGVec[counter]);
	glVertex3dv(newCubeHVec[counter]);
	glEnd();
	//drawPlane(newCubeE, newCubeF, newCubeG, newCubeH);

	glColor3ub(particles[counter][3], particles[counter][4], particles[counter][5]);
	glBegin(GL_POLYGON);
	glVertex3dv(newCubeAVec[counter]);
	glVertex3dv(newCubeBVec[counter]);
	glVertex3dv(newCubeCVec[counter]);
	glVertex3dv(newCubeDVec[counter]);
	glEnd();
	//drawPlane(newCubeA, newCubeB, newCubeC, newCubeD);

	glColor3ub(particles[counter][6], particles[counter][7], particles[counter][8]);
	glBegin(GL_POLYGON);
	glVertex3dv(newCubeAVec[counter]);
	glVertex3dv(newCubeEVec[counter]);
	glVertex3dv(newCubeFVec[counter]);
	glVertex3dv(newCubeBVec[counter]);
	glEnd();
	//drawPlane(newCubeA, newCubeE, newCubeF, newCubeB);

	glColor3ub(particles[counter][9], particles[counter][10], particles[counter][11]);
	glBegin(GL_POLYGON);
	glVertex3dv(newCubeHVec[counter]);
	glVertex3dv(newCubeEVec[counter]);
	glVertex3dv(newCubeAVec[counter]);
	glVertex3dv(newCubeDVec[counter]);
	glEnd();
	//drawPlane(newCubeH, newCubeE, newCubeA, newCubeD);
}

//draws the cube using lines
void drawCubeWire() {
	glColor3ub(particles[counter][9], particles[counter][10], particles[counter][11]);
	glBegin(GL_LINES);
	glVertex3dv(newCubeFVec[counter]);
	glVertex3dv(newCubeGVec[counter]);
	glVertex3dv(newCubeCVec[counter]);
	glVertex3dv(newCubeBVec[counter]);
	glEnd();

	glColor3ub((particles[counter][6]), (particles[counter][7]), particles[counter][8]);
	glBegin(GL_LINES);
	glVertex3dv(newCubeDVec[counter]);
	glVertex3dv(newCubeHVec[counter]);
	glVertex3dv(newCubeGVec[counter]);
	glVertex3dv(newCubeCVec[counter]);
	glEnd();

	glColor3ub(particles[counter][3], particles[counter][4], particles[counter][5]);
	glBegin(GL_LINES);
	glVertex3dv(newCubeEVec[counter]);
	glVertex3dv(newCubeFVec[counter]);
	glVertex3dv(newCubeGVec[counter]);
	glVertex3dv(newCubeHVec[counter]);
	glEnd();

	glColor3ub(particles[counter][3], particles[counter][4], particles[counter][5]);
	glBegin(GL_LINES);
	glVertex3dv(newCubeAVec[counter]);
	glVertex3dv(newCubeBVec[counter]);
	glVertex3dv(newCubeCVec[counter]);
	glVertex3dv(newCubeDVec[counter]);
	glEnd();

	glColor3ub(particles[counter][6], particles[counter][7], particles[counter][8]);
	glBegin(GL_LINES);
	glVertex3dv(newCubeAVec[counter]);
	glVertex3dv(newCubeEVec[counter]);
	glVertex3dv(newCubeFVec[counter]);
	glVertex3dv(newCubeBVec[counter]);
	glEnd();

	glColor3ub(particles[counter][9], particles[counter][10], particles[counter][11]);
	glBegin(GL_LINES);
	glVertex3dv(newCubeHVec[counter]);
	glVertex3dv(newCubeEVec[counter]);
	glVertex3dv(newCubeAVec[counter]);
	glVertex3dv(newCubeDVec[counter]);
	glEnd();
}

//draws the cube at a point
void drawCubePoint() {
	pointTemp[0] = (newCubeAVec[counter][0] + newCubeDVec[counter][0]) / 2;
	pointTemp[1] = (newCubeAVec[counter][1] + newCubeBVec[counter][1]) / 2;
	pointTemp[2] = (newCubeAVec[counter][2] + newCubeEVec[counter][2]) / 2;

	glColor3ub(particles[counter][9], particles[counter][10], particles[counter][11]);
	glBegin(GL_POINTS);
	glVertex3dv(pointTemp);
	glEnd();
}

//draws a plane
void drawPlane(double one[3], double two[3], double three[3], double four[3]) {
	glBegin(GL_POLYGON);
	glVertex3dv(one);
	glVertex3dv(two);
	glVertex3dv(three);
	glVertex3dv(four);
	glEnd();
}

//draws the fountain
void drawFountain() {

	//floor
	if (toggleHole == 0) { //if no hole in the floor
		glColor3f(1, 1, 1);
		drawPlane(FloorA, FloorB, FloorC, FloorD);

		glColor3ub(232, 234, 235);
		drawPlane(FloorD, FloorC, FloorE, FloorF);

		glColor3ub(210, 213, 214);
		drawPlane(FloorA, FloorD, FloorF, FloorG);
	}else { //if theres a hole in the floor
		glColor3f(1, 1, 1);
		drawPlane(FloorA1, FloorD1, FloorC1, FloorB1);

		glColor3ub(232, 234, 235);
		drawPlane(FloorB1, FloorC1, FloorG1, FloorF1);

		glColor3ub(210, 213, 214);
		drawPlane(FloorA1, FloorB1, FloorF1, FloorE1);

		glColor3f(1, 1, 1);
		drawPlane(FloorA2, FloorD2, FloorC2, FloorB2);

		glColor3ub(232, 234, 235);
		drawPlane(FloorD2, FloorC2, FloorG2, FloorH2);

		glColor3ub(210, 213, 214);
		drawPlane(FloorA2, FloorD2, FloorH2, FloorE2);

		glColor3f(1, 1, 1);
		drawPlane(FloorA3, FloorB3, FloorC3, FloorD3);

		glColor3ub(232, 234, 235);
		drawPlane(FloorC3, FloorB3, FloorF3, FloorG3);

		glColor3ub(210, 213, 214);
		drawPlane(FloorD3, FloorC3, FloorG3, FloorH3);

		glColor3f(1, 1, 1);
		drawPlane(FloorA4, FloorB4, FloorC4, FloorD4);

		glColor3ub(232, 234, 235);
		drawPlane(FloorA4, FloorE4, FloorF4, FloorB4);

		glColor3ub(210, 213, 214);
		drawPlane(FloorC4, FloorB4, FloorF4, FloorG4);
	}
	
	//shooter bottom
	glColor3ub(85, 89, 92);
	drawPlane(a, b, c, d);

	glColor3ub(101, 106, 110);
	drawPlane(a, e, f, b);

	glColor3ub(132, 137, 140);
	drawPlane(h, e, a, d);

	//shooter top
	glColor3ub(55, 56, 56);
	drawPlane(dd, hh, gg, cc);

	glColor3ub(40, 41, 41);
	drawPlane(ee, ff, gg, hh);

	glColor3ub(85, 89, 92);
	drawPlane(aa, bb, cc, dd);

	glColor3ub(101, 106, 110);
	drawPlane(aa, ee, ff, bb);
}

//sets the color of the particles
void setRGB(int r1, int g1, int b1, int r2, int g2, int b2, int r3, int g3, int b3, int particle) {
	particles[particle][3] = r1;
	particles[particle][4] = g1;
	particles[particle][5] = b1;

	particles[particle][6] = r2;
	particles[particle][7] = g2;
	particles[particle][8] = b2;

	particles[particle][9] = r3;
	particles[particle][10] = g3;
	particles[particle][11] = b3;
}

//sets the colors of the cubes, if the color is toggled the next cubes will be different colors
void initColors() {
	if (colourToggle == 1) { //colorful mode

		switch (nextColor) {
		case 0:
			setRGB(63, 133, 176, 99, 180, 230, 179, 210, 230, counter);
			break;
		case 1:
			setRGB(62, 72, 179, 80, 93, 230, 109, 120, 237, counter);
			break;
		case 2:
			setRGB(127, 50, 179, 170, 83, 230, 188, 125, 232, counter);
			break;
		case 3:
			setRGB(176, 60, 159, 214, 81, 195, 240, 113, 222, counter);
			break;
		case 4:
			setRGB(179, 46, 68, 212, 61, 86, 237, 83, 109, counter);
			break;
		case 5:
			setRGB(191, 23, 23, 224, 38, 38, 240, 67, 67, counter);
			break;
		case 6:
			setRGB(196, 83, 18, 235, 111, 40, 240, 128, 65, counter);
			break;
		case 7:
			setRGB(217, 183, 35, 237, 203, 57, 242, 213, 87, counter);
			break;
		case 8:
			setRGB(60, 181, 24, 83, 224, 40, 109, 240, 70, counter);
			break;
		case 9:
			setRGB(47, 214, 150, 71, 237, 174, 128, 224, 188, counter);
			break;
		}
		
		if (nextColor > 8) {
			nextColor = 0;
		}else {
			nextColor++;
		}
		
	}else {//not colorful mode
		setRGB(63, 133, 176, 99, 180, 230, 179, 210, 230, counter);
		
	}
}

//initializes each cubes coordinates
void initCubeVectors() {
	if (nextSize == 0) { // small cubes
		newCubeAVec[counter][0] = newCubeA[0];
		newCubeAVec[counter][1] = newCubeA[1] + 9;
		newCubeAVec[counter][2] = newCubeA[2];
		newCubeBVec[counter][0] = newCubeB[0];
		newCubeBVec[counter][1] = newCubeB[1] + 9;
		newCubeBVec[counter][2] = newCubeB[2];
		newCubeCVec[counter][0] = newCubeC[0];
		newCubeCVec[counter][1] = newCubeC[1] + 9;
		newCubeCVec[counter][2] = newCubeC[2];
		newCubeDVec[counter][0] = newCubeD[0];
		newCubeDVec[counter][1] = newCubeD[1] + 9;
		newCubeDVec[counter][2] = newCubeD[2];
		newCubeEVec[counter][0] = newCubeE[0];
		newCubeEVec[counter][1] = newCubeE[1] + 9;
		newCubeEVec[counter][2] = newCubeE[2];
		newCubeFVec[counter][0] = newCubeF[0];
		newCubeFVec[counter][1] = newCubeF[1] + 9;
		newCubeFVec[counter][2] = newCubeF[2];
		newCubeGVec[counter][0] = newCubeG[0];
		newCubeGVec[counter][1] = newCubeG[1] + 9;
		newCubeGVec[counter][2] = newCubeG[2];
		newCubeHVec[counter][0] = newCubeH[0];
		newCubeHVec[counter][1] = newCubeH[1] + 9;
		newCubeHVec[counter][2] = newCubeH[2];

		if (sizeToggle == 1) {
			nextSize = 1;
		}

	} else if (nextSize == 1) { //medium cubes
		newCubeAVec[counter][0] = newCubeAM[0];
		newCubeAVec[counter][1] = newCubeAM[1] + 9;
		newCubeAVec[counter][2] = newCubeAM[2];
		newCubeBVec[counter][0] = newCubeBM[0];
		newCubeBVec[counter][1] = newCubeBM[1] + 9;
		newCubeBVec[counter][2] = newCubeBM[2];
		newCubeCVec[counter][0] = newCubeCM[0];
		newCubeCVec[counter][1] = newCubeCM[1] + 9;
		newCubeCVec[counter][2] = newCubeCM[2];
		newCubeDVec[counter][0] = newCubeDM[0];
		newCubeDVec[counter][1] = newCubeDM[1] + 9;
		newCubeDVec[counter][2] = newCubeDM[2];
		newCubeEVec[counter][0] = newCubeEM[0];
		newCubeEVec[counter][1] = newCubeEM[1] + 9;
		newCubeEVec[counter][2] = newCubeEM[2];
		newCubeFVec[counter][0] = newCubeFM[0];
		newCubeFVec[counter][1] = newCubeFM[1] + 9;
		newCubeFVec[counter][2] = newCubeFM[2];
		newCubeGVec[counter][0] = newCubeGM[0];
		newCubeGVec[counter][1] = newCubeGM[1] + 9;
		newCubeGVec[counter][2] = newCubeGM[2];
		newCubeHVec[counter][0] = newCubeHM[0];
		newCubeHVec[counter][1] = newCubeHM[1] + 9;
		newCubeHVec[counter][2] = newCubeHM[2];
		nextSize = 2;

	}else if(nextSize == 2){ //large cubes
		newCubeAVec[counter][0] = newCubeAL[0];
		newCubeAVec[counter][1] = newCubeAL[1] + 9;
		newCubeAVec[counter][2] = newCubeAL[2];
		newCubeBVec[counter][0] = newCubeBL[0];
		newCubeBVec[counter][1] = newCubeBL[1] + 9;
		newCubeBVec[counter][2] = newCubeBL[2];
		newCubeCVec[counter][0] = newCubeCL[0];
		newCubeCVec[counter][1] = newCubeCL[1] + 9;
		newCubeCVec[counter][2] = newCubeCL[2];
		newCubeDVec[counter][0] = newCubeDL[0];
		newCubeDVec[counter][1] = newCubeDL[1] + 9;
		newCubeDVec[counter][2] = newCubeDL[2];
		newCubeEVec[counter][0] = newCubeEL[0];
		newCubeEVec[counter][1] = newCubeEL[1] + 9;
		newCubeEVec[counter][2] = newCubeEL[2];
		newCubeFVec[counter][0] = newCubeFL[0];
		newCubeFVec[counter][1] = newCubeFL[1] + 9;
		newCubeFVec[counter][2] = newCubeFL[2];
		newCubeGVec[counter][0] = newCubeGL[0];
		newCubeGVec[counter][1] = newCubeGL[1] + 9;
		newCubeGVec[counter][2] = newCubeGL[2];
		newCubeHVec[counter][0] = newCubeHL[0];
		newCubeHVec[counter][1] = newCubeHL[1] + 9;
		newCubeHVec[counter][2] = newCubeHL[2];
		nextSize = 0;
	}
}

//moves the cube
void changeCubeVectors() {
	newCubeAVec[counter][0] = newCubeAVec[counter][0] + particlesDir[counter][0];
	newCubeAVec[counter][1] = newCubeAVec[counter][1] + particlesDir[counter][1];
	newCubeAVec[counter][2] = newCubeAVec[counter][2] + particlesDir[counter][2];
	newCubeBVec[counter][0] = newCubeBVec[counter][0] + particlesDir[counter][0];
	newCubeBVec[counter][1] = newCubeBVec[counter][1] + particlesDir[counter][1];
	newCubeBVec[counter][2] = newCubeBVec[counter][2] + particlesDir[counter][2];
	newCubeCVec[counter][0] = newCubeCVec[counter][0] + particlesDir[counter][0];
	newCubeCVec[counter][1] = newCubeCVec[counter][1] + particlesDir[counter][1];
	newCubeCVec[counter][2] = newCubeCVec[counter][2] + particlesDir[counter][2];
	newCubeDVec[counter][0] = newCubeDVec[counter][0] + particlesDir[counter][0];
	newCubeDVec[counter][1] = newCubeDVec[counter][1] + particlesDir[counter][1];
	newCubeDVec[counter][2] = newCubeDVec[counter][2] + particlesDir[counter][2];
	newCubeEVec[counter][0] = newCubeEVec[counter][0] + particlesDir[counter][0];
	newCubeEVec[counter][1] = newCubeEVec[counter][1] + particlesDir[counter][1];
	newCubeEVec[counter][2] = newCubeEVec[counter][2] + particlesDir[counter][2];
	newCubeFVec[counter][0] = newCubeFVec[counter][0] + particlesDir[counter][0];
	newCubeFVec[counter][1] = newCubeFVec[counter][1] + particlesDir[counter][1];
	newCubeFVec[counter][2] = newCubeFVec[counter][2] + particlesDir[counter][2];
	newCubeGVec[counter][0] = newCubeGVec[counter][0] + particlesDir[counter][0];
	newCubeGVec[counter][1] = newCubeGVec[counter][1] + particlesDir[counter][1];
	newCubeGVec[counter][2] = newCubeGVec[counter][2] + particlesDir[counter][2];
	newCubeHVec[counter][0] = newCubeHVec[counter][0] + particlesDir[counter][0];
	newCubeHVec[counter][1] = newCubeHVec[counter][1] + particlesDir[counter][1];
	newCubeHVec[counter][2] = newCubeHVec[counter][2] + particlesDir[counter][2];

	//makes it so the particle falls off the edge if needed
	if (((newCubeBVec[counter][0] >= max) || (newCubeBVec[counter][0] <= -max) || (newCubeBVec[counter][2] >= max) || (newCubeBVec[counter][2] <= -max)) && newCubeBVec[counter][1] <= 0) {

		//makes it so cubes disapear after falling off the edge a bit
		if (newCubeBVec[counter][1] <= -15) {
			particles[counter][0] = 2;
		}

	}else if (toggleHole == 1 && ((newCubeBVec[counter][0] >= -15) && (newCubeBVec[counter][0] <= -2)) && ((newCubeBVec[counter][2] >= 12) && (newCubeBVec[counter][2] <= 23)) && newCubeBVec[counter][1] <= 0) { //if there is a hole, it falls through it
		//makes it so cubes disapear after falling off the edge a bit
		if (newCubeBVec[counter][1] <= -15) {
			particles[counter][0] = 2;
		}

	}else if (newCubeBVec[counter][1] <= 0 ) {//if it hits the floor, it bounces

		if (frictionToggle == 1) { //for friction mode
			particles[counter][2]++;
			particlesDir[counter][1] = originalY / (particles[counter][2]);
		}else { //for non friction mode
			particlesDir[counter][1] = -particlesDir[counter][1];
		}

		//makes it the particle eventually stops bouncing
		if (particlesDir[counter][1] <= 0.005) {
			particles[counter][0] = 2;
		}
	}
}

//for toggling the plotting version
void cube() {
	if (togglePlotting == 0) {
		drawCube();
	}else if (togglePlotting == 1) {
		drawCubeWire();
	}else {
		drawCubePoint();
	}
}

//main function, draws and tracks everything
void fountain() {
	//for tracking time between each frame
	if (firstClock == 0) {
		start = clock();
		firstClock = 1;
	}
	stop = clock();

	//for tracking time between each particle shooting
	if (secondClock == 0) {
		startNextParticle = clock();
		secondClock = 1;
		numParticles++;
		trackSingle++;
		nextParticle = clock();
	}

	if ((startNextParticle + fireingSpeed < nextParticle) && (numParticles < totalNumParticles)) {
		secondClock = 0;
	}
	nextParticle = clock();

	//for slow motion mode
	if (speed == 0 || start + 5 < stop) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawFountain();

		//for making the right amount of particles be drawn
		for (int i = 0; i < numParticles; i++)
		{
			counter = i;

			//checks the state of the particle, I made state = 1 at the top because state will be 1 more often then 0, and 2
			if (particles[i][0] == 1) {
				//moves particle
				changeCubeVectors();
				cube();
				particlesDir[counter][1] = particlesDir[counter][1] - currentGravity;

			}else if (particles[i][0] == 0 && fountainOff == 0) { //for when the particle is being initialized
				particles[counter][2] = 1;
				particlesDir[i][0] = 0;
				particlesDir[i][1] = 9;
				particlesDir[i][2] = 0;
				initCubeVectors();
				initColors();
				cube();
				particleDirection();
				particles[i][0] = 1;

			}else {//cube disapears
				deleteParticle();
				numParticles--;
				i--;
				particles[numParticles][0] = 0;
			}

			firstClock = 0;
		}

	}else {
		stop = clock();
	}
	
	//for single fire mode
	if (singleFire == 1 && temppCounter < trackSingle) {
		fountainOff = 1;
		singleFire = 0;
	}

	glutSwapBuffers();
}

//resets everything to defult
void reset() {
	counter = 0;
	numParticles = 0;
	start = clock();
	stop = clock();
	nextParticle = clock();
	startNextParticle = clock();
	firstClock = 0; 
	secondClock = 0; 
	numParticles = -1; 
	speed = 0; 
	colourToggle = 0;
	frictionToggle = 0; 
	nextColor = 0; 
	fountainOff = 0;
	toggleHole = 0;
	spreyModeVal = 0.01;
	gouraud = 0;
	fireingSpeed = 1;

	for (int i = 0; i < totalNumParticles; i++) {
		for (int j = 0; j < 14; j++) {
			particles[i][j] = 0;
		}
	}

	for (int i = 0; i < totalNumParticles; i++) {
		for (int j = 0; j < 3; j++) {
			particlesDir[i][j] = 0;
		}
	}

	for (int i = 0; i < totalNumParticles; i++) {
		newCubeAVec[i][0] = newCubeA[0];
		newCubeAVec[i][1] = newCubeA[1];
		newCubeAVec[i][2] = newCubeA[2];
	}

	for (int i = 0; i < totalNumParticles; i++) {
		newCubeBVec[i][0] = newCubeB[0];
		newCubeBVec[i][1] = newCubeB[1];
		newCubeBVec[i][2] = newCubeB[2];
	}

	for (int i = 0; i < totalNumParticles; i++) {
		newCubeCVec[i][0] = newCubeC[0];
		newCubeCVec[i][1] = newCubeC[1];
		newCubeCVec[i][2] = newCubeC[2];
	}

	for (int i = 0; i < totalNumParticles; i++) {
		newCubeDVec[i][0] = newCubeD[0];
		newCubeDVec[i][1] = newCubeD[1];
		newCubeDVec[i][2] = newCubeD[2];
	}

	for (int i = 0; i < totalNumParticles; i++) {
		newCubeEVec[i][0] = newCubeE[0];
		newCubeEVec[i][1] = newCubeE[1];
		newCubeEVec[i][2] = newCubeE[2];
	}

	for (int i = 0; i < totalNumParticles; i++) {
		newCubeFVec[i][0] = newCubeF[0];
		newCubeFVec[i][1] = newCubeF[1];
		newCubeFVec[i][2] = newCubeF[2];
	}

	for (int i = 0; i < totalNumParticles; i++) {
		newCubeGVec[i][0] = newCubeG[0];
		newCubeGVec[i][1] = newCubeG[1];
		newCubeGVec[i][2] = newCubeG[2];
	}

	for (int i = 0; i < totalNumParticles; i++) {
		newCubeHVec[i][0] = newCubeH[0];
		newCubeHVec[i][1] = newCubeH[1];
		newCubeHVec[i][2] = newCubeH[2];
	}
}

//for the kayboard input
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
	case 'A'://slow motion
		if (speed == 0) {
			speed = 1;
			fireingSpeed = 0;
		}else {
			speed = 0;
			fireingSpeed = 1;
		}
		break;
	case 'd':
	case 'D'://turns off fountain
		if (fountainOff == 0) {
			fountainOff = 1;
		}else {
			fountainOff = 0;
		}
		break;
	case 'F':
	case 'f'://manual or stream generation
		if (fountainOff == 1) {
			fountainOff = 0;
		}
		break;
	case 'r':
	case 'R'://friction on ground toggle
		if (frictionToggle == 0) {
			frictionToggle = 1;
		}else {
			frictionToggle = 0;
		}
		break;
	case 'x':
	case 'X'://colourful mode toggle
		if (sizeToggle == 0) {
			sizeToggle = 1;
		}else {
			nextSize = 0;
			sizeToggle = 0;
		}
		break;
	case 'c':
	case 'C'://colourful mode toggle
		if (colourToggle == 0) {
			colourToggle = 1;
		}else {
			colourToggle = 0;
		}
		break;
	case 'e':
	case 'E'://hole in floor toggle
		if (toggleHole == 0) {
			toggleHole = 1;
		}else {
			toggleHole = 0;
		}
		break;
	case 'v':
	case 'V'://spray mode
		if (spreyModeVal == 0.01) {
			spreyModeVal = 0.02;
		}else {
			spreyModeVal = 0.01;
		}
		break;
	case 't':
	case 'T'://toggle gouraud shading
		if (gouraud == 0) {
			gouraud = 1;
		}
		else {
			gouraud = 0;
		}
		break;
	case 's':
	case 'S'://toggle num particles
		if (fireingSpeed == 1) {
			fireingSpeed = 20;
		}else if(fireingSpeed == 20){
			fireingSpeed = 100;
		}else {
			fireingSpeed = 1;
		}
		break;
	case 'z':
	case 'Z'://toggles explosion
		if (gouraud == 0) {
			gouraud = 1;
		}
		else {
			gouraud = 0;
		}
		break;
	case 'y':
	case 'Y'://toggles polygon, lines, and points
		if (togglePlotting == 0) {
			togglePlotting = 1;
		}else if(togglePlotting == 1){
			togglePlotting = 2;
		}else {
			togglePlotting = 0;
		}
		break;
	case 'g':
	case 'G'://single fire mode
		fountainOff = 0;
		singleFire = 1;
		temppCounter = trackSingle;

		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	case 'w'://resets the simulation
	case 'W':
		reset();
		break;
	}
}

//for the manual stream generation when the key goes up
void keyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'F':
	case 'f'://manual or stream generation
		fountainOff = 1;
		
		break;
	}
}

void instructions() {
	printf("What the keys do:\n");
	printf("* q - quits the program\n");
	printf("* w - resets everything\n");
	printf("* e - toggles the hole in the ground\n");
	printf("* r - friction on the ground\n");
	printf("* t - toggles gouraud shading\n");
	printf("* y - toggles polygon, wire, and points for the cubes\n");
	printf("\n");
	printf("* a - toggles slow motion\n");
	printf("* s - toggles number of particles\n");
	printf("* d - turns the fountain off and on\n");
	printf("* f - when pressed particles fire, when released particles stop firing\n");
	printf("* g - single shot mode\n");
	printf("\n");
	printf("* x - toggles different size\n");
	printf("* c - toggles different colours\n");
	printf("* v - spray mode, toggles between low spray(default, slightly randomness to initial direction) to high spray(wildly random)\n");
}

main(int argc, char** argv) {
	instructions();
	srand(time(0));
	glutInit(&argc, argv);
	glutInitWindowSize(900, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	
	//for backface culling
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	
	glutCreateWindow("glLookat example");

	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(fountain);
	glutIdleFunc(fountain);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1.5, 0.1, 250.0);
	gluLookAt(45, 30, 150, -5.5, 5, 0, 0, 1.0, 0.0);

	glMatrixMode(GL_MODELVIEW);

	glClearColor(0, 0, 0, 0);  /* black */
	
	//to toggle gauroud shading
	if (gouraud == 0) {
		glShadeModel(GL_SMOOTH);
	}else {
		glShadeModel(GL_FLAT);
	}

	glutMainLoop();
	return 0;
}