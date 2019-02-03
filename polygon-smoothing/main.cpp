// Author: Tain� Lima

// --------------------------- BIBLIOTECAS --------------------------------
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>

// --------------------------- VARI�VEIS --------------------------------
// Declara��o das vari�veis de dimens�o da janela
int dimX = 400;
int dimY = 400;

// Declara��o das vari�veis relacionadas aos pontos 
float** points; // Guarda as coordenadas dos pontos em rela��o ao OpenGL
int currNumOfPoints = 0; // Guarda a quantidade de pontos atuais desenhados
int maxNumOfPoints = 4; // Guarda o n�mero m�ximo que a matriz de pontos pode ter

// Declara��o de vari�veis de controle
bool drawLine = false; // Indica que uma linha deve ser desenhada
bool smoth = false; // Indica que uma suaviza��o deve ser feita

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

// Fun��o drawPolygonal
// Par�metros: x e y (coordenadas do ponto onde recebeu o clique do mouse)
// Objetivo: Al�m de converter as coordenadas do ponto e guard�-las na matriz, habilita o desenho 
// das linhas, quando o n�mero de pontos � maior que 1
void drawPolygonal(int x, int y)
{
	if (currNumOfPoints + 1 >= maxNumOfPoints) // Caso o n�mero atual de pontos esteja pr�ximo ou igual ao m�ximo permitido
	{
		resizePointsMatrix(); // O redimensionamento da matriz � feito
	}

	// Converte as coordenadas do ponto clicado e guarda em "points"
	points[currNumOfPoints][0] = ((2 * x) / (float)dimX) - 1.0; 
	points[currNumOfPoints][1] = -((2 * y) / (float)dimY) + 1.0;

	currNumOfPoints++;

	if (currNumOfPoints > 1) // Caso exista mais de um ponto clicado, crio linhas
	{
		drawLine = true; // Habilita a cria��o de linhas
		glutPostRedisplay(); // Chama a fun��o displayFunc novamente
	}
}

// Fun��o polygonalSmoothing
// Par�metros: void
// Objetivo: Divide cada linha em quatro partes (utilizando o conceito de ponto m�dio de um segmento de reta), gerando tr�s
// pontos a mais para cada reta, "substituindo" estes pelo ponto que formava um "bico", fazendo com que estes �ltimo se
// pare�am mais com curvas.
void polygonalSmoothing(void)
{
	float ** newPoints = (float **)malloc(sizeof(float) * currNumOfPoints * 3 - 1); // Cria��o e aloca��o da nova matriz points
	// Cria��o de vari�veis auxiliares/locais
	float ** aux;
	int temp = 0, temp2 = 1;
	float xA, yA; 
	float xB, yB;
	float xM, yM;

	for (int i = 0; i < 3 * currNumOfPoints - 1; i++) 
	{
		newPoints[i] = (float *)malloc(sizeof(float) * 2);
	}

	// Coloco o primeiro ponto do desenho atual na nova matriz
	newPoints[0][0] = points[0][0];
	newPoints[0][1] = points[0][1];

	while (temp < currNumOfPoints - 1) // Enquanto a itera��o n�o chegar no pen�ltimo ponto
	{
		// Coordenadas de A
		xA = points[temp][0];
		yA = points[temp][1];

		// Coordenadas de B
		xB = points[temp + 1][0];
		yB = points[temp + 1][1];

		//Coordenadas do ponto m�dio M entre A e B
		xM = (xA + xB) / 2.0;
		yM = (yA + yB) / 2.0;

		//Colocando as coordendas ponto m�dio M1 entre A e M na nova matriz de pontos
		newPoints[temp2][0] = (xA + xM) / 2.0;
		newPoints[temp2][1] = (yA + yM) / 2.0;

		//Colocando as coordendas ponto m�dio M entre A e B na nova matriz de pontos
		newPoints[temp2 + 1][0] = xM;
		newPoints[temp2 + 1][1] = yM;

		//Colocando as coordendas ponto m�dio M2 entre M e B na nova matriz de pontos
		newPoints[temp2 + 2][0] = (xB + xM) / 2.0;
		newPoints[temp2 + 2][1] = (yB + yM) / 2.0;

		temp++;
		temp2 += 3;
	}

	// Coloco o �ltimo ponto do desenho atual na nova matriz 
	newPoints[temp2][0] = points[currNumOfPoints - 1][0];
	newPoints[temp2][1] = points[currNumOfPoints - 1][1];

	// Altero os valores dessas vari�veis para manter a coer�ncia 
	currNumOfPoints = maxNumOfPoints = 3 * currNumOfPoints - 1;

	// Guardo o endere�o da antiga matriz points para poder liberar esse espa�o
	aux = points;
	// Fa�o points apontar para a nova matriz
	points = newPoints;

	// Libero o espa�o da matriz points antiga
	free(aux);
}

// Fun��o mouseFunc
// Par�metros: button (diz qual bot�o que sofreu a a��o), state (diz qual foi a a��o), x (coordenada x do lugar que sofreu
// a a��o) e y (coordenada y do lugar que sofreu a a��o)
// Objetivo: A fun��o pode "disparar" uma esp�cie de evento que pode ocorrer ao realizar alguma a��o com o mouse
void mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // Caso o bot�o esquerdo tenha sido apertado
	{
		drawPolygonal(x, y); // Uma linha pode ser desenhada (a linha ser� desenhada de fato caso n�o seja o primeiro clique)
	}
}

// Fun��o keyboardFunc
// Par�metros: key (qual tecla foi apertada), x e y
// Objetivo: A fun��o pode "disparar" uma esp�cie de evento que pode ocorrer ao realizar alguma a��o com o teclado 
void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:             // ESCAPE key
			exit(0);
			break;

		case 's':
			polygonalSmoothing();
			smoth = true;
			glutPostRedisplay();
			break;
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

	glLineWidth(3);

	if (drawLine == true or smoth == true) // Se quero desenhar linhas ou suavizar essas linhas
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

		smoth = false; // Indica que n�o quero mais suavizar (caso previamente essa vari�vel fosse true)
	}

		
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char **argv) 
{
	// Cria��o e aloca��o da matriz points
	points = (float **) malloc(sizeof(float) * maxNumOfPoints);

	for (int i = 0; i < maxNumOfPoints; i++)
	{
		points[i] = (float *)malloc(sizeof(float) * 2);
	}

	glutInit(&argc, argv); // Inicializo o glut
		glutInitWindowSize(dimX, dimY); // Defino o tamanho da janela
	glutCreateWindow("Polygon Smoothing"); // Crio a janela cujo t�tulo � "Polygon Smoothing"
	// Associa��o entre dispositivos e o glut
	glutKeyboardFunc(keyboardFunc); 
	glutMouseFunc(mouseFunc);
	glutDisplayFunc(displayFunc);
	glutMainLoop(); // Mant�m a main em "loop" at� que a janela seja fechada ou um erro ocorra 

	free(points); // Libera o espa�o criado para a matriz points
	return 0;
}