#ifndef PONTO
#define	PONTO

#include <list>

// Estrututa de dados para o armazenamento dinamico dos pontos
// selecionados pelos algoritmos de rasterizacao
struct ponto {
	int x, y;
	bool empty = false;
	ponto* prox;

	void setCoordinates(int cX, int cY) {
		x = cX;
		y = cY;
	}
};
#endif