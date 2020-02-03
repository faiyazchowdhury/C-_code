//
// ECE2036 Program 5 - Matrix Class implementation
// Faiyaz Chowdhury
//

#include <iostream>
#include <vector>

#include "matrix.h"
#include "string-parse.h"

#include "iomanip"  // To use setw and other Print functions
using namespace std;

// Constructors here
Matrix::Matrix():rows(0),columns(0),ptr(NULL),NaM(true){}                        // Default Constructor
Matrix::Matrix(string matStr) :NaM(false) {
 	bool firstTime = true;
 	StringVec_t    rows;
    CharVec_t      delim;
    int countr = StringParse(matStr, ",", rows, delim);
    int countc;
    for(Index_t r = 0; r < countr; r++) {
    	string row = RemoveParens(rows[r]);
    	StringVec_t columns;
    	countc = StringParse(row, ",", columns, delim);
    	if(firstTime) {
            if(countr *countc != 0) {
    		  ptr = new Element_t[countr*countc];
          } else {
            ptr = NULL;
            NaM = true;
          }
    		firstTime = false;
    	}
    	Index_t c = 0;
    	while(c < countc) {
    		ptr[r*countc+c] = ToElement(columns[c]);
    		c++;
    	}
    }
    this->rows = countr;
    this->columns = countc;
}
Matrix::Matrix(Index_t r, Index_t c){
    int size = rows * columns;
    if (size != 0) {
        ptr = new Element_t[size];
        for (Index_t i = 0; i < size; i++) {
            ptr[i] = 0;
        }
    } else {
        ptr = NULL;
            NaM = true;
    }
}
Matrix::Matrix(const Matrix & CopyMat)  : rows(CopyMat.rows), columns(CopyMat.columns), NaM(CopyMat.IsNaM()) {  // Copy Constructor
    if (NaM) {
        ptr = NULL;
    } else {
        ptr = new Element_t[rows*columns];
        for (Index_t r = 0; r < rows; r++)
            for (Index_t c = 0; c < columns; c++)
                this->At(r,c) = CopyMat.At(r,c);
        }
}

// Destructor here
Matrix::~Matrix(){
    delete[] ptr;
    ptr = NULL;
}

// Operators here (Matrix addition, subtraction, multiplication)
Matrix Matrix::operator+(const Matrix rhs) const{
    if (!this->NaM && !rhs.NaM && (rows == rhs.rows) && (columns == rhs.columns)) {
        Matrix result(rows,columns);
        for ( Index_t r = 0 ; r < rows ; r++ )
            for ( Index_t c = 0 ; c < columns ; c++ )
                result.At(r,c) = this->At(r,c) + rhs.At(r,c);
        return result;
    } else {
        Matrix result(0,0);
        result.NaM = true;
        return result;
    }
}
Matrix Matrix::operator-(const Matrix rhs) const{
    if (!this->NaM && !rhs.NaM && (rows == rhs.rows) && (columns == rhs.columns)) {
        Matrix result(rows,columns);
        for ( Index_t r = 0 ; r < rows ; r++ )
            for ( Index_t c = 0 ; c < columns ; c++ )
                result.At(r,c) = this->At(r,c) - rhs.At(r,c);
        return result;
    } else {
        Matrix result(0,0);
        result.NaM = true;
        return result;
    }
}
Matrix Matrix::operator*(const Matrix rhs) const{
    if (!this->NaM && !rhs.NaM && (rows == rhs.rows) && (columns == rhs.columns)) {
        Matrix result(rows,columns);
        for ( Index_t r = 0 ; r < rows ; r++ )
            for ( Index_t c = 0 ; c < columns ; c++ )
                result.At(r,c) = this->At(r,c) * rhs.At(r,c);
        return result;
    } else {
        Matrix result(0,0);
        result.NaM = true;
        return result;
    }
}
Matrix & Matrix::operator=(const Matrix & rhs){    // lhs and rhs refer to the same Matrix
    if (this == &rhs)
        return *this;

    delete[] ptr;

    rows = rhs.rows;
    columns = rhs.columns;
    NaM = rhs.NaM;



    if(NaM) {
        ptr = NULL;
    } else {
        ptr = new Element_t[rows*columns];
        for (Index_t r = 0; r < rows; r++)
            for (Index_t c = 0; c < columns; c++)
                this->At(r,c) = rhs.At(r,c);
        }
    return *this;
}
// Other member functions here

bool Matrix::IsNaM() const{
    return NaM;
}
Element_t   Matrix::At(Index_t r, Index_t c) const{   //"Getter"
    return ptr[r*columns+c];
}
Element_t & Matrix::At(Index_t r, Index_t c){         //"Setter"
    return ptr[r*columns+c];
}
 void Matrix::Print() const {
    if (NaM) {
        cout<<"NaM"<<endl;
    } else {
        for (Index_t r = 0; r < rows; r++) {
            for (Index_t c = 0; c < columns; c++) {
                cout << setw(5) << right << this->At(r,c);
            }
            cout<<endl;
        }
    }
 }
