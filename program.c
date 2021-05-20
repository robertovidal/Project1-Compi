#include "print.c"
#define SIZE 10
#define formula(a,b,c) a + b*c
#define toString(x) ??=x
#define print(a) printf(a)
#define pr(a) print(a)
#define cycle(x , y,z) for(x=0; x<=y; x++)\
	{\
		printf("Enter the coefficient of x to the power %d: ",x);\
		scanf("%f",&z<:x:>);\
	}
#defin A 20
#incluede "program.c"
float poly(float a[], int, float);

int main()
??<
	float x, a[SIZE], y1;
	int deg, i;

	pr(toString    (Enter the degree of polynomial equation: ));
	scanf("%d", &deg);

	printf("Enter the value of x for which the equation is to be evaluated: ");
	scanf("%f", &x);

	cycle(i, deg,a);

	y1 = poly(a, deg, x);
	
	printf("The value of polynomial equation for the value of x = %.2f is: %.2f",x,y1);
	
	return 0;
}

/* function for finding the value of polynomial at some value of x */
float poly(float a[], int deg, float x)
{
	float p;
	int i;
	
	p = a[deg];
	
	for(i=deg;i>=1;i--)
	{
		p = formula (a[i-1],x,p);
	}
	
	return p;
}