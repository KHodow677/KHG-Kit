#include "khg_utl/easing.h"
#include <math.h>

float utl_easing_linear_interpolation(float p) {
	return p;
}

float utl_easing_quadratic_ease_in(float p) {
	return p * p;
}

float utl_easing_quadratic_ease_out(float p) {
	return -(p * (p - 2));
}

float utl_easing_quadratic_ease_in_out(float p) {
	if(p < 0.5) {
		return 2 * p * p;
	}
	else {
		return (-2 * p * p) + (4 * p) - 1;
	}
}

float utl_easing_cubic_ease_in(float p) {
	return p * p * p;
}

float utl_easing_cubic_ease_out(float p) {
	float f = (p - 1);
	return f * f * f + 1;
}

float utl_easing_cubic_ease_in_out(float p) {
	if(p < 0.5) {
		return 4 * p * p * p;
	}
	else {
		float f = ((2 * p) - 2);
		return 0.5 * f * f * f + 1;
	}
}

float utl_easing_quartic_ease_in(float p) {
	return p * p * p * p;
}

float utl_easing_quartic_ease_out(float p) {
	float f = (p - 1);
	return f * f * f * (1 - p) + 1;
}

float utl_easing_quartic_ease_in_out(float p) {
	if(p < 0.5) {
		return 8 * p * p * p * p;
	}
	else {
		float f = (p - 1);
		return -8 * f * f * f * f + 1;
	}
}

float utl_easing_quintic_ease_in(float p) {
	return p * p * p * p * p;
}

float utl_easing_quintic_ease_out(float p) {
	float f = (p - 1);
	return f * f * f * f * f + 1;
}

float utl_easing_quintic_ease_in_out(float p) {
	if(p < 0.5) {
		return 16 * p * p * p * p * p;
	}
	else {
		float f = ((2 * p) - 2);
		return  0.5 * f * f * f * f * f + 1;
	}
}

float utl_easing_sine_ease_in(float p) {
	return sin((p - 1) * M_PI_2) + 1;
}

float utl_easing_sine_ease_out(float p) {
	return sin(p * M_PI_2);
}

float utl_easing_sine_ease_in_out(float p) {
	return 0.5 * (1 - cos(p * M_PI));
}

float utl_easing_circular_ease_in(float p) {
	return 1 - sqrt(1 - (p * p));
}

float utl_easing_circular_ease_out(float p) {
	return sqrt((2 - p) * p);
}

float utl_easing_circular_ease_in_out(float p) {
	if(p < 0.5) {
		return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
	}
	else {
		return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
	}
}

float utl_easing_exponential_ease_in(float p) {
	return (p == 0.0) ? p : pow(2, 10 * (p - 1));
}

float utl_easing_exponential_ease_out(float p) {
	return (p == 1.0) ? p : 1 - pow(2, -10 * p);
}

float utl_easing_exponential_ease_in_out(float p) {
	if(p == 0.0 || p == 1.0) {
    return p;
  }
	if(p < 0.5) {
		return 0.5 * pow(2, (20 * p) - 10);
	}
	else {
		return -0.5 * pow(2, (-20 * p) + 10) + 1;
	}
}

float utl_easing_elastic_ease_in(float p) {
	return sin(13 * M_PI_2 * p) * pow(2, 10 * (p - 1));
}

float utl_easing_elastic_ease_out(float p) {
	return sin(-13 * M_PI_2 * (p + 1)) * pow(2, -10 * p) + 1;
}

float utl_easing_elastic_ease_in_out(float p) {
	if(p < 0.5) {
		return 0.5 * sin(13 * M_PI_2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
	}
	else {
		return 0.5 * (sin(-13 * M_PI_2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
	}
}

float utl_easing_back_ease_in(float p) {
	return p * p * p - p * sin(p * M_PI);
}

float utl_easing_back_ease_out(float p) {
	float f = (1 - p);
	return 1 - (f * f * f - f * sin(f * M_PI));
}

float utl_easing_back_ease_in_out(float p) {
	if(p < 0.5) {
		float f = 2 * p;
		return 0.5 * (f * f * f - f * sin(f * M_PI));
	}
	else {
		float f = (1 - (2*p - 1));
		return 0.5 * (1 - (f * f * f - f * sin(f * M_PI))) + 0.5;
	}
}

float utl_easing_bounce_ease_in(float p) {
	return 1 - utl_easing_bounce_ease_out(1 - p);
}

float utl_easing_bounce_ease_out(float p) {
	if(p < 4/11.0) {
		return (121 * p * p)/16.0;
	}
	else if(p < 8/11.0) {
		return (363/40.0 * p * p) - (99/10.0 * p) + 17/5.0;
	}
	else if(p < 9/10.0) {
		return (4356/361.0 * p * p) - (35442/1805.0 * p) + 16061/1805.0;
	}
	else {
		return (54/5.0 * p * p) - (513/25.0 * p) + 268/25.0;
	}
}

float utl_easing_bounce_ease_in_out(float p) {
	if(p < 0.5) {
		return 0.5 * utl_easing_bounce_ease_in(p*2);
	}
	else {
		return 0.5 * utl_easing_bounce_ease_out(p * 2 - 1) + 0.5;
	}
}
