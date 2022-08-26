#ifndef RAST
#define RAST
#include <iostream>
#include <math.h>
#include <list>
#include <map>

#include "Ponto.h"
#include "Transformations.h"

struct edge {
	float yMax;
	float x;
	float invM;

	edge(int yMaxt, int xt, float invMt) {
		yMax = yMaxt;
		x = xt;
		invM = invMt;
	}
};

inline int mod(int x, int m) {
	return (x % m + m) % m;
}

inline bool compareXEdges(edge e1, edge e2) {
	return e1.x < e2.x;
}

inline int getMinValidYIndex(ponto *pnts, int size, bool includeEmpty = false) {
	if (size <= 0) return -1;

	int minY = INT_MAX,
		minYIndex = -1;

	for (int i = 0; i < size; i++) {
		if ((!pnts[i].empty || includeEmpty) && minY > pnts[i].y) {
			minY = pnts[i].y;
			minYIndex = i;
		}
	}
	return minYIndex;
}

inline int getMaxValidYIndex(ponto* pnts, int size, bool includeEmpty = false) {
	if (size <= 0) return -1;

	int maxY = INT_MIN,
		maxYIndex = -1;

	for (int i = 0; i < size; i++) {
		if ((!pnts[i].empty || includeEmpty) && maxY < pnts[i].y) {
			maxY = pnts[i].y;
			maxYIndex = i;
		}
	}
	return maxYIndex;
}

inline void calculateVariation(int* dX, int* dY, ponto* p1, ponto* p2) {
	*dX = p2->x - p1->x;
	*dY = p2->y - p1->y;
}

inline void swapCordinates(ponto* p) {
	int aux = p->x;
	p->x = p->y;
	p->y = aux;
}

inline void swapPoints(ponto* p1, ponto* p2) {
	int aux = p1->x;
	p1->x = p2->x;
	p2->x = aux;

	aux = p1->y;
	p1->y = p2->y;
	p2->y = aux;
}

inline void printPoint(ponto * p) {
	if (p == NULL) return;
	std::cout << "( " << p->x << " , " << p->y << " )" << std::endl;
}

inline std::list<ponto> drawSimpleLine(ponto p1, ponto p2, bool invertY = false, int height = 0) {

	std::list<ponto> pontos;

	int dX, dY;

	calculateVariation(&dX, &dY, &p1, &p2);

	float d = 2 * dY - dX,
		incE = 2 * dY,
		incNE = 2 * (dY - dX);

	int currentY = p1.y;

	for (int i = p1.x + 1; i <= p2.x; i++) {
		if (d <= 0)
			d += incE;
		else {
			d += incNE;
			currentY++;
		}

		ponto* p = new ponto;
		p->setCoordinates(i, currentY);

		if (invertY)
			p->y = height - p->y;

		pontos.push_back(*p);
	}

	return pontos;
}

inline std::map<int, std::list<edge>> getEdgesTable(std::list<ponto> pnts) {
	std::map<int, std::list<edge>> edgesTable;

	int size = pnts.size();
	ponto* pntsV = new ponto[size];

	std::copy(pnts.begin(), pnts.end(), pntsV);
	while(true) {
		int i = getMinValidYIndex(pntsV, size);
		int min = mod((i - 1), size);
		if (i < 0)
			break;

		ponto p = pntsV[i];
		ponto prev = pntsV[mod((i - 1), size)];
		ponto next = pntsV[mod((i + 1), size)];

		auto f = edgesTable.find(p.y);

		if (f == edgesTable.end()) {
			std::list<edge> temp;
			edgesTable.insert({p.y, temp});
		}

		auto edgesPair = edgesTable.find(p.y);
		std::list<edge> *edges = &edgesPair->second;

		if (!prev.empty && prev.y != p.y) {
			int dX, dY;
			calculateVariation(&dX, &dY, &p, &prev);
			float invM = (float)dX / (float)dY;
			edge *e = new edge{ std::max(p.y, prev.y), p.x, invM };
			edges->push_back(*e);
		}

		if (!next.empty && next.y != p.y) {
			int dX, dY;
			calculateVariation(&dX, &dY, &p, &next);
			float invM = (float)dX / (float)dY;

			edge* e = new edge{ std::max(p.y, next.y), p.x, invM };

			edges->push_back(*e);
		}

		pntsV[i].empty = true;
	}

	return edgesTable;
}

inline std::map<int, std::list<edge>> getActiveEdgesTable(std::map<int, std::list<edge>> edges, std::list<ponto> pnts) {
	std::map<int, std::list<edge>> activeEdgesTable;

	int size = pnts.size();
	ponto* pntsV = new ponto[size];
	std::copy(pnts.begin(), pnts.end(), pntsV);

	int minY = pntsV[getMinValidYIndex(pntsV, size, true)].y,
		maxY = pntsV[getMaxValidYIndex(pntsV, size, true)].y;

	printf("min = %d\n", minY);

	std::list<edge> lastEdges;

	for (int i = minY; i <= maxY; i++) {
		std::list<edge> atualEdges;

		for (edge e : lastEdges) {

			if (e.yMax > i) {
				e.x += e.invM;
				atualEdges.push_back(e);
			}
		}

		auto f = edges.find(i);
		if (f != edges.end()) {
			std::list<edge> corresp = f->second;
			atualEdges.insert(atualEdges.end(), corresp.begin(), corresp.end());
		}

		lastEdges = atualEdges;
		activeEdgesTable.insert({ i, atualEdges });
	}

	return activeEdgesTable;
}

inline std::list<ponto> fillPolygon(std::list<ponto> pnts, bool invert = false, int height = 0) {
	std::list<ponto> pontos;
	std::map<int, std::list<edge>> edgesTable = getEdgesTable(pnts);

	std::map<int, std::list<edge>> activeEdgesTable = getActiveEdgesTable(edgesTable, pnts);

	for (auto itr = activeEdgesTable.begin(); itr != activeEdgesTable.end(); ++itr) {
		std::list<edge> atualEdges = itr->second;
		atualEdges.sort(compareXEdges);

		for (int i = 0; i < atualEdges.size(); i+=2) {
			edge e1 = atualEdges.front();
			atualEdges.pop_front();

			edge e2 = atualEdges.front();
			atualEdges.pop_front();

			for (int x = (int)e1.x; x < ceil(e2.x); x++) {
				ponto* p = new ponto;
				p->setCoordinates(x, itr->first);
				if (invert)
					p->y = height - p->y;
				pontos.push_back(*p);
			}
		}
	}
	return pontos;
}

inline std::list<ponto> drawLine(ponto p1, ponto p2,bool invertY = false, int height = 0){

	std::list<ponto> pontos;

	int size = std::max(
		abs((float)p2.x - (float)p1.x) + 1,
		abs((float)p2.y - (float)p1.y) + 1);

	int	dX = 0,
		dY = 0;

	calculateVariation(&dX, &dY, &p1, &p2);

	bool declive = false,
		 simetrico = false;

	if (dX * dY < 0) {
		p1.y *= -1;
		p2.y *= -1;
		calculateVariation(&dX, &dY, &p1, &p2);
		simetrico = true;
	}

	if (abs(dX) < abs(dY)) {
		swapCordinates(&p1);
		swapCordinates(&p2);
		calculateVariation(&dX, &dY, &p1, &p2);
		declive = true;
	}

	if (p1.x > p2.x) {
		swapPoints(&p1, &p2);
		calculateVariation(&dX, &dY, &p1, &p2);
	}
	
	float d = 2 * dY - dX,
		incE = 2 * dY,
		incNE = 2 * (dY - dX);

	int currentY = p1.y;


	for (int i = 0; i < size; i++) {
		if (i != 0) {
			if (d <= 0)
				d += incE;
			else {
				d += incNE;
				currentY++;
			}
		}

		ponto* p = new ponto;
		p->x = i + p1.x;
		p->y = currentY;

		if (declive)
			swapCordinates(p);

		if (simetrico)
			p->y *= -1;

		if(invertY)
			p->y = height - p->y;

		pontos.push_back(*p);
	}

	return pontos;

}

inline std::list<ponto> getRetanglePoints(ponto p1, ponto p2, bool invertY = false, int height = 0) {
	std::list<ponto> pontos;

	int hX = std::max(p1.x, p2.x),
		hY = std::max(p1.y, p2.y),
		lX = std::min(p1.x, p2.x),
		lY = std::min(p1.y, p2.y);

	ponto* v1 = new ponto,
		* v2 = new ponto,
		* v3 = new ponto,
		* v4 = new ponto;

	v1->setCoordinates(lX, lY);
	v2->setCoordinates(hX, lY);
	v3->setCoordinates(hX, hY);
	v4->setCoordinates(lX, hY);

	pontos.push_back(*v1);
	pontos.push_back(*v2);
	pontos.push_back(*v3);
	pontos.push_back(*v4);

	return pontos;
}

inline std::list<ponto> drawRectangle(ponto p1, ponto p2, bool invertY = false, int height = 0) {
	std::list<ponto> pontos;

	int hX = std::max(p1.x, p2.x),
		hY = std::max(p1.y, p2.y),
		lX = std::min(p1.x, p2.x),
		lY = std::min(p1.y, p2.y);

	//vertices
	ponto* v1 = new ponto,
		* v2 = new ponto,
		* v3 = new ponto,
		* v4 = new ponto;

	v1->setCoordinates(lX, lY);
	v2->setCoordinates(hX, lY);
	v3->setCoordinates(hX, hY);
	v4->setCoordinates(lX, hY);

	int s = 0;

	//lines
	std::list<ponto> l1 = drawLine(*v1, *v2, invertY, height),
		l2 = drawLine(*v2, *v3, invertY, height),
		l3 = drawLine(*v3, *v4, invertY, height),
		l4 = drawLine(*v4, *v1, invertY, height);

	pontos.insert(pontos.end(), l1.begin(), l1.end());
	pontos.insert(pontos.end(), l2.begin(), l2.end());
	pontos.insert(pontos.end(), l3.begin(), l3.end());
	pontos.insert(pontos.end(), l4.begin(), l4.end());

	return pontos;
}

inline std::list<ponto> drawTriangle(ponto p1, ponto p2, ponto p3, bool invertY = false, int height = 0) {
	std::list<ponto> pontos;
	
	std::list<ponto> l1 = drawLine(p1, p2, invertY, height),
		l2 = drawLine(p2, p3, invertY, height),
		l3 = drawLine(p3, p1, invertY, height);

	pontos.insert(pontos.end(), l1.begin(), l1.end());
	pontos.insert(pontos.end(), l2.begin(), l2.end());
	pontos.insert(pontos.end(), l3.begin(), l3.end());

	return pontos;
}

inline std::list<ponto> drawPolygon(std::list<ponto> pts, bool invertY, int height = 0) {
	std::list<ponto> pontos;
	
	ponto last = pts.back();

	ponto lastAcessed = last;
	pts.pop_back();

	while (true) {
		if (pts.empty())
			break;

		ponto p = pts.back();
		pts.pop_back();
		std::list<ponto> l = drawLine(p, lastAcessed, invertY, height);
		pontos.insert(pontos.end(), l.begin(), l.end());
		lastAcessed = p;
	}
	std::list<ponto> l = drawLine(last, lastAcessed, invertY, height);
	pontos.insert(pontos.end(), l.begin(), l.end());

	return pontos;
}

inline std::list<ponto> drawCircuference(int r, bool invertY = false, int height = 0, ponto* center = NULL) {
	std::list<ponto> pontos;

	if (center == NULL) {
		center = new ponto;
		center->setCoordinates(0, 0);
	}

	std::cout << "centro da circuferencia: " << std::endl;
	printPoint(center);

	float d = 1 - r,
		dE = 3,
		dSE = (2 * (-r)) + 5,
		currentX = 0,
		currentY = r;

	int i = 0;

	while (currentY > currentX) {
		if (currentX != 0) {
			if (d < 0) {
				d += dE;
				dSE += 2;
			}
			else {
				d += dSE;
				dSE += 4;
				currentY--;
			}

			dE += 2;
		}

		ponto* pnt = new ponto;
		pnt->setCoordinates(currentX++, currentY);
		pontos.push_back(*pnt);
	}
	if (invertY)
		center->y = height - center->y;

	pontos = octanteMirroring(pontos);
	pontos = translate(pontos, center->x, center->y);
	
	return pontos;
}

#endif