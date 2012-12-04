#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#define	G	28	/* 1 Earth gravity in accelerometer units */
#define	MID	512	/* accelerometer middle value */

#define	S	1000	/* sample rate (samples/s) */
#define	F	2	/* rotational speed in Hz */
#define	R	0.46	/* radius of accelerometer orbit */

#define	H	(G/2)	/* hysteresis, in accelerometer units */


/* noise pattern: frequency, amplitude pairs */

static struct noise {
	int f;	/* frequency (in samples) */
	int a;	/* amplitude (in accelerometer units) */
} noise[] = {
	{ 1, G/3 },	/* general noise */
	{ S/F/10, 5*G }, /* hits caused by the chain "jumping" */
	{ 0, }
};


static uint16_t sample(double t)
{
	int fz = R*4*M_PI*M_PI*F*F/9.81*G;
	int fg = cos(2*M_PI*F*t)*G;
	int f = fz+fg;
	const struct noise *n;

	for (n = noise; n->f; n++)
		if (!(random() % n->f))
			f += random() % (2*n->a) - n->a;
	return f+MID;
}


#define	E_SHIFT	3	/* ~ 0.1 */
#define	M_SHIFT	10	/* ~ 1/S */

int main(void)
{
	uint16_t e = MID << E_SHIFT;
	uint32_t m = MID << M_SHIFT;
	int d;
	int i;
	bool up = 0;

	for (i = 0; i != 10*S; i++) {
		unsigned v = sample((double) i/S);
		e = v+(e-(e >> E_SHIFT));
		m = v+(m-(m >> M_SHIFT));
		d = (e >> E_SHIFT)-(m >> M_SHIFT); 
		if (up) {
			if (d < -H)
				up = 0;
		} else {
			if (d > H)
				up = 1;
		}
		printf("%d %d %d %d %d\n",
		    v, e >> E_SHIFT, m >> M_SHIFT, d, up);
	}
	return 0;
}
