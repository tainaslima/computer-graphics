// Author: Tain� Lima

// --------------------------- BIBLIOTECAS --------------------------------
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// --------------------------- VARI�VEIS --------------------------------
// Declara��o das vari�veis de dimens�o da janela
int dimX = 400;
int dimY = 400;

// Declara��o das vari�veis relacionadas aos pontos 
float** points; // Guarda as coordenadas dos pontos em rela��o ao OpenGL
int currNumOfPoints = 0; // Guarda a quantidade de pontos atuais desenhados
int maxNumOfPoints = 4; // Guarda o n�mero m�ximo que a matriz de pontos pode ter

float specialPoint[2] = { 2, 2 }; // Vari�vel que representa o ponto no qual quero saber
								// se est� dentro ou fora do pol�gono
// Declara��o de vari�veis de controle
bool drawLine = false; // Indica se uma linha deve ser desenhada
bool closed = false; // Indica se o pol�gono j� foi fechado ou ainda n�o

// --------------------------- FUN��ES --------------------------------
// Fun��o resizePointsMatriz
// Par�metros: nenhum
// Objetivo: Redimensionar a matriz "points", dobrando o tamanho da mesma
void resizePointsMatrix(void)
{
	points = (float **)realloc(points, sizeof(int)*maxNumOfPoints * 2); // Redimensionamento e a realoca��o dos dados que 
																		// j� existiam em "points"

	for (int i = maxNumOfPoints; i < 2 * maxNumOfPoints; i++)
	{
		points[i] = (float *)malloc(sizeof(float) * 2); // Aloca��o de espa�o para os novos dados
	}

	maxNumOfPoints *= 2;
}

// Fun��o distance
// Par�metros: x1, y1, x2, y2
// Objetivo: Calcular a dist�ncia entre (x1, y1) e (x2, y2)
float distance(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
}

// Fun��o scalarProd
// Par�metros: a e b
// Objetivo: Calcula o produto escalar entre os vetores a e b
float scalarProd(float* a, float*b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// Fun��o vectorProd
// Par�metros: a, b e result
// Objetivo: Calcula o produto vetorial entre a e b, colocando o resultado em result
void vectorProd(float* a, float* b, float * result)
{
	result[0] = a[1] * b[2] - a[2] * b[1];
	result[1] = a[2] * b[0] - a[0] * b[2];
	result[2] = a[0] * b[1] - a[1] * b[0];
}

// Fun��o drawClosedPolygonal
// Par�metros: x e y
// Objetivo: Fun��o para desenhar o pol�gono fechado
void drawClosedPolygonal(int x, int y)
{
	if (currNumOfPoints + 1 >= maxNumOfPoints) // Caso o n�mero atual de pontos esteja pr�ximo ou igual ao m�ximo permitido
	{
		resizePointsMatrix(); // O redimensionamento da matriz � feito
	}

	if (closed == false) // Caso o pol�gono ainda n�o tenha sido fechado, eu permane�o fazendo linhas
	{
		// Converte as coordenadas do ponto clicado e guarda em "points"
		points[currNumOfPoints][0] = ((2 * x) / (float)dimX) - 1.0;
		points[currNumOfPoints][1] = -((2 * y) / (float)dimY) + 1.0;

		currNumOfPoints++; // Incrementa o contador de pontos

		// Caso tenha mais de um ponto e o �ltimo clicado esteja a uma dist�ncia menor que 0.1,
		// ent�o fa�o esse ponto ter as mesmas coordenadas q o primeiro, ou seja fecho o pol�gono
		if ((currNumOfPoints > 1) && (distance(points[0][0], points[0][1], points[currNumOfPoints-1][0], points[currNumOfPoints-1][1]) < 0.1))
		{
			// Fa�o o �ltimo ponto ser o primeiro
			points[currNumOfPoints - 1][0] = points[0][0];
			points[currNumOfPoints - 1][1] = points[0][1];
			closed = true; // Indico que agora o pol�gono est� fechado
			glutPostRedisplay(); // Chama a fun��o displayFunc novamente
		}
		else if (currNumOfPoints > 1) // Caso contr�rio
		{
			drawLine = true; // Habilita a cria��o de linhas
			glutPostRedisplay(); // Chama a fun��o displayFunc novamente
		}
	}
	else // Caso o pol�gono j� esteja fechado
	{
		// Guardo as coordenadas do pr�ximo ponto que for clicado, pois ele � quem
		// quero saber se est� dentro ou fora do pol�gono
		specialPoint[0] = ((2 * x) / (float)dimX) - 1.0; 
		specialPoint[1] = -((2 * y) / (float)dimY) + 1.0;
		glutPostRedisplay(); // Chama a fun��o displayFunc novamente
	}
}

// Fun��o pointLocalization
// Par�metros: nenhum
// Objetivo: Fun��o para dizer se o ponto em quest�o est� dentro ou fora do pol�gono
void pointLocalization(void)
{
	int numOfCuts = 0; // Inicializo o contador de interse��es 

	float qPoint[3] = {1.0, 0.0, 0.0}; // Escolho um ponto Q fora do pol�gono para formar
									  // a reta PQ
	//Calculo o vetor PQ
	float pqVector[3] = { qPoint[0] - specialPoint[0], qPoint[1] - specialPoint[1], 0.0 }; 
	// Vari�veis que representam os vetores PD, PC, CD, CP e CQ, onde P e Q s�o o A e B 
	// do exemplo dado em aula
	float pdVector[3], pcVector[3], cdVector[3], cpVector[3], cqVector[3];
	// Vari�veis que representam os vetores resultantes dos produtos vetoriais
	float vProd1[3], vProd2[3], vProd3[3], vProd4[3];

	// Como tudo � no R2, a terceira coordenada � nula (o produto vetorial s� faz
	// sentido no R3)
	pdVector[2] = pcVector[2] = cdVector[2] = cpVector[2] = cqVector[2] = 0.0;

	for (int i = 0; i < currNumOfPoints-1; i++) // Percorre a matriz dos pontos
	{
		// Calculo os vetores necess�rios
		pdVector[0] = -specialPoint[0] + points[i][0];
		pdVector[1] = -specialPoint[1] + points[i][1];

		pcVector[0] = -specialPoint[0] + points[i+1][0];
		pcVector[1] = -specialPoint[1] + points[i+1][1];

		cdVector[0] = points[i + 1][0] - points[i][0];
		cdVector[1] = points[i + 1][1] - points[i][1];

		cpVector[0] = -pcVector[0];
		cpVector[1] = -pcVector[1];

		cqVector[0] = -points[i + 1][0] + qPoint[0];
		cqVector[1] = -points[i + 1][1] + qPoint[1];
		
		// Calculo os produtos vetoriais
		vectorProd(pqVector, pdVector, vProd1);
		vectorProd(pqVector, pcVector, vProd2);
		vectorProd(cdVector, cpVector, vProd3);
		vectorProd(cdVector, cqVector, vProd4);

		// Se (PQxPD) . (PQxPC) < 0 e (CDxCP) . (CDxCQ) < 0 ent�o temos uma interse��o
		if ((scalarProd(vProd1, vProd2) < 0) and (scalarProd(vProd3, vProd4) < 0))
		{
			numOfCuts++;
		}
	}

	if (numOfCuts % 2 == 0) // Se o n�mero de interse��es da reta PQ com as outras arestas
							// do pol�gono for par, ent�o o ponto est� fora
	{
		printf("O ponto P = (%f,%f) esta fora do poligono desenhado.\n", specialPoint[0], specialPoint[1]);
	}
	else // Caso contr�rio, ele est� dentro
	{
		printf("O ponto P = (%f,%f) esta dentro do poligono desenhado.\n", specialPoint[0], specialPoint[1]);
	}
}

// Fun��o mouseFunc
// Par�metros: button (diz qual bot�o que sofreu a a��o), state (diz qual foi a a��o), x (coordenada x do lugar que sofreu
// a a��o) e y (coordenada y do lugar que sofreu a a��o)
// Objetivo: A fun��o pode "disparar" uma esp�cie de evento que pode ocorrer ao realizar alguma a��o com o mouse
void mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // Caso o bot�o esquerdo tenha sido apertado
	{
		drawClosedPolygonal(x, y); // Chama a fun��o para desenhar o pol�gono
		// Caso o pol�gono ja esteja fechado e j� tem algum ponto que desejo descobrir sua localiza��o
		if (closed == true && (specialPoint[0] != 2.0 && specialPoint[1] != 2.0)) 
		{
			pointLocalization(); // Chamo a fun��o pra encontrar a localiza��o do ponto
		}
		
	}
}

// Fun��o displayFunc
// Par�metros: nenhum
// Objetivo: Mostra tudo que � criado dentro dela na janela
void displayFunc(void)
{
	srand(time(NULL));
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glPointSize(5.0);

	glLineWidth(3);

	if (drawLine == true or closed == true) // Se quero desenhar linhas ou se o pol�gono ja est� fechdao
	{
		glBegin(GL_LINE_STRIP); // Ligo os pontos guardados em points por linhas (a cada 2 pontos uma linha � criada)

		// Gera valores rand�micos entre 0.1 e 1.0 para fazer linhas de diferentes cores
		float r = (rand() % 10 + 1) / 10.0;
		float g = (rand() % 10 + 1) / 10.0;
		float b = (rand() % 10 + 1) / 10.0;

		glColor3f(r, g, b);

		for (int i = 0; i < currNumOfPoints; i++)
		{

			glVertex2f(points[i][0], points[i][1]);
		}

		glEnd();

		if (closed == true) // Se o pol�gono est� fechado
		{
			// Desenho o pr�ximo ponto na janela
				glBegin(GL_POINTS);
				glVertex2f(specialPoint[0], specialPoint[1]);
				glEnd();
		}
	}
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char **argv)
{

	// Cria��o e aloca��o da matriz points
	points = (float **)malloc(sizeof(float) * maxNumOfPoints);

	for (int i = 0; i < maxNumOfPoints; i++)
	{
		points[i] = (float *)malloc(sizeof(float) * 2);
	}

	glutInit(&argc, argv); // Inicializo o glut
	glutInitWindowSize(dimX, dimY); // Defino o tamanho da janela
	glutCreateWindow("In or out of the polygonal?"); // Crio a janela cujo t�tulo � "In or out of the polygonal?"
											 // Associa��o entre dispositivos e o glut
	glutMouseFunc(mouseFunc);
	glutDisplayFunc(displayFunc);
	glutMainLoop(); // Mant�m a main em "loop" at� que a janela seja fechada ou um erro ocorra 

	//free(specialPoint);
	free(points); // Libera o espa�o criado para a matriz points
	return 0;
}