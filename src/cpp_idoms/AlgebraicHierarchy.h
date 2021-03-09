#pragma once

#include <iostream>

// struct BaseConstructor 
// {
//     BaseConstructor(int=0)
//     {}
// };

// class RealNumber;
// class Complex;
// class Number;

// class Number
// {
//     friend class RealNumber;
//     friend class Complex;
    
// public:
//     Number& operator=(const Number& n);
//     Number(const Number&n);
//     virtual ~Number();

//     virtual Number operator+(const Number& n) const;
//     void swap(Number& n) throw();

//     static Number makeReal(double r);
//     static Number makeComplex(double rpart, double ipart);

// protected:
//     Number();
//     Number(BaseConstructor);

// private:
//     void redefine(Number* n);
//     virtual Number complexAdd(const Complex& n) const;
//     virtual Number realAdd(const RealNumber& n) const;

//     Number* rep_;
//     short refCount_;
// }