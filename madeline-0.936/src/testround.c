
#include "tools.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

int main(int argc,char *argv[]){

	double Round(double);

	double x1=15.4;
	double x2=15.5;
	double x3=15.6;

	std::cout << Round(x1) << " " << Round(x2) << " " << Round(x3) << std::endl;

	return 0;
}

/*                                */
/* Round(): Rounds away from zero */
/*                                */
double Round( double v ){
	
	double integer;
	return modf(v,&integer) >= 0.5 ? ++integer : integer ;

}

