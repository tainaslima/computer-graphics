// Author: Tainá Lima

// --------------------------- BIBLIOTECAS --------------------------------
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>

// --------------------------- VARIÁVEIS --------------------------------
// Declaração das variáveis de dimensão da janela
int dimX = 400;
int dimY = 400;

// Declaração das variáveis relacionadas aos pontos 
float** points; // Guarda as coordenadas dos pontos em relação ao OpenGL
int currNumOfPoints = 0; // Guarda a quantidade de pontos atuais desenhados
int maxNumOfPoints = 4; // Guarda o número máximo que a matriz de pontos pode ter

// Declaração de variáveis de controle
bool drawLine = false; // Indica que uma linha deve ser desenhada
bool smoth = false; // Indica que uma suavização deve ser feita

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

// Função drawPolygonal
// Parâmetros: x e y (coordenadas do ponto onde recebeu o clique do mouse)
// Objetivo: Além de converter as coordenadas do ponto e guardá-las na matriz, habilita o desenho 
// das linhas, quando o número de pontos é maior que 1
void drawPolygonal(int x, int y)
{
	if (currNumOfPoints + 1 >= maxNumOfPoints) // Caso o número atual de pontos esteja próximo ou igual ao máximo permitido
	{
		resizePointsMatrix(); // O redimensionamento da matriz é feito
	}

	// Converte as coordenadas do ponto clicado e guarda em "points"
	points[currNumOfPoints][0] = ((2 * x) / (float)dimX) - 1.0; 
	points[currNumOfPoints][1] = -((2 * y) / (float)dimY) + 1.0;

	currNumOfPoints++;

	if (currNumOfPoints > 1) // Caso exista mais de um ponto clicado, crio linhas
	{
		drawLine = true; // Habilita a criação de linhas
		glutPostRedisplay(); // Chama a função displayFunc novamente
	}
}

// Função polygonalSmoothing
// Parâmetros: void
// Objetivo: Divide cada linha em quatro partes (utilizando o conceito de ponto médio de um segmento de reta), gerando três
// pontos a mais para cada reta, "substituindo" estes pelo ponto que formava um "bico", fazendo com que estes último se
// pareçam mais com curvas.
void polygonalSmoothing(void)
{
	float ** newPoints = (float **)malloc(sizeof(float) * currNumOfPoints * 3 - 1); // Criação e alocação da nova matriz points
	// Criação de variáveis auxiliares/locais
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

	while (temp < currNumOfPoints - 1) // Enquanto a iteração não chegar no penúltimo ponto
	{
		// Coordenadas de A
		xA = points[temp][0];
		yA = points[temp][1];

		// Coordenadas de B
		xB = points[temp + 1][0];
		yB = points[temp + 1][1];

		//Coordenadas do ponto médio M entre A e B
		xM = (xA + xB) / 2.0;
		yM = (yA + yB) / 2.0;

		//Colocando as coordendas ponto médio M1 entre A e M na nova matriz de pontos
		newPoints[temp2][0] = (xA + xM) / 2.0;
		newPoints[temp2][1] = (yA + yM) / 2.0;

		//Colocando as coordendas ponto médio M entre A e B na nova matriz de pontos
		newPoints[temp2 + 1][0] = xM;
		newPoints[temp2 + 1][1] = yM;

		//Colocando as coordendas ponto médio M2 entre M e B na nova matriz de pontos
		newPoints[temp2 + 2][0] = (xB + xM) / 2.0;
		newPoints[temp2 + 2][1] = (yB + yM) / 2.0;

		temp++;
		temp2 += 3;
	}

	// Coloco o último ponto do desenho atual na nova matriz 
	newPoints[temp2][0] = points[currNumOfPoints - 1][0];
	newPoints[temp2][1] = points[currNumOfPoints - 1][1];

	// Altero os valores dessas variáveis para manter a coerência 
	currNumOfPoints = maxNumOfPoints = 3 * currNumOfPoints - 1;

	// Guardo o endereço da antiga matriz points para poder liberar esse espaço
	aux = points;
	// Faço points apontar para a nova matriz
	points = newPoints;

	// Libero o espaço da matriz points antiga
	free(aux);
}

// Função mouseFunc
// Parâmetros: button (diz qual botão que sofreu a ação), state (diz qual foi a ação), x (coordenada x do lugar que sofreu
// a ação) e y (coordenada y do lugar que sofreu a ação)
// Objetivo: A função pode "disparar" uma espécie de evento que pode ocorrer ao realizar alguma ação com o mouse
void mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // Caso o botão esquerdo tenha sido apertado
	{
		drawPolygonal(x, y); // Uma linha pode ser desenhada (a linha será desenhada de fato caso não seja o primeiro clique)
	}
}

// Função keyboardFunc
// Parâmetros: key (qual tecla foi apertada), x e y
// Objetivo: A função pode "disparar" uma espécie de evento que pode ocorrer ao realizar alguma ação com o teclado 
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

// Função displayFunc
// Parâmetros: nenhum
// Objetivo: Mostra tudo que é criado dentro dela na janela
void displayFunc(void)
{
	srand(time(NULL));
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glLineWidth(3);

	if (drawLine == true or smoth == true) // Se quero desenhar linhas ou suavizar essas linhas
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

		smoth = false; // Indica que não quero mais suavizar (caso previamente essa variável fosse true)
	}

		
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char **argv) 
{
	// Criação e alocação da matriz points
	points = (float **) malloc(sizeof(float) * maxNumOfPoints);

	for (int i = 0; i < maxNumOfPoints; i++)
	{
		points[i] = (float *)malloc(sizeof(float) * 2);
	}

	glutInit(&argc, argv); // Inicializo o glut
		glutInitWindowSize(dimX, dimY); // Defino o tamanho da janela
	glutCreateWindow("Polygon Smoothing"); // Crio a janela cujo título é "Polygon Smoothing"
	// Associação entre dispositivos e o glut
	glutKeyboardFunc(keyboardFunc); 
	glutMouseFunc(mouseFunc);
	glutDisplayFunc(displayFunc);
	glutMainLoop(); // Mantém a main em "loop" até que a janela seja fechada ou um erro ocorra 

	free(points); // Libera o espaço criado para a matriz points
	return 0;
}