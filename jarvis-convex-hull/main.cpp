#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

int dimX = 400;
int dimY = 400;


float** cloudOfPoints; 
int currNumOfPointsCP = 0; 
int maxNumOfPointsCP = 4; 

float** convexHull;
int currNumOfPointsCH = 0;
int maxNumOfPointsCH = 4;

bool canDrawPoints = false; 
bool canDrawCH = false; 

void resizeCloudMatrix(void)
{
	cloudOfPoints = (float **)realloc(cloudOfPoints, sizeof(int)*maxNumOfPointsCP * 2); 

	for (int i = maxNumOfPointsCP; i < 2 * maxNumOfPointsCP; i++)
	{
		cloudOfPoints[i] = (float *)malloc(sizeof(float) * 2); 
	}

	maxNumOfPointsCP *= 2;
}

void resizeCHMatrix(void)
{
	convexHull = (float **)realloc(convexHull, sizeof(int)*maxNumOfPointsCH * 2); 

	for (int i = maxNumOfPointsCH; i < 2 * maxNumOfPointsCH; i++)
	{
		convexHull[i] = (float *)malloc(sizeof(float) * 2); 
	}

	maxNumOfPointsCH *= 2;
}

void freeMatrix(float** p, int max)
{
	for (int i = 0; i < max; i++)
	{
		free(p[i]);
	}

	free(p);
}

void addPointOnCloud(int x, int y)
{
	if (currNumOfPointsCP + 1 >= maxNumOfPointsCP) 
	{
		resizeCloudMatrix(); 
	}

	
	cloudOfPoints[currNumOfPointsCP][0] = ((2 * x) / (float)dimX) - 1.0;
	cloudOfPoints[currNumOfPointsCP][1] = -((2 * y) / (float)dimY) + 1.0;

	currNumOfPointsCP++;
}

void addPointOnConvexHull(float x, float y)
{
	if (currNumOfPointsCH + 1 >= maxNumOfPointsCH) 
	{
		resizeCHMatrix(); 
	}

	
	convexHull[currNumOfPointsCH][0] = x;
	convexHull[currNumOfPointsCH][1] = y;

	currNumOfPointsCH++;
}

bool isEqual(float* a, float* b)
{
	return ((a[0] == b[0]) and (a[1] == b[1])) ? true : false;
}

bool findInCH(float * a)
{
	for (int i = 0; i < currNumOfPointsCH; i++)
	{
		if ((a[0] == convexHull[i][0]) and (a[1] == convexHull[i][1]))
			return true;
	}

	return false;
}

float vectorialProduct(float* a, float* b)
{
	return (a[0] * b[1] - a[1] * b[0]);
}

float escalarProduct(float* a, float* b)
{
	return (a[0] * b[0] + a[1] * b[1]);
}

float vectorModule(float* a)
{
	return sqrt(pow(a[0], 2) + pow(a[1], 2));
}

float cosAng(float* a, float* b)
{
	return (escalarProduct(a, b) / (vectorModule(a) * vectorModule(b)));
}

float* findPointWithLowestY(void)
{
	float* lowestY = (float*)malloc(sizeof(float) * 2);


	lowestY[0] = cloudOfPoints[0][0];
	lowestY[1] = cloudOfPoints[0][1];

	

	for (int i = 0; i < currNumOfPointsCP; i++)
	{
		if (cloudOfPoints[i][1] < lowestY[1])
		{
			lowestY[0] = cloudOfPoints[i][0];
			lowestY[1] = cloudOfPoints[i][1];
		}
	}

	return lowestY;
}

float* findFirstChoice(float* base, float*p1)
{
	float* currVector = (float*)malloc(sizeof(float) * 2);
	for (int i = 0; i < currNumOfPointsCP; i++)
	{
		currVector[0] = cloudOfPoints[i][0] - p1[0];
		currVector[1] = cloudOfPoints[i][1] - p1[1];
		
		if ((vectorialProduct(base, currVector) > 0.0) and (findInCH(cloudOfPoints[i]) == false || isEqual(convexHull[0], cloudOfPoints[i])) and ((isEqual(p1, cloudOfPoints[i]) == false) || isEqual(convexHull[0], cloudOfPoints[i]))) //or isEqual(convexHull[0], cloudOfPoints[i]) )
		{
			currVector[0] = cloudOfPoints[i][0];
			currVector[1] = cloudOfPoints[i][1];
			return currVector;
		}
	}

	currVector[0] = -2;
	currVector[1] = -2;
	return currVector;
}

float* findPointWithLowestAng(float* base, float* p1)
{
	float* currVector = (float*)malloc(sizeof(float) * 2);
	float* lowestV = (float*)malloc(sizeof(float) * 2);
	float* lowest = findFirstChoice(base, p1);
	float* RES = (float*)malloc(sizeof(float) * 2);

	if (lowest[0] == -2)
	{
		printf("Não achou uma primeira escolha!\n");

	}
	else {
		lowestV[0] = lowest[0] - p1[0];
		lowestV[1] = lowest[1] - p1[1];

		RES[0] = lowest[0];
		RES[1] = lowest[1];

		for (int i = 0; i < currNumOfPointsCP; i++)
		{
			currVector[0] = cloudOfPoints[i][0] - p1[0];
			currVector[1] = cloudOfPoints[i][1] - p1[1];
			if ((vectorialProduct(base, currVector) >= 0) and (findInCH(cloudOfPoints[i]) == false or isEqual(convexHull[0], cloudOfPoints[i])) and ((isEqual(p1, cloudOfPoints[i]) == false))) //or isEqual(convexHull[0], cloudOfPoints[i]))
			{
				if (acos(cosAng(lowestV, base)) > acos(cosAng(currVector, base)))
				{
					lowestV[0] = cloudOfPoints[i][0] - p1[0];
					lowestV[1] = cloudOfPoints[i][1] - p1[1];

					RES[0] = cloudOfPoints[i][0];
					RES[1] = cloudOfPoints[i][1];
				}
			}
		}
	}

	return RES;
}
void jarvisConvexHull(void)
{

	int aux = 0;
	float* lowestY = findPointWithLowestY();

	addPointOnConvexHull(lowestY[0], lowestY[1]); 

	float* base = (float*)malloc(sizeof(float) * 2); 
	float* currPoint = (float*)malloc(sizeof(float) * 2); 
	float* initial = (float*)malloc(sizeof(float) * 2);
	float* p1 = (float*)malloc(sizeof(float) * 2);

	bool firstLoop = true;

	initial[0] = lowestY[0];
	initial[1] = lowestY[1];

	p1[0] = lowestY[0];
	p1[1] = lowestY[1];

	currPoint[0] = -2;
	currPoint[1] = 2;

	base[0] = 1.0 - lowestY[0];
	base[1] = 0.0;

	while (!isEqual(initial, currPoint))
	{
		currPoint = findPointWithLowestAng(base, p1);

		addPointOnConvexHull(currPoint[0], currPoint[1]); 

		base[0] = currPoint[0] - p1[0];
		base[1] = currPoint[1] - p1[1];

		p1[0] = currPoint[0];
		p1[1] = currPoint[1];
		aux++;
	}

	addPointOnConvexHull(initial[0], initial[1]);
	free(lowestY);
	free(base);
	free(currPoint);
	free(initial);
	free(p1);
}

void mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
		addPointOnCloud(x, y); 
		canDrawPoints = true;
		glutPostRedisplay();
	}
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:             
		exit(0);
		break;

	case 'j':
		jarvisConvexHull();
		canDrawCH = true;
		canDrawPoints = false;
		glutPostRedisplay();
		break;
	}
}

void displayFunc(void)
{
	srand(time(NULL));
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glPointSize(6);

	if (canDrawPoints == true) 
	{
		glBegin(GL_POINTS); 
		
		float r = (rand() % 10 + 1) / 10.0;
		float g = (rand() % 10 + 1) / 10.0;
		float b = (rand() % 10 + 1) / 10.0;

		glColor3f(r, g, b);

		for (int i = 0; i < currNumOfPointsCP; i++)
		{
			glVertex2f(cloudOfPoints[i][0], cloudOfPoints[i][1]);
		}

		glEnd();
	}
	if (canDrawCH)
	{
		glBegin(GL_LINE_STRIP);

		float r = (rand() % 10 + 1) / 10.0;
		float g = (rand() % 10 + 1) / 10.0;
		float b = (rand() % 10 + 1) / 10.0;

		glColor3f(r, g, b);

		for (int i = 0; i < currNumOfPointsCH; i++)
		{
			glVertex2f(convexHull[i][0], convexHull[i][1]);
		}

		glEnd();

		glBegin(GL_POINTS);

		 r = (rand() % 10 + 1) / 10.0;
		 g = (rand() % 10 + 1) / 10.0;
		 b = (rand() % 10 + 1) / 10.0;

		glColor3f(r, g, b);

		for (int i = 0; i < currNumOfPointsCP; i++)
		{
			if(findInCH(cloudOfPoints[i]) == false)
				glVertex2f(cloudOfPoints[i][0], cloudOfPoints[i][1]);
		}

		glEnd();
	}


	glFlush();
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	cloudOfPoints = (float **)malloc(sizeof(float) * maxNumOfPointsCP);

	for (int i = 0; i < maxNumOfPointsCP; i++)
	{
		cloudOfPoints[i] = (float *)malloc(sizeof(float) * 2);
	}

	convexHull = (float **)malloc(sizeof(float) * maxNumOfPointsCH);

	for (int i = 0; i < maxNumOfPointsCP; i++)
	{
		convexHull[i] = (float *)malloc(sizeof(float) * 2);
	}

	glutInit(&argc, argv); 
	glutInitWindowSize(dimX, dimY); 
	glutCreateWindow("Convex Hull"); 
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	glutDisplayFunc(displayFunc);
	glutMainLoop(); 

	freeMatrix(cloudOfPoints, maxNumOfPointsCP); 
	freeMatrix(convexHull, maxNumOfPointsCH);
	return 0;
}