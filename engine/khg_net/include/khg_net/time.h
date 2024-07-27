#pragma once

#define net_time_overflow 86400000

#define net_time_less(a, b) ((a) - (b) >= net_time_overflow)
#define net_time_greater(a, b) ((b) - (a) >= net_time_overflow)
#define net_time_less_equal(a, b) (! net_time_greater(a, b))
#define net_time_greater_equal(a, b) (! net_time_less(a, b))

#define net_time_difference(a, b) ((a) - (b) >= net_time_overflow ? (b) - (a) : (a) - (b))
