//
// ECE2036 Program 2 - Matrix Calculator
// Faiyaz Chowdhury
//

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "matrix.h"
#include "string-parse.h"

using namespace std;

int main()
{
  Matrix result, op1, op2;
 // Your declarations here
  while(true)
    {
      StringVec_t    operands;
      CharVec_t      delim;
      string         line;
      line = GetStringFromKeyboard(); // Read input line
      int count = StringParse(line, "+-*", operands, delim);
      if (!count) break; // All done
      //Set operand/result values from input string.
      if (count == 1) {
          Matrix temp(operands[0]);
          result = temp;
      } else {
          if (operands[0].empty()) {
              op1 = result;
          } else {
              Matrix temp(operands[0]);
              op1 = temp;
          }
          Matrix temp(operands[1]);
          op2 = temp;
      }
      //Chose and perform given operation.
      switch (delim[0]) {
        case '+':
            result = op1 + op2;
            break;
        case '-':
            result = op1 - op2;
            break;
        case '*':
            result = op1 * op2;
            break;
      }


      //Print the result to the console.
      result.Print();
      cout<<endl;


      // Your code here
      result = result; // !! Leave this here to test proper self-assignment
    }
  return 0;  /* Successful completion of main() */
}
