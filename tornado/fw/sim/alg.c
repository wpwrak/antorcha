#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#define	G	28	/* 1 Earth gravity in accelerometer units */
#define	MID	512	/* accelerometer middle value */

#define	S	2000	/* sample rate (samples/s) */
#define	F	2	/* rotational speed in Hz */
#define	R	0.46	/* radius of accelerometer orbit */

#define	H	(G/3)	/* hysteresis, in accelerometer units */


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


#define	E_SHIFT	8	/* ~ 0.06 */
#define	M_SHIFT	11	/* ~ 2/S */


static void process(unsigned v)
{
	static uint32_t e = MID << E_SHIFT;
	static uint32_t m = MID << M_SHIFT;
	static bool up = 0;
	int d;

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


static void usage(const char *name)
{
	fprintf(stderr, "usage: %s [seconds]\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	double t;
	char *end;
	int i;
	unsigned v;

	switch (argc) {
	case 1:
		while (scanf("%u", &v) == 1)
			process(v);
		break;
	case 2:
		t = strtod(argv[1], &end);
		if (*end)
			usage(*argv);
		for (i = 0; i != t*S; i++) {
			v = sample((double) i/S);
			process(v);
		}
		break;
	default:
		usage(*argv);
	}

	return 0;
}
