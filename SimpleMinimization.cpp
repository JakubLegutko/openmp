#include "SimpleMinimization.h"

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sys/time.h>
#include <omp.h>

const double DR_SHRINK = 0.8;

using namespace std;


SimpleMinimization::SimpleMinimization(Function *f, double timeLimit) :
		Minimization(f, timeLimit) {
		#pragma omp parallel
		{

	generateRandomPosition();
	bestX = x;
	bestY = y;
	bestZ = z;

	bestV = function->value(bestX, bestY, bestZ);

	unsigned long seed = (unsigned long) time(NULL);
	srand48_r(2137*omp_get_thread_num(), &rand);
		}
}
SimpleMinimization::~SimpleMinimization() {
}


void SimpleMinimization::find(double dr_ini, double dr_fin,
		int idleStepsLimit ) {
		#pragma omp parallel
		{
	double v, xnew, ynew, znew, vnew, dr, x_temp, y_temp, z_temp, v_temp;

	int idleSteps = 0;  // liczba krokow, ktore nie poprawily lokalizacji

	std::cout << "Start " << std::endl;

	while (hasTimeToContinue()) {
		// inicjujemy losowo polozenie startowe w obrebie kwadratu o bokach od min do max


		v = function->value(x, y, z); // wartosc funkcji w punkcie startowym

		idleSteps = 0;
		dr = dr_ini;
		double xr,yr,zr;
				double randomx;
				double randomy;
				double randomz;
				drand48_r(&rand, &randomx);
				drand48_r(&rand, &randomy);
				drand48_r(&rand, &randomz);
				x = randomx * (maxX - minX) + minX;
				y = randomy * (maxY - minY) + minY;
				z = randomz * (maxZ - minZ) + minZ;
				v = function->value(x, y, z); // wartosc funkcji w punkcie startowym
				v_temp = v;
				x_temp = x;
				y_temp = y;
				z_temp = z;

		while ( ( dr > dr_fin ) && ( idleSteps < idleStepsLimit ) ) {
			drand48_r(&rand,&xr);
			drand48_r(&rand,&yr);
			drand48_r(&rand,&zr);
			xnew = x + ( xr - 0.5) * dr;
			ynew = y + ( yr - 0.5) * dr;
			znew = z + ( zr - 0.5) * dr;

			// upewniamy sie, ze nie opuscilismy przestrzeni poszukiwania rozwiazania
			xnew = limitX(xnew);
			ynew = limitY(ynew);
			znew = limitZ(znew);

			// wartosc funkcji w nowym polozeniu
			vnew = function->value(xnew, ynew, znew);

			if (vnew < v) {
				x = xnew;  // przenosimy sie do nowej, lepszej lokalizacji
				y = ynew;
				z = znew;
				v = vnew;
				idleSteps = 0; // resetujemy licznik krokow, bez poprawy polozenia
			} else {
				idleSteps++; // nic sie nie stalo

				if ( idleSteps == idleStepsLimit ) {
					dr *= DR_SHRINK; // zmniejszamy dr
					idleSteps = 0;
				}
			}
		} // dr wciaz za duze

		#pragma omp critical
		{
			addToHistory();
		if (v < bestV) {  // znalezlismy najlepsze polozenie globalnie
			bestV = v;
			bestX = x;
			bestY = y;
			bestZ = z;

			std::cout << "New better position: " << x << ", " << y << ", " << z
					<< " value = " << v << std::endl;
		}
		}
	} // mamy czas na obliczenia
		}
}

void SimpleMinimization::generateRandomPosition() {
	x = drand48() * (maxX - minX) + minX;
	y = drand48() * (maxY - minY) + minY;
	z = drand48() * (maxZ - minZ) + minZ;
}
