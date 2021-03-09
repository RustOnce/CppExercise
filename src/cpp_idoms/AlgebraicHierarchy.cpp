#include "AlgebraicHierarchy.h"

// Number::Number(BaseConstructor)
//     : rep_(nullptr), refCount_(0)
// {
// }

// Number::Number()
//     : rep_(nullptr), refCount_(0)
// {
// }

// Number::Number(const Number &n)
//     : rep_(n.rep_), refCount_(0)
// {
//     std::cout << "Constructing a Number using Number::Number" << std::endl;
//     if(n.rep_)
//     {
//         n.rep_->refCount_++;
//     }
// }

// Number Number::makeReal(double r)
// {
//     Number n;
//     n.redefine(new RealNumber(r));
//     return n;
// }

// Number Number::makeComplex(double rpart, double ipart)
// {
//     Number n;
//     n.redefine(new Complex(rpart, ipart));
//     return n;
// }

// Number::~Number()
// {
//     if(rep_ && --rep_->refCount_ == 0)
//     {
//         delete rep_;
//     }
// }

// Number& Number::operator=(const Number& n)
// {
//     std::cout << "Assigning  a Number using Number::operator=" << std::endl;
//     Number temp(n);
//     this->swap(temp);
//     return *this;
// }
