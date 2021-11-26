// standard
#include <assert.h>
#include <math.h>
#include <iostream>
#define PI 3.14159265
// glut
#include <GL/glut.h>

//================================
// global variables
//================================

//================================
//INSTRUCTION:
//
//================


//option
int spline_opt, //1 for catmull-rom, 2 for b spline
rotation_opt; // 1 for fixed angle, 2 for quaternion

float point_num = 6;
//speed for leg movement
float speed = 3;
//ball location
float ballloc[1][3] = { {-15, -5, -10} };
//ball velocity
float ballv[1][3] = { {1, 0, 0} };
//gravity
float g[3] = { 0, 1, 0 };

//geometric point for quaternion
float qaray[6][7] = { {-8.0, -6.0, -25.0, 0, 1, 0, 0}, {-2.0, -5.0, -17.0, 1, 0, 1, 0}, {5.5, -2.0, -13.0, 1, 0, 0, 1},{2.0, 3.0, -15.0, 1, 1, 0, 0},
	{-3.5, 6.5, -20.0, 1, 0, 1, 0}, {10.0, 6.0, -15.0, 0, 0, 1, 0} };

//geometric point for fixed angle
float fixaray[6][6] = { {-8.0, -6.0, -25.0, 0, 0, 0}, {-2.0, -5.0, -17.0, 0, 0, 30}, {5.5, -2.0, -13.0, 0, 90, 0},{2.0, 3.0, -15.0, 0, 0, 0},
	{-3.5, 6.5, -20.0, 0, 30, 0}, {3.0, 8.0, -15.0, 0, 0, 0} };

float dt = 0.01; //dt is the spcing to used in the animation

//geometrix matrix for torso and legs
float M[16] = { 0 };//torso

float* MR; //right leg

//t is the value in both catmull-rom and b spline function.
float t = 0.0;
float x, y, z; //current location for the object

//Matrix for Calculating Catmull Rom, using 1/2 as 'a'
float mCR[4][4] = { {-0.5, 1.5, -1.5, 0.5}, {1.0, -2.5, 2.0, -0.5},
	{-0.5, 0.0, 0.5, 0.0}, {0.0, 1.0, 0.0, 0.0} };
//Matrix for Calculating B-spline
float mB[4][4] = { {-1.0, 3.0, -3.0, 1.0}, {3.0, -6.0, 3.0, 0.0},
	{-3.0, 0.0, 3.0, 0.0}, {1.0, 4.0, 1.0, 0.0} };

// screen size
int g_screenWidth = 0;
int g_screenHeight = 0;

// frame index
int g_frameIndex = 0;

//draw floor
void drawFloor() {
	glBegin(GL_QUADS);
	glVertex3f(10.0f, -5.0f, -10.0f);

	glVertex3f(-10.0f, -5.0f, -10.0f);

	glVertex3f(-10.0f, -5.0f, -20.0f);

	glVertex3f(10.0f, -5.0f, -20.0f);

	glEnd();

}
void drawBalls() {

}

//Matrix multiplication for Q(t) = TMG
float matrixTMG(float M[4][4], float G[4]) {
	float TMG;//return value
	float MG[4];//valur of M * G
	float mt = t - int(t);
	MG[0] = mCR[0][0] * G[0] + mCR[0][1] * G[1] + mCR[0][2] * G[2] + mCR[0][3] * G[3];
	MG[1] = mCR[1][0] * G[0] + mCR[1][1] * G[1] + mCR[1][2] * G[2] + mCR[1][3] * G[3];
	MG[2] = mCR[2][0] * G[0] + mCR[2][1] * G[1] + mCR[2][2] * G[2] + mCR[2][3] * G[3];
	MG[3] = mCR[3][0] * G[0] + mCR[3][1] * G[1] + mCR[3][2] * G[2] + mCR[3][3] * G[3];
	TMG = MG[0] * mt * mt * mt + MG[1] * mt * mt + MG[2] * mt + MG[3];
	return TMG;
}

//matrix multiplacation for transformation(4*4 matrix * 4*4 matrix)
float* matrixtransform(float M1[16], float M2[16]) {
	static float M12[16];
	M12[0] = M1[0] * M2[0] + M1[4] * M2[1] + M1[8] * M2[2] + M1[12] * M2[3];
	M12[1] = M1[1] * M2[0] + M1[5] * M2[1] + M1[9] * M2[2] + M1[13] * M2[3];
	M12[2] = M1[2] * M2[0] + M1[6] * M2[1] + M1[10] * M2[2] + M1[14] * M2[3];
	M12[3] = M1[3] * M2[0] + M1[7] * M2[1] + M1[11] * M2[2] + M1[15] * M2[3];
	M12[4] = M1[0] * M2[4] + M1[4] * M2[5] + M1[8] * M2[6] + M1[12] * M2[7];
	M12[5] = M1[1] * M2[4] + M1[5] * M2[5] + M1[9] * M2[6] + M1[13] * M2[7];
	M12[6] = M1[2] * M2[4] + M1[6] * M2[5] + M1[10] * M2[6] + M1[14] * M2[7];
	M12[7] = M1[3] * M2[4] + M1[7] * M2[5] + M1[11] * M2[6] + M1[15] * M2[7];
	M12[8] = M1[0] * M2[8] + M1[4] * M2[9] + M1[8] * M2[10] + M1[12] * M2[11];
	M12[9] = M1[1] * M2[8] + M1[5] * M2[9] + M1[9] * M2[10] + M1[13] * M2[11];
	M12[10] = M1[2] * M2[8] + M1[6] * M2[9] + M1[10] * M2[10] + M1[14] * M2[11];
	M12[11] = M1[3] * M2[8] + M1[7] * M2[9] + M1[11] * M2[10] + M1[15] * M2[11];
	M12[12] = M1[0] * M2[12] + M1[4] * M2[13] + M1[8] * M2[14] + M1[12] * M2[15];
	M12[13] = M1[1] * M2[12] + M1[5] * M2[13] + M1[9] * M2[14] + M1[13] * M2[15];
	M12[14] = M1[2] * M2[12] + M1[6] * M2[13] + M1[10] * M2[14] + M1[14] * M2[15];
	M12[15] = M1[3] * M2[12] + M1[7] * M2[13] + M1[11] * M2[14] + M1[15] * M2[15];
	return M12;
}

//================================
// Quaternion rotation 
// ===============================
void quaternion(float w, float qx, float qy, float qz) {
	M[0] = 1.0f - 2.0f * (qy * qy + qz * qz);
	M[1] = 2.0f * (qx * qy + w * qz);
	M[2] = 2.0f * (qx * qz - w * qy);
	M[3] = 0.0f;
	M[4] = 2.0f * (qx * qy - w * qz);
	M[5] = 1.0f - 2.0f * (qx * qx + qz * qz);
	M[6] = 2.0f * (qy * qz + w * qx);
	M[7] = 0.0f;
	M[8] = 2.0f * (qx * qz + w * qy);
	M[9] = 2.0f * (qy * qz - w * qx);
	M[10] = 1 - 2.0f * (qx * qx + qy * qy);
	M[11] = 0.0f;
	M[12] = x;
	M[13] = y;
	M[14] = z;
	M[15] = 1.0f;
}

//================================
//fixed angle rotation
//================================
void fixedangle(float fx, float fy, float fz) {
	//use pi to represent degree
	double dx = fx * PI / 180.0;
	double dy = fy * PI / 180.0;
	double dz = fz * PI / 180.0;
	//assume rotate about x first, then y, then z
	M[0] = cos(dz) * cos(dy);
	M[1] = sin(dz) * cos(dy);
	M[2] = -sin(dy);
	M[3] = 0.0f;
	M[4] = cos(dz) * sin(dy) * sin(dx) - sin(dz) * cos(dx);
	M[5] = sin(dz) * sin(dy) * sin(dx) + cos(dz) * cos(dx);
	M[6] = cos(dy) * sin(dx);
	M[7] = 0.0f;
	M[8] = cos(dz) * sin(dy) * cos(dx) + sin(dz) * sin(dx);
	M[9] = sin(dz) * sin(dy) * cos(dx) - cos(dz) * sin(dx);
	M[10] = cos(dy) * cos(dx);
	M[11] = 0.0f;
	M[12] = x;
	M[13] = y;
	M[14] = z;
	M[15] = 1.0f;
}


//================================
// init
//================================
void init(void) {
	// init something before main loop...

}

//================================
// update
//================================
void update(void) {
}

//================================
// render
//================================
void render(void) {
	// clear buffer
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render state
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// light source attributes
	GLfloat LightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightDiffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat LightSpecular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightPosition[] = { 5.0f, 5.0f, 5.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	// surface material attributes
	GLfloat material_Ka[] = { 0.11f, 0.06f, 0.11f, 1.0f };
	GLfloat material_Kd[] = { 0.43f, 0.47f, 0.54f, 1.0f };
	GLfloat material_Ks[] = { 0.33f, 0.33f, 0.52f, 1.0f };
	GLfloat material_Ke[] = { 0.1f , 0.0f , 0.1f , 1.0f };
	GLfloat material_Se = 10;

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_Ks);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_Se);

	// modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.0, 0.0, 1.0);
	//glRotated(0, 0.0, 1.0, 0.0);
	//glutSolidTeapot(1.1);
	drawFloor();
	//draw ball
	drawBalls();
	// disable lighting
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	// swap back and front buffers
	glutSwapBuffers();



}

//================================
// keyboard input
//================================
void keyboard(unsigned char key, int x, int y) {
}

//================================
// reshape : update viewport and projection matrix when the window is resized
//================================
void reshape(int w, int h) {
	// screen size
	g_screenWidth = w;
	g_screenHeight = h;

	// viewport
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, (GLfloat)w / (GLfloat)h, 1.0, 500);
}


//================================
// timer : triggered every 16ms ( about 60 frames per second )
//================================
void timer(int value) {
	// increase frame index
	g_frameIndex++;

	update();

	// render
	glutPostRedisplay();

	// reset timer
	// 16 ms per frame ( about 60 frames per second )
	glutTimerFunc(16, timer, 0);
}

//================================
// main
//================================
int main(int argc, char** argv) {
	/***
	std::cout << "press 1 for catmull-rom spline \n";
	std::cout << "press 2 for B-spline\n";
	std::cin >> spline_opt;
	std::cout << "press 1 for fixed angle\n";
	std::cout << "press 2 for quaternion\n";
	std::cin >> rotation_opt;
	***/
	spline_opt = 1;
	rotation_opt = 1;

	//get critical points for left legs--using fixed angle
	// create opengL window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1600, 1200);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	// init
	init();

	// set callback functions
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(16, timer, 0);

	// main loop
	glutMainLoop();

	return 0;
}
