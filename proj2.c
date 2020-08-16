/**
* IZP Proj2
* Autor: Iveta Strnadova, xstrna14
* Datum 21.11.2018
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

/**
* Function extracts integer from given text
* @param argument[] - the string from which the number is extracted
* @param number - pointer on integer, where the result value is stored
* @return 0 upon successful conversion
* @return 1 if the conversion failed (when there is something else beside the number or it is not a whole number)
*/
int getInteger(char argument[], int *number)
{
    char *pointerOnExpectedEnd;
    int resultNumber;
    resultNumber=strtol(argument, &pointerOnExpectedEnd, 10);
    if((*pointerOnExpectedEnd)=='\0'&&(resultNumber>0))
    {
        *number=resultNumber;
        return 0;
    }
    printf("Failed to convert number from argument %s.\n", argument);
    return 1;
}

/**
* Function extracts double from given text
* @param argument[] - the string from which the number is extracted
* @param number - pointer on double, where the result value is stored
* @return 0 upon successful conversion
* @return 1 if the conversion failed (when there is something else beside the number or it is not a whole number)
*/
int getDouble(char argument[], double *number)
{
    char *pointerOnExpectedEnd;
    double resultNumber;
    resultNumber=strtod(argument, &pointerOnExpectedEnd);
    if((*pointerOnExpectedEnd)=='\0')
    {
        *number=resultNumber;
        return 0;
    }
    printf("Failed to convert number from argument %s.\n", argument);
    return 1;
}

/**
* Function calculates value of natual logarithm from x with use of Taylor polynomial
* @param x - number, from which the value of ln(x) is to be calculated
* @param n - number of iterations (determines the accuracy of result)
* @return sum - result value of taylor_log
*/
double taylor_log(double x, unsigned int n)
{
    ///upperFraction will be multiplied by y each turn - initiate value 1
    ///sum is the variable in which every partial fraction is added to - initiate value 0
    double y, upperFraction=1.0, sum=0.0;

    if(x<=1&&x>0)
    ///For x<=1 && x>0, ln(x) is calculated from formule ln(1-y)=-y-(y^2/2)-(y^3/3)-(y^4/4)-...
    {
        y=1-x;
        for(unsigned int i=0; i<n&&((isinf(sum)==0)||isnan(sum)==0);i++)
        {
            upperFraction=upperFraction*y;
            sum=sum-(upperFraction/(i+1));
        }
        return sum;
    }
    else if(x==0) return -INFINITY;
    else if(x<0) return NAN;
    else
    ///For x>1 ln(x) is calculated from formule ln(x)=sum(((x-1)/x)^n/n)
    {
        for(unsigned int i=0;i<n&&(isinf(sum)==0);i++)
        {
            upperFraction=upperFraction*((x-1)/x);
            sum=sum+upperFraction/(i+1);
        }
        return sum;
    }
}

/**
* Function calculates value of natual logarithm from x with use of continued fraction
* @param x - number, from which the value of ln(x) is to be calculated
* @param n - number of iterations (determines the accuracy of result)
* @return sum - result value of cfrag_log
*/
double cfrac_log(double x, unsigned int n)
{
    double temp, z, upperFraction, lowerFraction;
    temp=0.0;
    n--; ///To fix the iteration count, as my implementation counted the first fraction as 0th one, doing one more iteration extra

    if(x==0) return -INFINITY;
    if(x<0) return NAN;

    ///Caltulating from formule ln((1+z)/(1-z))=2z/(1-(z^2/3-(4z^2/(5-(9z^2/(7-...))))))
    z=(x-1)/(x+1);
    for(;n>=1&&(isinf(temp)==0);n--) ///going from smaller fractions to the original one
    {
        upperFraction=n*n*z*z;
        lowerFraction=2*n+1-temp;
        temp=upperFraction/lowerFraction;
    }
    return (2*z)/(1-temp);
}

/**
* Function calculates value of x on the power of y using Taylor polynomial and taylor_log or cfrac_log defined earlier
* @param x - number, which is calculated on power of y
* @param y - number
* @param n - number of iterations (determines the accuracy of result)
* @return sum - return value of taylor_pow
*/
double universal_pow(double x, double y, unsigned int n, int decide)
{
    double upperFraction=1.0, lowerFraction=1.0, sum=1.0, temp, lnx;

    if(decide==1) lnx=taylor_log(x, n);
    if(decide==0) lnx=cfrac_log(x, n);

    n--; ///To fix the iteration count, as my implementation counted the first polynome as 0th one, doing one more iteration extra
    temp=y*lnx;
    for(unsigned int i=0;i<n&&isinf(sum)==0;i++)
    {
        upperFraction=upperFraction*temp;
        lowerFraction=lowerFraction*(i+1);
	if(isnan(lowerFraction)==1) return sum;
        sum=sum+upperFraction/lowerFraction;
    }
    return sum;
}

/**
* Function calls universal_pow with specific parametrs, dealing with specific cases of x and y
* @param x - number, which is calculated on power of y
* @param y - number
* @param n - number of iterations (determines the accuracy of result)
* @return sum - return value of taylor_pow
*/
double taylor_pow(double x, double y, unsigned int n)
{
    if(y==0) return 1;
    if(x==0&&y>0) return 0;
    if(x==0&&y<0) return INFINITY;

    if(x>0&&y>0) return universal_pow(x, y, n, 1);
    if(x<0&&y>0)
    {
        if(y-(int)y!=0) return NAN;
        if((int)y%2==0) return universal_pow(fabs(x), y, n, 1);
        return -1*universal_pow(fabs(x), y, n, 1);
    }
    ///x^-y == 1/(x^y)
    if(x>0&&y<0) return 1/universal_pow(x, fabs(y), n, 1);
    if(x<0&&y<0)
    {
        if(y-(int)y!=0) return NAN;
        if((int)y%2==0) return 1/universal_pow(fabs(x), fabs(y), n, 1);
        return -1*(1/universal_pow(fabs(x), fabs(y), n, 1));
    }
    return NAN;
}

/**
* Function calls universal_pow with specific parametrs, dealing with specific cases of x and y
* @param x - number, which is calculated on power of y
* @param y - number
* @param n - number of iterations (determines the accuracy of result)
* @return sum - return value of taylorcf_pow
*/
double taylorcf_pow(double x, double y, unsigned int n)
{
    if(y==0) return 1;
    if(x==0&&y>0) return 0;
    if(x==0&&y<0) return INFINITY;

    if(x>0&&y>0) return universal_pow(x, y, n, 0);
    if(x<0&&y>0)
    {
        if(y-(int)y!=0) return NAN;
        if((int)y%2==0) return universal_pow(fabs(x), y, n, 0);
        return -1*universal_pow(fabs(x), y, n, 0);
    }
    ///x^-y == 1/(x^y)
    if(x>0&&y<0) return 1/universal_pow(x, fabs(y), n, 0);
    if(x<0&&y<0)
    {
        if(y-(int)y!=0) return NAN;
        if((int)y%2==0) return 1/universal_pow(fabs(x), fabs(y), n, 0);
        return -1*(1/universal_pow(fabs(x), fabs(y), n, 0));
    }
    return NAN;
}

/**
* Function uses different methods of calculating ln(x) to print them out for comparison
* @param x - number from which ln is calculated
* @param n - number of iterations (bigger number, more accurate result)
*/
void printLog(double x, int n)
{
    printf("       log(%g) = %0.12g\n", x, log(x));
    printf(" cfrac_log(%g) = %0.12g\n", x, cfrac_log(x, n));
    printf("taylor_log(%g) = %0.12g\n", x, taylor_log(x, n));
}

/**
* Function uses different methods of calculating x^y to print them out for comparison
* @param x
* @param y
* @param n - number of iterations (bigger number, more accurate result)
*/
void printPow(double x, double y, int n)
{
    printf("         pow(%g,%g) = %0.12g\n", x, y, pow(x, y));
    printf("  taylor_pow(%g,%g) = %0.12g\n", x, y, taylor_pow(x, y, n));
    printf("taylorcf_pow(%g,%g) = %0.12g\n", x, y, taylorcf_pow(x, y, n));
}

int main(int argc, char* argv[])
{
    int n, errorCheck=0;
    double x, y;

    if((argc==4)&&(strcmp(argv[1], "--log")==0))
    {
        ///In case any of the numbers isn't loaded correctly, one is added to errorCheck, any number other than 0 in errorCheck results is ending of program
        errorCheck+=getDouble(argv[2], &x);
        errorCheck+=getInteger(argv[3], &n);
        if(n<=0||errorCheck!=0)
        {
            if(n<=0) printf("Unvalid second number. (Required whole number bigger than 0)\n");
            return 1;
        }
        printLog(x, n);
    }
    else if((argc==5)&&(strcmp(argv[1], "--pow")==0))
    {
        errorCheck+=getDouble(argv[2], &x);
        errorCheck+=getDouble(argv[3], &y);
        errorCheck+=getInteger(argv[4], &n);
        if(n<=0||errorCheck!=0)
        {
            if(n<=0) printf("Unvalid third number. (Required whole number bigger than 0)\n");
            return 1;
        }
        printPow(x,y,n);
    }
    else
    {
        printf("Function not recognised. Invalid number of arguments or wrong command.\n");
        return 1;
    }
    return 0;
}
