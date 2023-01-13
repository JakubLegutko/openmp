#ifndef MINIMUM_H
#define MINIMUM_H

#include "Function.h"
#include "Minimization.h"

class SimpleMinimization: public Minimization {
private:
	void generateRandomPosition();
public:
	struct drand48_data rand;
	void find(double dr_ini, double dr_fin, int idleStepsLimit);
	SimpleMinimization(Function *f, double timeLimit);
	~SimpleMinimization();
};

#endif

