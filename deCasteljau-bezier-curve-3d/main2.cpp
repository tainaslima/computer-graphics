#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

int dimX = 800;
int dimY = 600;

float** cloudOfPoints;
int currNumOfPointsCP = 0;
int maxNumOfPointsCP = 4;

float** bezierCurve;
int currNumOfPointsBC = 0;
int numOfDiv = 50;
int maxNumOfPointsBC = numOfDiv+1;


bool canDrawPoints = false;
bool canDrawBC = false;

void printCloudOfPoints(void)
{
	for (int i = 0; i < currNumOfPointsCP; i++)
	{
		printf("cloudOfPoints[%d] = %f %f %f\n", i, cloudOfPoints[i][0], cloudOfPoints[i][1], cloudOfPoints[i][2]);
	}
}

void resizeCloudMatrix(void)
{
	cloudOfPoints = (float **)realloc(cloudOfPoints, sizeof(int)*maxNumOfPointsCP * 2);

	for (int i = maxNumOfPointsCP; i < 2 * maxNumOfPointsCP; i++)
	{
		cloudOfPoints[i] = (float *)malloc(sizeof(float) * 3);
	}

	maxNumOfPointsCP *= 2;
}

void freeMatrix(float** p, int max)
{
	for (int i = 0; i < max; i++)
	{
		free(p[i]);
	}

	free(p);
}

void freeMatrix2(float*** p, int max)
{
	for (int i = 0; i < max; i++)
	{
		for (int j = 0; j < max; j++)
		{
			free(p[i][j]);
		}

		free(p[i]);
	}

	free(p);
}

void addPointOnCloud(float x, float y, float z)
{
	if (currNumOfPointsCP + 1 >= maxNumOfPointsCP)
	{
		resizeCloudMatrix();
	}


	cloudOfPoints[currNumOfPointsCP][0] = x;
	cloudOfPoints[currNumOfPointsCP][1] = y;
	cloudOfPoints[currNumOfPointsCP][2] = z;

	currNumOfPointsCP++;
}

void initB(float*** b)
{
	for (int i = 0; i < currNumOfPointsCP; i++)
	{
		b[i] = (float **)malloc(sizeof(float) * currNumOfPointsCP);

		for (int j = 0; j < currNumOfPointsCP; j++)
		{
			b[i][j] = (float *)malloc(sizeof(float) * 3);
		}
	}

	for (int i = 0; i < currNumOfPointsCP; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			b[0][i][j] = cloudOfPoints[i][j];
		}
	}
}

void findXt(float t, float*** b, float** bc)
{
	for (int r = 1; r < currNumOfPointsCP; r++)
	{
		for (int i = 0; i < currNumOfPointsCP - r; i++)
		{
			b[r][i][0] = (1 - t)*b[r - 1][i][0] + t * b[r - 1][i + 1][0];
			b[r][i][1] = (1 - t)*b[r - 1][i][1] + t * b[r - 1][i + 1][1];
			b[r][i][2] = (1 - t)*b[r - 1][i][2] + t * b[r - 1][i + 1][2];
		}
	}

	bc[currNumOfPointsBC][0] = b[currNumOfPointsCP-1][0][0];
	bc[currNumOfPointsBC][1] = b[currNumOfPointsCP-1][0][1];
	bc[currNumOfPointsBC][2] = b[currNumOfPointsCP-1][0][2];

	currNumOfPointsBC++;
}

void deCasteljuAlgorithm(void)
{
	float*** b = (float ***)malloc(sizeof(float)*currNumOfPointsCP);
	float t;
	initB(b);

	bezierCurve[currNumOfPointsBC][0] = cloudOfPoints[0][0];
	bezierCurve[currNumOfPointsBC][1] = cloudOfPoints[0][1];
	bezierCurve[currNumOfPointsBC][2] = cloudOfPoints[0][2];

	currNumOfPointsBC++;

	for (int i = 0; i < numOfDiv; i++)
	{
		t = (float) i / numOfDiv;

		findXt(t, b, bezierCurve);
	}

	bezierCurve[currNumOfPointsBC-1][0] = cloudOfPoints[currNumOfPointsCP-1][0];
	bezierCurve[currNumOfPointsBC-1][1] = cloudOfPoints[currNumOfPointsCP-1][1];
	bezierCurve[currNumOfPointsBC-1][2] = cloudOfPoints[currNumOfPointsCP-1][2];

	freeMatrix2(b, currNumOfPointsCP);

}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:
			exit(0);
			break;

		case 'a': // Girar para a esquerda
			glRotatef(-1.0,0.0,1.0,0.0);
			glutPostRedisplay();
			break;

		case 'd': // Girar para a direita
			glRotatef(1.0, 0.0, 1.0, 0.0);
			glutPostRedisplay();
			break;

		case 'w': // Girar para cima
			glRotatef(1.0, 1.0, 0.0, 0.0);
			glutPostRedisplay();
			break;
	
		case 's': // Girar para baixo
			glRotatef(-1.0, 1.0, 0.0, 0.0);
			glutPostRedisplay();
			break;

		case 'b':
			deCasteljuAlgorithm();
			canDrawBC = true;
			glutPostRedisplay();
			break;
	}
}

void printCoordSystem(void)
{
	glColor3f(1.0, 0.0, 0.0); // red x
	glBegin(GL_LINES);
	glVertex3f(-1.0, 0.0f, 0.0f);
	glVertex3f(1.0, 0.0f, 0.0f);
	glEnd();

	glColor3f(0.0, 1.0, 0.0); // green y
	glBegin(GL_LINES);
	glVertex3f(0.0, -1.0f, 0.0f);
	glVertex3f(0.0, 1.0f, 0.0f);
	glEnd();
 
	glColor3f(0.0, 0.0, 1.0); // blue z
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0f, -1.0f);
	glVertex3f(0.0, 0.0f, 1.0f);
	glEnd();

}

void displayFunc(void)
{
	srand(time(NULL));
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glPushMatrix();
	glRotatef(0, 0, 0, 1.0);

	glPointSize(6);

	printCoordSystem();

	if (canDrawPoints)
	{
		glBegin(GL_POINTS);

		float r = (rand() % 10 + 1) / 10.0;
		float g = (rand() % 10 + 1) / 10.0;
		float b = (rand() % 10 + 1) / 10.0;

		glColor3f(r, g, b);

		for (int i = 0; i < currNumOfPointsCP; i++)
		{
			glVertex3f(cloudOfPoints[i][0], cloudOfPoints[i][1], cloudOfPoints[i][2]);
		}

		glEnd();

		glBegin(GL_LINE_STRIP);

		glColor3f(r, g, b);

		for (int i = 0; i < currNumOfPointsCP; i++)
		{
			glVertex3f(cloudOfPoints[i][0], cloudOfPoints[i][1], cloudOfPoints[i][2]);
		}

		glEnd();
	}

	if (canDrawBC)
	{
		glBegin(GL_LINE_STRIP);
		float r = (rand() % 10 + 1) / 10.0;
		float g = (rand() % 10 + 1) / 10.0;
		float b = (rand() % 10 + 1) / 10.0;

		glColor3f(r, g, b);

		for (int i = 0; i < currNumOfPointsBC; i++)
		{
			glVertex3f(bezierCurve[i][0], bezierCurve[i][1], bezierCurve[i][2]);
		}

		glEnd();


	}
	
	glFlush();
	glPopMatrix();
	glutSwapBuffers();
}

int main(int argc, char **argv)
{

	cloudOfPoints = (float **)malloc(sizeof(float) * maxNumOfPointsCP);

	for (int i = 0; i < maxNumOfPointsCP; i++)
	{
		cloudOfPoints[i] = (float *)malloc(sizeof(float) * 3);
	}

	bezierCurve = (float **)malloc(sizeof(float) * maxNumOfPointsBC);

	for (int i = 0; i < maxNumOfPointsBC; i++)
	{
		bezierCurve[i] = (float *)malloc(sizeof(float) * 3);
	}

	addPointOnCloud(-0.1f, 0.1f, 0.1f);
	addPointOnCloud(0.9f, 0.4f, 0.5f);
	addPointOnCloud(0.0f, -0.6f, 0.8f);
	addPointOnCloud(0.3f, 0.5f, 0.4f);
	addPointOnCloud(-0.5f, 0.2f, -0.6f);

	canDrawPoints = true;

	glutInit(&argc, argv);
	glutInitWindowSize(dimX, dimY);
	glutCreateWindow("Bezier curve");
	glutKeyboardFunc(keyboardFunc);
	glutDisplayFunc(displayFunc);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();

	freeMatrix(cloudOfPoints, maxNumOfPointsCP);
	freeMatrix(bezierCurve, maxNumOfPointsBC);

	return 0;
}