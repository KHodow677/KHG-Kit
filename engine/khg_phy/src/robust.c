#include "khg_phy/robust.h"
#include "khg_phy/vect.h"


bool
cpCheckPointGreater(const cpVect a, const cpVect b, const cpVect c)
{
	return (b.y - a.y)*(a.x + b.x - 2*c.x) > (b.x - a.x)*(a.y + b.y - 2*c.y);
}

bool
cpCheckAxis(cpVect v0, cpVect v1, cpVect p, cpVect n){
	return cpvdot(p, n) <= phy_max(cpvdot(v0, n), cpvdot(v1, n));
}
