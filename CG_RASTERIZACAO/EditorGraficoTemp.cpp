#include "Ponto.h"
#include "Rasterizacao.h"

int mainTemp(int argc, char** argv) {
	ponto * p1 = new ponto();
	ponto * p2 = new ponto();
	 
	/*p1->x = -4;
	p1->y = 1; 
	p2->x = -1;
	p2->y = 5;*/
	
	p1->x = 4;
	p1->y = 1;
	p2->x = 1;
	p2->y = 5;

	//int size;
	//ponto* pontos = drawLine(p1, p2, &size);
	//std::cout << "RESULT:";
	//for (int i = 0; i < size; i++) {
		//printPoint(pontos[i]);
	//}

	return 0;
}