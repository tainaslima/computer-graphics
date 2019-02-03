#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

typedef struct HalfEdge
{
	vector<int> H;
	vector<int> O;
	int curr = 0;
}HalfEdge;

int dimX = 400;
int dimY = 400;

HalfEdge he;
vector<vector<int>> queue;
vector<vector<int>> freeEdges;

float** cloudOfPoints;
int currNumOfPointsCP = 0;
int maxNumOfPointsCP = 4;

float** convexHull;
int currNumOfPointsCH = 0;
int maxNumOfPointsCH = 4;

bool canDrawPoints = false;
bool canDrawCH = false;
bool canDrawDT = false;
bool isTheFirst = false;


void printHalfEdge(void)
{
	for (int i = 0; i < (int)he.H.size(); i += 3)
	{
		for (int k = i; k < i + 3; k++)
		{
			printf("%d ", he.H[k]);
		}
		printf("\n");
	}
}

void printCloudOfPoints(void)
{
	for (int i = 0; i < currNumOfPointsCP; i++)
	{
		printf("cloudOfPoints[%d] = %f %f\n", i, cloudOfPoints[i][0], cloudOfPoints[i][1]);
	}
}

void printQueue(void)
{
	for (int i = 0; i < (int)queue.size(); i++)
	{
		printf("Triangulo %d = { ", i);
		for (int k = 0; k < (int)queue[i].size(); k++)
		{
			printf("%d ", queue[i][k]);
		}
		printf(" }\n");
	}
}

void printFreeEdges(void)
{
	for (int i = 0; i < (int)freeEdges.size(); i++)
	{
		printf("Aresta %d = { ", i);
		for (int k = 0; k < (int)freeEdges[i].size(); k++)
		{
			printf("%d ", freeEdges[i][k]);
		}
		printf(" }\n");
	}
}

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

int findIndexCP(float x, float y)
{
	for (int i = 0; i < currNumOfPointsCP; i++)
	{
		if ((cloudOfPoints[i][0] == x) && (cloudOfPoints[i][1] == y))
		{
			return i;
		}
	}

	printf("O ponto não está na nuvem de pontos.\n");
	return -1;
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

bool isIntercepting(float* a, float* s)
{
	float* saVector = (float *)malloc(sizeof(float) * 2);
	float* stVector = (float *)malloc(sizeof(float) * 2);
	float* sqVector = (float *)malloc(sizeof(float) * 2);
	float* tqVector = (float *)malloc(sizeof(float) * 2);
	float* tsVector = (float *)malloc(sizeof(float) * 2);
	float* taVector = (float *)malloc(sizeof(float) * 2);

	saVector[0] = a[0] - s[0];
	saVector[1] = a[1] - s[1];

	for (int i = 0; i < (int)he.H.size(); i += 3)
	{
		for (int k = i; k < i+3; k++)
		{
			if (!isEqual(s, cloudOfPoints[he.H[k]]))
			{
				if (k == i + 2)
				{
					stVector[0] = cloudOfPoints[he.H[k]][0] - s[0];
					stVector[1] = cloudOfPoints[he.H[k]][1] - s[1];

					sqVector[0] = cloudOfPoints[he.H[i]][0] - s[0];
					sqVector[1] = cloudOfPoints[he.H[i]][1] - s[1];

					tqVector[0] = cloudOfPoints[he.H[i]][0] - cloudOfPoints[he.H[k]][0];
					tqVector[1] = cloudOfPoints[he.H[i]][0] - cloudOfPoints[he.H[k]][1];

					tsVector[0] = -stVector[0];
					tsVector[1] = -stVector[1];

					taVector[0] = a[0] - cloudOfPoints[he.H[k]][0];
					taVector[1] = a[1] - cloudOfPoints[he.H[k]][1];


					float prodVet1 = vectorialProduct(stVector, saVector);
					float prodVet2 = vectorialProduct(sqVector, saVector);
					float prodVet3 = vectorialProduct(tsVector, tqVector);
					float prodVet4 = vectorialProduct(taVector, tqVector);

					if ((prodVet1*prodVet2 < 0) and (prodVet3*prodVet4 < 0))
					{
						free(saVector);
						free(stVector);
						free(sqVector);
						free(tqVector);
						free(tsVector);
						free(taVector);
						return true;
					}
				}
				else
				{
					stVector[0] = cloudOfPoints[he.H[k]][0] - s[0];
					stVector[1] = cloudOfPoints[he.H[k]][1] - s[1];

					sqVector[0] = cloudOfPoints[he.H[k + 1]][0] - s[0];
					sqVector[1] = cloudOfPoints[he.H[k + 1]][1] - s[1];

					tqVector[0] = cloudOfPoints[he.H[k + 1]][0] - cloudOfPoints[he.H[k]][0];
					tqVector[1] = cloudOfPoints[he.H[k + 1]][0] - cloudOfPoints[he.H[k]][1];

					tsVector[0] = -stVector[0];
					tsVector[1] = -stVector[1];

					taVector[0] = a[0] - cloudOfPoints[he.H[k]][0];
					taVector[1] = a[1] - cloudOfPoints[he.H[k]][1];


					float prodVet1 = vectorialProduct(stVector, saVector);
					float prodVet2 = vectorialProduct(sqVector, saVector);
					float prodVet3 = vectorialProduct(tsVector, tqVector);
					float prodVet4 = vectorialProduct(taVector, tqVector);

					if ((prodVet1*prodVet2 < 0) and (prodVet3*prodVet4 < 0))
					{
						free(saVector);
						free(stVector);
						free(sqVector);
						free(tqVector);
						free(tsVector);
						free(taVector);
						return true;
					}


				}
			}
			
		}
	}

	free(saVector);
	free(stVector);
	free(sqVector);
	free(tqVector);
	free(tsVector);
	free(taVector);
	return false;
}

float* findFirstChoice2(float* a, float* b, float* c)
{
	if (isTheFirst)
	{
		for (int i = 0; i < currNumOfPointsCP; i++)
		{
			if ((!isEqual(cloudOfPoints[i], a)) && (!isEqual(cloudOfPoints[i], b)))
			{
				return cloudOfPoints[i];
			}
		}
	}
	else {
		for (int i = 0; i < currNumOfPointsCP; i++)
		{
			if ((!isEqual(cloudOfPoints[i], a)) && (!isEqual(cloudOfPoints[i], b)) && (!isEqual(cloudOfPoints[i], c)))
			{
				return cloudOfPoints[i];
			}
		}
	}
	
	return NULL;
}

bool isInsideTriangle(float* p)
{
	int numOfCuts = 0;

	float qPoint[3] = { 1.0, 0.0 };

	float pqVector[2] = { qPoint[0] - p[0], qPoint[1] - p[1] };

	float pdVector[2], pcVector[2], cdVector[2], cpVector[2], cqVector[2];

	float vProd1, vProd2, vProd3, vProd4;

	for (int i = 0; i < (int)he.H.size(); i+=3)
	{
		vector<float*> t{cloudOfPoints[he.H[i]], cloudOfPoints[he.H[i+1]] , cloudOfPoints[he.H[i+2]] };

		for (int k = 0; k < 3; k++)
		{
			if (k == 2)
			{
				pdVector[0] = -p[0] + t[k][0];
				pdVector[1] = -p[1] + t[k][1];

				pcVector[0] = -p[0] + t[0][0];
				pcVector[1] = -p[1] + t[0][1];

				cdVector[0] = t[0][0] - t[k][0];
				cdVector[1] = t[0][1] - t[k][1];

				cpVector[0] = -pcVector[0];
				cpVector[1] = -pcVector[1];

				cqVector[0] = -t[0][0] + qPoint[0];
				cqVector[1] = -t[0][1] + qPoint[1];

				vProd1 = vectorialProduct(pqVector, pdVector);
				vProd2 = vectorialProduct(pqVector, pcVector);
				vProd3 = vectorialProduct(cdVector, cpVector);
				vProd4 = vectorialProduct(cdVector, cqVector);

			}
			else
			{
				pdVector[0] = -p[0] + t[k][0];
				pdVector[1] = -p[1] + t[k][1];

				pcVector[0] = -p[0] + t[k + 1][0];
				pcVector[1] = -p[1] + t[k + 1][1];

				cdVector[0] = t[k + 1][0] - t[k][0];
				cdVector[1] = t[k + 1][1] - t[k][1];

				cpVector[0] = -pcVector[0];
				cpVector[1] = -pcVector[1];

				cqVector[0] = -t[k + 1][0] + qPoint[0];
				cqVector[1] = -t[k + 1][1] + qPoint[1];

				vProd1 = vectorialProduct(pqVector, pdVector);
				vProd2 = vectorialProduct(pqVector, pcVector);
				vProd3 = vectorialProduct(cdVector, cpVector);
				vProd4 = vectorialProduct(cdVector, cqVector);

			}

			if ((vProd1*vProd2 < 0) and (vProd3*vProd4 < 0))
			{
				numOfCuts++;
			}
		}

		if (numOfCuts % 2 != 0)
		{
			return true;
		}

		numOfCuts = 0;
	}

	return false;
}

float* findPointWithHighestAng(float* a, float* b, float* c)
{
	float currVectorA[2];
	float currVectorB[2];

	float highestVectorA[2];
	float highestVectorB[2];

	float* highest = (float*) malloc (sizeof(float)*2);

	float*h = (isTheFirst)? findFirstChoice2(a, b, NULL): findFirstChoice2(a, b, c);
	
	highest[0] = h[0];
	highest[1] = h[1];

	highestVectorA[0] = a[0] - highest[0];
	highestVectorA[1] = a[1] - highest[1];

	highestVectorB[0] = b[0] - highest[0];
	highestVectorB[1] = b[1] - highest[1];

	for (int i = 0; i < currNumOfPointsCP; i++)
	{
		if (isTheFirst)
		{
			if ((!isEqual(cloudOfPoints[i], a) && (!isEqual(cloudOfPoints[i], b))))
			{
				currVectorA[0] = a[0] - cloudOfPoints[i][0];
				currVectorA[1] = a[1] - cloudOfPoints[i][1];

				currVectorB[0] = b[0] - cloudOfPoints[i][0];
				currVectorB[1] = b[1] - cloudOfPoints[i][1];

				if ((!isIntercepting(a, cloudOfPoints[i]) && !isIntercepting(b, cloudOfPoints[i])) && !(isInsideTriangle(cloudOfPoints[i])
					and isInsideTriangle(a) and isInsideTriangle(b)))
				{
					if (acos(cosAng(currVectorA, currVectorB)) > acos(cosAng(highestVectorA, highestVectorB)))
					{
						highestVectorA[0] = currVectorA[0];
						highestVectorA[1] = currVectorA[1];

						highestVectorB[0] = currVectorB[0];
						highestVectorB[1] = currVectorB[1];

						highest[0] = cloudOfPoints[i][0];
						highest[1] = cloudOfPoints[i][1];
					}
				}
				
			}
		}
		else
		{
			if ((!isEqual(cloudOfPoints[i], a)) && (!isEqual(cloudOfPoints[i], b)) && (!isEqual(cloudOfPoints[i], c)))
			{
				currVectorA[0] = a[0] - cloudOfPoints[i][0];
				currVectorA[1] = a[1] - cloudOfPoints[i][1];

				currVectorB[0] = b[0] - cloudOfPoints[i][0];
				currVectorB[1] = b[1] - cloudOfPoints[i][1];

			if ((!isIntercepting(a, cloudOfPoints[i]) && !isIntercepting(b, cloudOfPoints[i])) && !(isInsideTriangle(cloudOfPoints[i])
				and isInsideTriangle(a) and isInsideTriangle(b)))
			{
					if (acos(cosAng(currVectorA, currVectorB)) > acos(cosAng(highestVectorA, highestVectorB)))
					{
						highestVectorA[0] = currVectorA[0];
						highestVectorA[1] = currVectorA[1];

						highestVectorB[0] = currVectorB[0];
						highestVectorB[1] = currVectorB[1];

						highest[0] = cloudOfPoints[i][0];
						highest[1] = cloudOfPoints[i][1];
				
					}
			}
			}
		}
		
	}
	
	return highest;
}

bool isInTriangle(vector<int> triangle, vector<int> edge)
{
	for (int i = 0; i < (int)triangle.size(); i++)
	{
		if (triangle[i] == edge[0])
		{
			for (int j = 0; j < (int)triangle.size(); j++)
			{
				if (triangle[j] == edge[1])
					return true;
			}
			return false;
		}
		if (triangle[i] == edge[1])
		{
			for (int j = 0; j < (int)triangle.size(); j++)
			{
				if (triangle[j] == edge[0])
					return true;
			}
			return false;
		}
	}

	return false;
}

bool isInCH(vector<int> edge)
{
	for (int i = 0; i < currNumOfPointsCH - 1; i++)
	{
		if ((isEqual(convexHull[i], cloudOfPoints[edge[0]]) && isEqual(convexHull[i + 1], cloudOfPoints[edge[1]])) or (isEqual(convexHull[i], cloudOfPoints[edge[1]]) && isEqual(convexHull[i + 1], cloudOfPoints[edge[0]])))
			return true;
	}

	return false;
}

void findFirstTriangle(void)
{
	isTheFirst = true;
	float point[2];
	float* s = findPointWithHighestAng(convexHull[0], convexHull[1], NULL);

	point[0] = s[0];
	point[1] = s[1];

	//printCloudOfPoints();

	int index1 = findIndexCP(convexHull[0][0], convexHull[0][1]);
	int index2 = findIndexCP(point[0], point[1]);
	int index3 = findIndexCP(convexHull[1][0], convexHull[1][1]);
	vector<int> a{index1 ,index2 ,  index3 };
	vector<int> b{ index2, index3 };
	vector<int> c{ index1, index2 };

	he.H.push_back(index1);
	he.H.push_back(index2);
	he.H.push_back(index3);

	he.curr += 3;

	queue.push_back(a);


	if ((!isInCH(b))) freeEdges.push_back(b);
	if ((!isInCH(c))) freeEdges.push_back(c);

	isTheFirst = false;
}

void findNextTriangle(vector<int> prevTriangle, vector<int> edge)
{
	int thirdPoint;
	float point[2];

	for (int k = 0; k < (int)prevTriangle.size(); k++)
	{
		if ((prevTriangle[k] != edge[0]) && (prevTriangle[k] != edge[1]))
		{
			thirdPoint = prevTriangle[k];
			break;
		}
	}

	float* s = findPointWithHighestAng(cloudOfPoints[edge[0]], cloudOfPoints[edge[1]], cloudOfPoints[thirdPoint]);

	point[0] = s[0];
	point[1] = s[1];

//	printCloudOfPoints();

	int index2 = findIndexCP(point[0], point[1]);
	vector<int> a{ edge[0], index2, edge[1] };
	vector<int> b{ index2,  edge[1] };
	vector<int> c{ edge[0], index2};

	queue.push_back(a);

//	printQueue();

	bool isNotTB = true;
	bool isNotTC = true;

	for (int i = 0; i < (int)he.H.size(); i += 3)
	{
		vector<int> t{ he.H[i], he.H[i + 1], he.H[i + 2] };
		if (isInTriangle(t, b))
		{
			isNotTB = false;
		}
		if (isInTriangle(t, c))
		{
			isNotTC = false;
		}
	}

	he.H.push_back(a[0]);
	he.H.push_back(a[1]);
	he.H.push_back(a[2]);

//	printHalfEdge();

	if(isNotTB and (!isInCH(b))) 
		freeEdges.push_back(b);
	if (isNotTC and (!isInCH(c)))
		freeEdges.push_back(c);
	
//	printFreeEdges();
	
}

bool isInAnotherTriangle(vector<int> edge, vector<int> currT)
{
	bool isTEqual = false;
	for (int i = 0; i < (int)he.H.size(); i += 3)
	{
		vector<int> t{he.H[i], he.H[i+1], he.H[i+2]};
		
		for (int k = 0; k < 3; k++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (currT[k] == t[j])
				{
					isTEqual = true;
				}
			}

			if (isTEqual == false)
			{
				break;
			}
			else if ((isTEqual == true) and (k != 2))
			{
				isTEqual = false;
			}
		}

		if (!isTEqual)
		{
			if (isInTriangle(t, edge))
				return true;
		}

		isTEqual = false;
		
	}

	return false;
}

void delaunayTriangulation(void)
{
	jarvisConvexHull();
	canDrawCH = true;
	findFirstTriangle();
	printHalfEdge();
	printQueue();
	printFreeEdges();
	while (!queue.empty())
	{
		while ((!freeEdges.empty()) && (isInTriangle(queue[0], freeEdges[0])))
		{
			if (!isInAnotherTriangle(freeEdges[0], queue[0]) && !isInCH(freeEdges[0]))
			{
				findNextTriangle(queue[0], freeEdges[0]);
//				printHalfEdge();
			}
			freeEdges.erase(freeEdges.begin());
//			printFreeEdges();
		}
		queue.erase(queue.begin());
//		printQueue();
	}
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

	case 'd':
		delaunayTriangulation();
		canDrawDT = true;
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
			if (findInCH(cloudOfPoints[i]) == false)
				glVertex2f(cloudOfPoints[i][0], cloudOfPoints[i][1]);
		}

		glEnd();
	}
	if (canDrawDT)
	{
		string str;

		for (int i = 0; i < (int)he.H.size(); i += 3)
		{
			glBegin(GL_LINE_STRIP);

			float r = (rand() % 10 + 1) / 10.0;
			float g = (rand() % 10 + 1) / 10.0;
			float b = (rand() % 10 + 1) / 10.0;

			glColor3f(r, g, b);

			for (int k = i; k < i+3; k++)
			{

				glVertex2f(cloudOfPoints[he.H[k]][0], cloudOfPoints[he.H[k]][1]);
			}

			glEnd();
		}

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
	glutCreateWindow("Delaunay Triangulation");
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	glutDisplayFunc(displayFunc);
	glutMainLoop();

	freeMatrix(cloudOfPoints, maxNumOfPointsCP);
	freeMatrix(convexHull, maxNumOfPointsCH);
 
	return 0;
}