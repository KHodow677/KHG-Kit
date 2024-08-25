#include "khg_phy/robust.h"
#include "khg_phy/vect.h"


bool
phy_check_point_greater(const phy_vect a, const phy_vect b, const phy_vect c)
{
	return (b.y - a.y)*(a.x + b.x - 2*c.x) > (b.x - a.x)*(a.y + b.y - 2*c.y);
}

bool
phy_check_axis(phy_vect v0, phy_vect v1, phy_vect p, phy_vect n){
	return phy_v_dot(p, n) <= phy_max(phy_v_dot(v0, n), phy_v_dot(v1, n));
}
