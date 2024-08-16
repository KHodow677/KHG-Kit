#include "khg_phy/robust.h"
#include "khg_phy/vect.h"


bool
cpCheckPointGreater(const phy_vect a, const phy_vect b, const phy_vect c)
{
	return (b.y - a.y)*(a.x + b.x - 2*c.x) > (b.x - a.x)*(a.y + b.y - 2*c.y);
}

bool
cpCheckAxis(phy_vect v0, phy_vect v1, phy_vect p, phy_vect n){
	return cpvdot(p, n) <= phy_max(cpvdot(v0, n), cpvdot(v1, n));
}
