//*********************************************************************
//  Codigo exemplo para aula pratica de Computacao Grafica 
//  Assunto: Rasterizacao e Transformacoes Geometricas
//
//  Autor: Prof. Laurindo de Sousa Britto Neto
//*********************************************************************

// Bibliotecas utilizadas pelo OpenGL
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

// Arquivos do projeto
#include "Ponto.h"
#include "Rasterizacao.h"
#include "Transformations.h"

// Biblioteca com funcoes matematicas
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <list>

// Variaveis Globais
//1 - linha (l); 2 - quadrilatero(q); 3 - triangulo(t); 4 - poligono(p); 5 - translation(T)
int drawMode = 1;
bool enablePolygonFilling = false;
bool lastPolygonFilling = enablePolygonFilling;
int lastDrawMode = drawMode;
int clicks = 0;

int width = 512, height = 512; //Largura e altura da janela

struct colorPositionMap {int position;float* color;};

// Lista encadeada de pontos
// indica o primeiro elemento da lista
ponto* pontos = NULL;
std::list<ponto> pontosListT;
std::list<ponto> drawPontosList;

std::list<ponto> lastForm;


//coloração das formas
float* lastColor;
std::list<int> colorsPosition;
std::list<float[3]> colorsAtPositions;

std::list<colorPositionMap> colorsMap;

int translationFactor = 3;
float scaleFactor = 1.5;
float shearingFactor = 0.1;
float rotationFactor = 2.0;

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void SpecialInput(int key, int x, int y);
void mouse(int button, int state, int x, int y);

// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos(std::list<ponto>);


// Funcao para armazenar um ponto na lista
// Armazena como uma Pilha (empilha)
ponto* pushPonto(ponto* p, std::list<ponto>& pontosList) {
    pontosList.push_back(*p);
    return p;
} 

// Funcao para desarmazenar um ponto na lista
// Desarmazena como uma Pilha (desempilha)
ponto popPonto(std::list<ponto> &pontosList) {
    if (pontosList.empty()) return *new ponto;

    ponto pnt = pontosList.back();
    pontosList.pop_back();

    return pnt;
}

void sequentialPopPonto(std::list<ponto>& pontoList, int init) {
    int quantity = pontoList.size() - 1 - init; //quantity of Pops required

    for (int i = 0; i < quantity; i++) {
        popPonto(pontoList);
    }
}

void changeLastColor(float r, float g, float b) {
    lastColor = new float[3];
    lastColor[0] = r;
    lastColor[1] = g;
    lastColor[2] = b;
}

void definePointsColor() {
    if (lastColor == NULL)
        changeLastColor(0, 0, 0);

    colorPositionMap* cpm = new colorPositionMap;
    cpm->position = drawPontosList.size() - 1;
    cpm->color = lastColor;
    colorsMap.push_back(*cpm);
}

void setLastForm(bool clearOriginal) {
    lastForm = pontosListT;

    if (clearOriginal) {
        std::list<ponto> temp;
        pontosListT = temp;
    }
}

void insertForm(std::list<ponto> listPonto, bool updateLastDrawMode = true, bool updateColorPosition = true, bool resetClicks = true) {
    
    if (updateLastDrawMode) {
        lastDrawMode = drawMode;
        lastPolygonFilling = enablePolygonFilling;
    }
    if(updateColorPosition)
        definePointsColor();
    drawPontosList.insert(drawPontosList.end(), listPonto.begin(), listPonto.end());

    if(resetClicks)
        clicks = 0;
    pontosListT.clear();
    glutPostRedisplay();
}
// Funcao Principal do C
int main(int argc, char** argv) {

    definePointsColor();
     
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
    glutInitWindowSize(width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition(100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow("Rasterizacao"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutSpecialFunc(SpecialInput);
    glutMouseFunc(mouse); //funcao callback do mouse
    glutDisplayFunc(display); //funcao callback de desenho
    glutMainLoop(); // executa o loop do OpenGL
    return 0; // retorna 0 para o tipo inteiro da funcao main();
}

// Funcao com alguns comandos para a inicializacao do OpenGL;
void init(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

void reshape(int w, int h)
{
    // Reinicializa o sistema de coordenadas
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Definindo o Viewport para o tamanho da janela
    glViewport(0, 0, w, h);

    width = w;
    height = h;
    glOrtho(0, w, 0, h, -1, 1);

    // muda para o modo GL_MODELVIEW (não pretendemos alterar a projecção
    // quando estivermos a desenhar na tela)
    glMatrixMode(GL_MODELVIEW); 
}

float** getColorsMatrix() {
    float** colorsMatrix = new float*[(width * height)]; 
    float* clr = new float[3]{ 1, 1, 1 }; 

    for (int i = 0; i < width * height; i++) {
        colorsMatrix[i] = new float[3];
    }

    int size = colorsMap.size();
    colorPositionMap* arr = new colorPositionMap[size];
    std::copy(colorsMap.begin(), colorsMap.end(), arr);

    int i = 0, j = 0;

    for (auto p : drawPontosList) {
        if (j < size) {
            if (arr[j].position <= i) {
                clr = arr[j].color;
                j++;
            }
        }

        if (((width * p.y) + p.x < width * height) && p.x < width && p.y < height && p.x >= 0 && p.y >= 0) {
            colorsMatrix[(width * p.y) + p.x] = clr;  
        }
        i++;
    }

    return colorsMatrix;
}

void fillLastForm() {
    if (lastForm.size() > 2 && lastPolygonFilling) {
        std::list<ponto> temp = fillPolygon(lastForm, true, height);
        insertForm(temp, false, false, false);
    }
}

void reRasterizeForm(int drawListIndex, std::list<ponto> &listPonto ) {

    if (listPonto.empty()) return;

    ponto* pontos = new ponto[listPonto.size()];
    std::copy(listPonto.begin(), listPonto.end(), pontos);

    std::list<ponto> temp;
    float r = 0;
    ponto center;  
    switch (lastDrawMode)
    {
    case 10:////circuference
        center = calculateMidPoint(&pontos[0], &pontos[1]);

        r = abs(calculateDistance(&pontos[0], &pontos[1])) / 2;

        temp = drawCircuference(r, true, height, &center);
        break;

    default:
        temp = drawPolygon(lastForm, true, height);
        break;
    }
    sequentialPopPonto(drawPontosList, drawListIndex);
    insertForm(temp, false, false, false);
    fillLastForm();
}

void resetPontos() {
    clicks = 0;
    pontosListT.clear();
    delete pontos;
    pontos = NULL;
}
//Utilizada para mapear setas do teclado, para aplicar transformações
void SpecialInput(int key, int x, int y)
{
    ponto* center = calculateCenter(lastForm);
    if (lastDrawMode == 11)
        return;

    switch (key) 
    {
    case GLUT_KEY_UP:
        switch (drawMode)
        { 
        case 5:
            lastForm = translate(lastForm, 0, translationFactor, true);
            break;
        case 6:
            lastForm = scale(lastForm, scaleFactor, scaleFactor, center);
            break;
        case 7:
            lastForm = shearing(lastForm, 0, shearingFactor, center);
            break;
        case 8:
            lastForm = reflection(lastForm, false, true, center);
            break;
        default:
            return;
        }
        break;


    case GLUT_KEY_DOWN:
        switch (drawMode)
        {
        case 5:
            lastForm = translate(lastForm, 0, -translationFactor, true);
            break;
        case 6:
            lastForm = scale(lastForm, 1/scaleFactor, 1/scaleFactor, center);
            break;
        case 7:
            lastForm = shearing(lastForm, 0, -shearingFactor, center);
            break;
        case 8:
            lastForm = reflection(lastForm, false, true, center);
            break;
        default:
            return;
        }
        break;


    case GLUT_KEY_LEFT:
        switch (drawMode)
        {
        case 5:
            lastForm = translate(lastForm, -translationFactor, 0);
            break;
        case 6:
            lastForm = scale(lastForm, 1/scaleFactor, 1, center);
            break;
        case 7:
            lastForm = shearing(lastForm, -shearingFactor, 0, center);
            break;
        case 8:
            lastForm = reflection(lastForm, true, false, center);
            break;
        case 9:
            lastForm = rotate(lastForm, -rotationFactor, center);
            break;
        default:
            return;
        }
        break;


    case GLUT_KEY_RIGHT:
        switch (drawMode)
        {
        case 5:
            lastForm = translate(lastForm, translationFactor, 0);
            break;
        case 6:
            lastForm = scale(lastForm, scaleFactor, 1, center);
            break;
        case 7:
            lastForm = shearing(lastForm, shearingFactor, 0, center);
            break;
        case 8:
            lastForm = reflection(lastForm, true, false, center);
            break;
        case 9:
            lastForm = rotate(lastForm, rotationFactor, center);
            break;
        default:
            return;
        }
        break;
    default:
        return;
    }
    colorPositionMap cpm = colorsMap.back();
    int initIndex = cpm.position;
    reRasterizeForm(initIndex, lastForm);
    glutPostRedisplay();
}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y) {

    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
    case 27: // codigo ASCII da tecla ESC
        exit(0); // comando pra finalizacao do programa
        break;
    case 'l':
        drawMode = 1;
        std::cout << "changed to Line mode." << std::endl;
        resetPontos();
        break;
    case 'q':
        drawMode = 2;
        std::cout << "changed to Rectangle mode." << std::endl;
        resetPontos();
        break;
    case 't':
        drawMode = 3;
        std::cout << "changed to Triangle mode." << std::endl;
        resetPontos();
        break;
    case 'p':
        drawMode = 4;
        std::cout << "changed to Polygon mode." << std::endl;
        resetPontos();
        break;
    case 13:
        if (drawMode == 4 && !pontosListT.empty()) {
            std::cout << "drawing Polygon..." << std::endl;
            setLastForm(false);
            std::list<ponto> temp = drawPolygon(pontosListT, true, height);
            insertForm(temp);
            fillLastForm();
        }
        break;

    case 'c':
        drawMode = 10;
        std::cout << "changed to Circuference mode." << std::endl;
        resetPontos();
        break;

    case 'f':
        drawMode = 11;
        std::cout << "changed to Floyd Fill mode." << std::endl;
        resetPontos();
        break;

    case 'T':
        drawMode = 5;
        std::cout << "changed to Translation mode." << std::endl;
        resetPontos();
        break;
    case 'E':
        drawMode = 6;
        std::cout << "changed to Scale mode." << std::endl;
        resetPontos();
        break;

    case 'C':
        drawMode = 7;
        std::cout << "changed to Shear mode." << std::endl;
        resetPontos();
        break;
    case 'I':
        drawMode = 8;
        std::cout << "changed to Reflection mode." << std::endl;
        resetPontos();
        break;
    case 'R':
        drawMode = 9;
        std::cout << "changed to Rotation mode." << std::endl;
        resetPontos();
        break;
    
    //Alteração das cores
    case '1':
        changeLastColor(0, 0, 0);
        std::cout << "cor setada como Preto" << std::endl;
        break;
    case '2':
        changeLastColor(1, 0, 0);
        std::cout << "cor setada como Vermelho" << std::endl;
        break;
    case '3':
        changeLastColor(0, 1, 0);
        std::cout << "cor setada como Verde" << std::endl;
        break;
    case '4':
        changeLastColor(0, 0, 1);
        std::cout << "cor setada como Azul" << std::endl;
        break;
    case '5':
        changeLastColor(1, 1, 1);
        std::cout << "cor setada como Branco" << std::endl;
        break;
    case '6':
        changeLastColor(1, 0, 1);
        std::cout << "cor setada como Roxo" << std::endl;
        break;
    case '7':
        changeLastColor(1, 1, 0);
        std::cout << "cor setada como Amarelo" << std::endl;
        break; 
    case '8':
        changeLastColor(0, 1, 1);
        std::cout << "cor setada como Ciano" << std::endl;
        break;
    case 'F':
        enablePolygonFilling = !enablePolygonFilling;
        break;
    }
}

//Funcao usada na funcao callback para a utilizacao do mouse
void mouse(int button, int state, int x, int y)
{
    bool updateFilling;
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            clicks++;
            float** screen;
            ponto* p = new ponto; 
            p->x = x;
            p->y = y;
            pushPonto(p, pontosListT);

            std::cout << "Click at (" << x << "," << y << ")" << std::endl;
            std::list<ponto> temp;

            switch (drawMode)
            {
            case 1:
                if (clicks >= 2) {
                    std::cout << "drawing line..." << std::endl;
                    setLastForm(false);
                    temp = drawLine(popPonto(pontosListT), popPonto(pontosListT), true, height);
                    insertForm(temp);
                    fillLastForm();
                }
                break;
            case 2:
                if (clicks >= 2) {
                    std::cout << "drawing Rectangle..." << std::endl;
                    setLastForm(false);
                    temp = drawRectangle(popPonto(pontosListT), popPonto(pontosListT), true, height);
                    insertForm(temp);
                    lastForm = getRetanglePoints(popPonto(lastForm), popPonto(lastForm), true, height);
                    fillLastForm();
                }
                break;
            case 3:
                if (clicks >= 3) {
                    std::cout << "drawing Triangle..." << std::endl;
                    setLastForm(false);
                    temp = drawTriangle(popPonto(pontosListT), popPonto(pontosListT), popPonto(pontosListT), true, height);
                    insertForm(temp);
                    fillLastForm();
                }
                break;
            case 10:
                if (clicks >= 2) {
                    std::cout << "drawing Circuference..." << std::endl;
                    setLastForm(false);
                    ponto p1 =  popPonto(pontosListT);
                    ponto p2 =  popPonto(pontosListT);

                    ponto center = calculateMidPoint(&p1, &p2);

                    float r = abs(calculateDistance(&p1, &p2)) / 2;
                     
                    temp = drawCircuference(r, true, height, &center);
                    insertForm(temp);
                    fillLastForm();
                }
                break; 

            case 11: 
                p->setCoordinates(x, height - p->y);  
                printf("getting screen\n");
                screen = getColorsMatrix();
                printf("getting filling\n");
                temp = floydFill(screen, p, height, width);
                insertForm(temp);
                break;
            default:
                break; 
            }
        }
        break;
        
    default:
        break;
    }

}

// Funcao usada na funcao callback para desenhar na tela
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
    glLoadIdentity();

    glColor3f(0.0, 0.0, 0.0); // Seleciona a cor default como preto

    drawPontos(drawPontosList);

    glutSwapBuffers(); // manda o OpenGl renderizar as primitivas
}

//Funcao que desenha os pontos contidos em uma lista de pontos
void drawPontos(std::list<ponto> pontos) {
    if (pontos.empty()) return;
    
    int size = colorsMap.size();
    colorPositionMap* arr = new colorPositionMap[size];
    std::copy(colorsMap.begin(), colorsMap.end(), arr);
    glBegin(GL_POINTS); //Seleciona a primitiva GL_POINTS para desenhar



    int i = 0;
    int j = 0;
    for (auto p : pontos) {
        if (j < size) {
            if (arr[j].position <= i) {
                float* clr = arr[j].color;
                glColor3f(clr[0], clr[1], clr[2]);
                j++;
                glEnd();
                glBegin(GL_POINTS);
            }
        }

        glVertex2i(p.x, p.y);
        i++;
    }
    
    glEnd();  // indica o fim do desenho
}