// Author: Tainá Lima

// --------------------------- BIBLIOTECAS --------------------------------
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// --------------------------- VARIÁVEIS --------------------------------
// Declaração das variáveis de dimensão da janela
int dimX = 400;
int dimY = 400;

// Declaração das variáveis relacionadas aos pontos 
float** points; // Guarda as coordenadas dos pontos em relação ao OpenGL
int currNumOfPoints = 0; // Guarda a quantidade de pontos atuais desenhados
int maxNumOfPoints = 4; // Guarda o número máximo que a matriz de pontos pode ter

float specialPoint[2] = { 2, 2 }; // Variável que representa o ponto no qual quero saber
								// se está dentro ou fora do polígono
// Declaração de variáveis de controle
bool drawLine = false; // Indica se uma linha deve ser desenhada
bool closed = false; // Indica se o polígono já foi fechado ou ainda não

// --------------------------- FUNÇÕES --------------------------------
// Função resizePointsMatriz
// Parâmetros: nenhum
// Objetivo: Redimensionar a matriz "points", dobrando o tamanho da mesma
void resizePointsMatrix(void)
{
	points = (float **)realloc(points, sizeof(int)*maxNumOfPoints * 2); // Redimensionamento e a realocação dos dados que 
																		// já existiam em "points"

	for (int i = maxNumOfPoints; i < 2 * maxNumOfPoints; i++)
	{
		points[i] = (float *)malloc(sizeof(float) * 2); // Alocação de espaço para os novos dados
	}

	maxNumOfPoints *= 2;
}

// Função distance
// Parâmetros: x1, y1, x2, y2
// Objetivo: Calcular a distância entre (x1, y1) e (x2, y2)
float distance(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
}

// Função scalarProd
// Parâmetros: a e b
// Objetivo: Calcula o produto escalar entre os vetores a e b
float scalarProd(float* a, float*b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// Função vectorProd
// Parâmetros: a, b e result
// Objetivo: Calcula o produto vetorial entre a e b, colocando o resultado em result
void vectorProd(float* a, float* b, float * result)
{
	result[0] = a[1] * b[2] - a[2] * b[1];
	result[1] = a[2] * b[0] - a[0] * b[2];
	result[2] = a[0] * b[1] - a[1] * b[0];
}

// Função drawClosedPolygonal
// Parâmetros: x e y
// Objetivo: Função para desenhar o polígono fechado
void drawClosedPolygonal(int x, int y)
{
	if (currNumOfPoints + 1 >= maxNumOfPoints) // Caso o número atual de pontos esteja próximo ou igual ao máximo permitido
	{
		resizePointsMatrix(); // O redimensionamento da matriz é feito
	}

	if (closed == false) // Caso o polígono ainda não tenha sido fechado, eu permaneço fazendo linhas
	{
		// Converte as coordenadas do ponto clicado e guarda em "points"
		points[currNumOfPoints][0] = ((2 * x) / (float)dimX) - 1.0;
		points[currNumOfPoints][1] = -((2 * y) / (float)dimY) + 1.0;

		currNumOfPoints++; // Incrementa o contador de pontos

		// Caso tenha mais de um ponto e o último clicado esteja a uma distância menor que 0.1,
		// então faço esse ponto ter as mesmas coordenadas q o primeiro, ou seja fecho o polígono
		if ((currNumOfPoints > 1) && (distance(points[0][0], points[0][1], points[currNumOfPoints-1][0], points[currNumOfPoints-1][1]) < 0.1))
		{
			// Faço o último ponto ser o primeiro
			points[currNumOfPoints - 1][0] = points[0][0];
			points[currNumOfPoints - 1][1] = points[0][1];
			closed = true; // Indico que agora o polígono está fechado
			glutPostRedisplay(); // Chama a função displayFunc novamente
		}
		else if (currNumOfPoints > 1) // Caso contrário
		{
			drawLine = true; // Habilita a criação de linhas
			glutPostRedisplay(); // Chama a função displayFunc novamente
		}
	}
	else // Caso o polígono já esteja fechado
	{
		// Guardo as coordenadas do próximo ponto que for clicado, pois ele é quem
		// quero saber se está dentro ou fora do polígono
		specialPoint[0] = ((2 * x) / (float)dimX) - 1.0; 
		specialPoint[1] = -((2 * y) / (float)dimY) + 1.0;
		glutPostRedisplay(); // Chama a função displayFunc novamente
	}
}

// Função pointLocalization
// Parâmetros: nenhum
// Objetivo: Função para dizer se o ponto em questão está dentro ou fora do polígono
void pointLocalization(void)
{
	int numOfCuts = 0; // Inicializo o contador de interseções 

	float qPoint[3] = {1.0, 0.0, 0.0}; // Escolho um ponto Q fora do polígono para formar
									  // a reta PQ
	//Calculo o vetor PQ
	float pqVector[3] = { qPoint[0] - specialPoint[0], qPoint[1] - specialPoint[1], 0.0 }; 
	// Variáveis que representam os vetores PD, PC, CD, CP e CQ, onde P e Q são o A e B 
	// do exemplo dado em aula
	float pdVector[3], pcVector[3], cdVector[3], cpVector[3], cqVector[3];
	// Variáveis que representam os vetores resultantes dos produtos vetoriais
	float vProd1[3], vProd2[3], vProd3[3], vProd4[3];

	// Como tudo é no R2, a terceira coordenada é nula (o produto vetorial só faz
	// sentido no R3)
	pdVector[2] = pcVector[2] = cdVector[2] = cpVector[2] = cqVector[2] = 0.0;

	for (int i = 0; i < currNumOfPoints-1; i++) // Percorre a matriz dos pontos
	{
		// Calculo os vetores necessários
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

		// Se (PQxPD) . (PQxPC) < 0 e (CDxCP) . (CDxCQ) < 0 então temos uma interseção
		if ((scalarProd(vProd1, vProd2) < 0) and (scalarProd(vProd3, vProd4) < 0))
		{
			numOfCuts++;
		}
	}

	if (numOfCuts % 2 == 0) // Se o número de interseções da reta PQ com as outras arestas
							// do polígono for par, então o ponto está fora
	{
		printf("O ponto P = (%f,%f) esta fora do poligono desenhado.\n", specialPoint[0], specialPoint[1]);
	}
	else // Caso contrário, ele está dentro
	{
		printf("O ponto P = (%f,%f) esta dentro do poligono desenhado.\n", specialPoint[0], specialPoint[1]);
	}
}

// Função mouseFunc
// Parâmetros: button (diz qual botão que sofreu a ação), state (diz qual foi a ação), x (coordenada x do lugar que sofreu
// a ação) e y (coordenada y do lugar que sofreu a ação)
// Objetivo: A função pode "disparar" uma espécie de evento que pode ocorrer ao realizar alguma ação com o mouse
void mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // Caso o botão esquerdo tenha sido apertado
	{
		drawClosedPolygonal(x, y); // Chama a função para desenhar o polígono
		// Caso o polígono ja esteja fechado e já tem algum ponto que desejo descobrir sua localização
		if (closed == true && (specialPoint[0] != 2.0 && specialPoint[1] != 2.0)) 
		{
			pointLocalization(); // Chamo a função pra encontrar a localização do ponto
		}
		
	}
}

// Função displayFunc
// Parâmetros: nenhum
// Objetivo: Mostra tudo que é criado dentro dela na janela
void displayFunc(void)
{
	srand(time(NULL));
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glPointSize(5.0);

	glLineWidth(3);

	if (drawLine == true or closed == true) // Se quero desenhar linhas ou se o polígono ja está fechdao
	{
		glBegin(GL_LINE_STRIP); // Ligo os pontos guardados em points por linhas (a cada 2 pontos uma linha é criada)

		// Gera valores randômicos entre 0.1 e 1.0 para fazer linhas de diferentes cores
		float r = (rand() % 10 + 1) / 10.0;
		float g = (rand() % 10 + 1) / 10.0;
		float b = (rand() % 10 + 1) / 10.0;

		glColor3f(r, g, b);

		for (int i = 0; i < currNumOfPoints; i++)
		{

			glVertex2f(points[i][0], points[i][1]);
		}

		glEnd();

		if (closed == true) // Se o polígono está fechado
		{
			// Desenho o próximo ponto na janela
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

	// Criação e alocação da matriz points
	points = (float **)malloc(sizeof(float) * maxNumOfPoints);

	for (int i = 0; i < maxNumOfPoints; i++)
	{
		points[i] = (float *)malloc(sizeof(float) * 2);
	}

	glutInit(&argc, argv); // Inicializo o glut
	glutInitWindowSize(dimX, dimY); // Defino o tamanho da janela
	glutCreateWindow("In or out of the polygonal?"); // Crio a janela cujo título é "In or out of the polygonal?"
											 // Associação entre dispositivos e o glut
	glutMouseFunc(mouseFunc);
	glutDisplayFunc(displayFunc);
	glutMainLoop(); // Mantém a main em "loop" até que a janela seja fechada ou um erro ocorra 

	//free(specialPoint);
	free(points); // Libera o espaço criado para a matriz points
	return 0;
}