#ifndef TRANSF
#define TRANSF

#define _USE_MATH_DEFINES

#include "Ponto.h"
#include <math.h>
#include<list>

inline double deg2rad(double deg)
{
	return deg * M_PI / 180.;
}

inline ponto* calculateCenter(std::list<ponto> pnts) {

	float	x = 0,
			y = 0;

	for (ponto p : pnts) {
		x += p.x;
		y += p.y;
	}

	float size = pnts.size();
	y /= size;
	x /= size;

	ponto* p = new ponto;
	p->setCoordinates((int)x, (int)y);

	return p;
}

inline double calculateDistance(ponto* p1, ponto* p2) {
	double	dX = p2->x - p1->x,
			dY = p2->y - p1->y;

	dX *= dX;
	dY *= dY;

	return sqrt(dX + dY);
}

inline ponto calculateMidPoint(ponto* p1, ponto* p2) {
	double x = (p2->x + p1->x)/2,
		y = (p2->y + p1->y)/2;

	ponto* p = new ponto;
	p->setCoordinates((int)x, (int)y);
	return *p;
}

inline std::list<ponto> translate(std::list<ponto>& pnts, int tX, int tY, bool invertY = false) {

	if (invertY) tY *= -1;

	std::list<ponto> pontos;

	for (ponto p : pnts) {
		ponto* pnt = new ponto;
		pnt->setCoordinates(p.x + tX, p.y + tY);
		pontos.push_back(*pnt);
	}

	pnts.clear();

	return pontos;
}

inline std::list<ponto> octanteMirroring(std::list<ponto>& pnts, ponto* center = NULL) {
	std::list<ponto> pontos;

	if (center == NULL) {
		center = new ponto;
		center->setCoordinates(0, 0);
	}

	for (ponto p : pnts) {
		
		ponto *pnt = new ponto;
		pnt->setCoordinates(p.x - center->x, p.y - center->y);

		for (int i = 0; i < 2; i++) {
			ponto* p2 = new ponto,
				* p3 = new ponto,
				* p4 = new ponto;

			p2->setCoordinates(-pnt->x, pnt->y);
			p3->setCoordinates(pnt->x, -pnt->y);
			p4->setCoordinates(-pnt->x, -pnt->y);

			pontos.push_back(*pnt);
			pontos.push_back(*p2);
			pontos.push_back(*p3);
			pontos.push_back(*p4);

			pnt = new ponto;
			pnt->setCoordinates(p.y - center->y, p.x - center->x);
		}

	}

	pontos = translate(pontos, center->x, center->y);
	return pontos;
}

inline std::list<ponto> scale(std::list<ponto> &pnts, float sX, float sY, ponto* center = NULL) {

	std::list<ponto> pontos;

	if (center == NULL) {
		center = new ponto;
		center->setCoordinates(0, 0);
	}
		

	for (ponto p : pnts) {
		float x = (p.x - center->x) * (float)sX;
		x += center->x;
		float y = (p.y - center->y) * (float)sY;
		y += center->y;
		
		ponto* pnt = new ponto;
		pnt->setCoordinates((int)x, (int)y);
		pontos.push_back(*pnt);
	}

	pnts.clear();
	return pontos;
}

inline std::list<ponto> shearing(std::list<ponto> &pnts, float cX, float cY, ponto *center = NULL) {

	std::list<ponto> pontos;

	if (center == NULL) {
		center = new ponto;
		center->setCoordinates(0, 0);
	}

	for (ponto p : pnts) {
		int pX = p.x - center->x,
			pY = p.y - center->y;

		float x =  pX + (cY * (float)pY);
		x += center->x;

		float y = pY + ( cX * (float)pX);
		y += center->y;

		ponto* pnt = new ponto;
		pnt->setCoordinates((int)x, (int)y);
		pontos.push_back(*pnt);
	}

	pnts.clear();
	return pontos;
}

inline std::list<ponto> reflection(std::list<ponto> &pnts, bool applyOnX = true, bool applyOnY = false, ponto* center = NULL) {
	std::list<ponto> pontos;

	if (center == NULL) {
		center = new ponto;
		center->setCoordinates(0, 0);
	}

	for (ponto p : pnts) {
		int pX = p.x - center->x,
			pY = p.y - center->y;

		float x = ((applyOnX)? -pX : pX) + center->x;
		float y = ((applyOnY)? -pY : pY) + center->y;

		ponto* pnt = new ponto;
		pnt->setCoordinates((int)x, (int)y);
		pontos.push_back(*pnt);
	}

	pnts.clear();
	return pontos;
}

inline std::list<ponto> rotate(std::list<ponto>& pnts, float angle, ponto *center = NULL) {
	std::list<ponto> pontos;

	double rad = deg2rad(angle);

	if (center == NULL) {
		center = new ponto;
		center->setCoordinates(0, 0);
	}

	for (ponto p : pnts) {
		int pX = p.x - center->x,
			pY = p.y - center->y;

		double x = ((double)pX * cos(rad)) - ((double)pY * sin(rad));
		double y = ((double)pX * sin(rad)) + ((double)pY * cos(rad));
		x += center->x;
		y += center->y;

		ponto* pnt = new ponto;
		pnt->setCoordinates((int)x, (int)y);
		pontos.push_back(*pnt);
	}

	pnts.clear();
	return pontos;
}

#endif