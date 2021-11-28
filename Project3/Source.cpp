// standard
#include <assert.h>
#include <math.h>
#include <iostream>
// glut
#include <GL/glut.h>

//================================
// global variables
//================================

//================================
//INSTRUCTION:
//please change the following section of ball to test the code.
//you could decide how many ball do you want to use, their original locations, origina velocity, force on axis and mass.
// if you would like to change the ball's number(ballnum), remember to change first digit of ballloc, ballold, ballv, and ballm.
//================

//=====================
// ball
// ====================
//total ball number
int ballnum = 5;
//ball location
float ballloc[5][3] = { {-10, 10, -15}, {10, 8, -15}, {-8, 6, -10}, {-9, 7, -15}, {10, 10, -15} };
float ballold[5][3] = { 0 };
//force on ball
float ballf[5][3] = { {10, 0, 0}, {-10, 0, 0}, {5, 0, 3}, {8, 0, 5}, {-10, 0, 0} };
//ball's velocity
float ballv[5][3] = { 0 };
//ball mass
float ballm[5] = {10, 8, 5, 5, 10};
//ball radius
float ballr = 1;
//gravity
float g = -9.8;
//boolean if on floor
bool onfloor = false;

//geometrix matrix for ball
float M[16] = { 0 };

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
void drawBalls(int bn) {

	//update location;
	//calculate acceleration a = F/m;
	float ax = ballf[bn][0] / ballm[bn];
	float ay = ballf[bn][1] / ballm[bn];
	float az = ballf[bn][2] / ballm[bn];
	//calculate velocity
	ballv[bn][0] = ballv[bn][0] + ax * 0.01;
	ballv[bn][1] = ballv[bn][1] + (ay + g) * 0.01;
	ballv[bn][2] = ballv[bn][2] + az * 0.01;
	//add gravity
	//ballf[bn][1] = ballf[bn][1] + g * 0.01;
	//std::cout << ballf[bn][1];
	float x = ballloc[bn][0] + ballv[bn][0] * 0.01;
	float y = ballloc[bn][1] + ballv[bn][1] * 0.01 ;
	float z = ballloc[bn][2] + ballv[bn][2] * 0.01;
	ballloc[bn][0] = x;
	ballloc[bn][1] = y;
	ballloc[bn][2] = z;

	glPushMatrix();
	M[0] = 1.0f;
	M[5] = 1.0f;
	M[10] = 1.0f;
	M[15] = 1.0f;
	M[12] = x;
	M[13] = y;
	M[14] = z;
	glMultMatrixf(M);
	//glTranslatef(x, y, z);
	glutSolidSphere(ballr, 20, 20);
	glPopMatrix();

}

//check floor collision
//bn is current ball's index

void checkFloor(int bn) {
	//if ball collade with floor
	//std::cout << ballloc[bn][2];
	if ((ballloc[bn][1] + 5 <= ballr) // - (-5) = +5, -5 is the floor's y
		&& ( -10 <= ballloc[bn][0]) &&( ballloc[bn][0] <= 10)  //check if the ball will locate on floor
		&& (-20 <= ballloc[bn][2]) && (ballloc[bn][2] <= -10)) {
		ballv[bn][1] = ballv[bn][1] * -0.9 ; //energy loss coused by collision 
		std::cout << ballloc[bn][0];
		//onfloor = true;
	}

}

//chcek boll collision
void checkball(int bn) {
	//use current x/y minus other x/y, if the absolute value is less than radius, then collision detected
	for (int i = 0; i < ballnum; i++) {
		if (i != bn) {
			//check distance
			if (sqrtf((ballloc[i][0] - ballloc[bn][0]) * (ballloc[i][0] - ballloc[bn][0])
				+ (ballloc[i][1] - ballloc[bn][1]) * (ballloc[i][1] - ballloc[bn][1])
				+ (ballloc[i][2] - ballloc[bn][2]) * (ballloc[i][2] - ballloc[bn][2])) < 2 * ballr) {
				//use elastic collision to calculate velocity of two balls
				//velocity on x
				ballv[i][0] = ((ballm[i] - ballm[bn]) * ballv[i][0] + 2 * ballm[bn] * ballv[bn][0]) / (ballm[i] + ballm[bn]);
				ballv[bn][0] = ((ballm[bn] - ballm[i]) * ballv[bn][0] + 2 * ballm[i] * ballv[i][0]) / (ballm[i] + ballm[bn]);

				//velocity on y
				ballv[i][1] = ((ballm[i] - ballm[bn]) * ballv[i][1] + 2 * ballm[bn] * ballv[bn][1]) / (ballm[i] + ballm[bn]);
				ballv[bn][1] = ((ballm[bn] - ballm[i]) * ballv[bn][1] + 2 * ballm[i] * ballv[i][1]) / (ballm[i] + ballm[bn]);

				// velocity on z
				ballv[i][2] = ((ballm[i] - ballm[bn]) * ballv[i][2] + 2 * ballm[bn] * ballv[bn][2]) / (ballm[i] + ballm[bn]);
				ballv[bn][2] = ((ballm[bn] - ballm[i]) * ballv[bn][2] + 2 * ballm[i] * ballv[i][2]) / (ballm[i] + ballm[bn]);

				//change force after collision
				float tempx = ballf[i][0];
				ballf[i][0] += ballf[bn][0];
				ballf[bn][0] += ballf[i][0];
				
				float tempy = ballf[i][1];
				ballf[i][1] += ballf[bn][1];
				ballf[bn][1] += ballf[i][1];
				
				float tempz = ballf[i][2];
				ballf[i][2] += ballf[bn][2];
				ballf[bn][2] += ballf[i][2];
				
				
			}
		}
	}

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
	for (int i = 0; i < ballnum; i++) {
		checkFloor(i);
		drawBalls(i);
	}
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
	gluPerspective(70.0, (GLfloat)w / (GLfloat)h, 1.0, 500);
}


//================================
// timer : triggered every 16ms ( about 60 frames per second )
//================================
void timer(int value) {

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

	//inital ball variables
	/***
	for (int i = 0; i < ballnum; i++) {
		ballold[i][0] = ballloc[i][0];
		ballold[i][1] = ballloc[i][1];
		ballold[i][2] = ballloc[i][2];
	}
	***/

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
