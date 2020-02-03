#include <iostream>
#include <string>

#include <math.h>

#include "complex.h"

using namespace std;

Complex::Complex()
{}
Complex::Complex(double f, double g)
{
    a = f;
    b = g;
}

double Complex::getA() const
{
    return a;
}

double Complex::getB() const
{
    return b;
}

void Complex::setA(double f)
{
    a = f;
}

void Complex::setB(double g)
{
    b = g;
}

Complex Complex::operator + ( Complex const& RHS )
{
  double r = getA() + RHS.getA();
  double i = getB() + RHS.getB();  
  Complex result(r,i);
  return result;
}

Complex Complex::operator - ( Complex const& RHS )
{
  double r = getA() - RHS.getA();
  double i = getB() - RHS.getB();
  Complex result(r,i);
  return result;
}

Complex operator * ( Complex const& RHS, Complex const& LHS  )
{
    double r = (LHS.getA() * RHS.getA()) - (LHS.getB() * RHS.getB()) ;
    double i = (LHS.getA() * RHS.getB()) + (LHS.getB() * RHS.getA());
    Complex result(r,i);
    return result;
}
