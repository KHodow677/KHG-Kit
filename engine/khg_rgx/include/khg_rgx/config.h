typedef enum {
	MTC,
	MCI,
	CHC,
	MOV,
	EOM,
	SET,
	REC,
	STP,
	BRF,
	BRI,
	BOL,
	EOL,
	BOW,
	EOW,
	BND,
	MEV	
} opcode;

#define WRX_MATCH			1
#define WRX_NOMATCH			0

#define WRX_SUCCESS			0
#define WRX_MEMORY -1
#define WRX_VALUE	-2
#define WRX_BRACKET	-3
#define WRX_INVALID	-4
#define WRX_ANGLEB -5
#define WRX_SET	-6
#define WRX_RNG_ORDER	-7
#define WRX_RNG_BADCHAR	-8
#define WRX_RNG_MISMATCH -9
#define WRX_ESCAPE -10
#define WRX_BAD_DOLLAR -11
#define WRX_CURLYB -12
#define WRX_BAD_CURLYB -13
#define WRX_BAD_NFA	-14
#define WRX_SMALL_NSM	-15
#define WRX_INV_BREF -16
#define WRX_MANY_STATES	-17
#define WRX_STACK	-18
#define WRX_OPCODE -19

#define START_OF_PRINT 0x20

#define ESC		'\\'

#define BV_SET(bv, c) bv[c>>3] |= 1 << (c & 0x07)
#define BV_CLR(bv, c) bv[c>>3] &= ~(1 << (c & 0x07))
#define BV_TGL(bv, c) bv[c>>3] ^= 1 << (c & 0x07)
#define BV_TST(bv, c) (bv[c>>3] & 1 << (c & 0x07))

#define OPTIMIZE
