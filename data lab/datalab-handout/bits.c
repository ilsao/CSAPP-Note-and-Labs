/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * <asciibase64 I_don't_have_userid>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  /*
  * 010111001
  * 110100101
  * 
  * 100011100
  */

  return ~(~(x&~y) & ~(~x&y));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1<<31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  //01111111111111111111111111111111
  int tmp = ~(x+1);

  int cond1 = !(x^tmp);

  // if x == -1 can causes cond1 = 1, thus we need to tackle it.
  int isNegOne = !((~x)^0);
  int result = cond1&(!isNegOne);

  return result;
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  //1010 = 10 = A
  int partOfMask = 10;
  int mask = partOfMask + (partOfMask << 4);
  mask += (mask << 8);
  mask += (mask << 16);
  return !((x & mask) ^ mask);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  /* 0001 => 1111
  *  0010 => 1110
  *  0011 => 1101
  */
  return (~x)+1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  /*
  *  0x30 = 00110000
  *  0x39 = 00111001
  */

  int maskThree = 3; // 0011
  int maskNine = 9;  // 1001

  /*
  * 1. 0...00011****
  * 2. *...*****0000 <= x <= *...*****1001
  *   - *00*
  *   - 0***
  */
  int cond1, cond2;

  cond1 = !(maskThree ^ (x >> 4));
  cond2 = !((maskNine | (x & 0xF)) ^ maskNine) | !(((x & 0xF) >> 3) ^ 0);
  
  return cond1 & cond2;
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {

  int isXTrue = !!(x ^ 0);

  int maskY = (isXTrue << 31) >> 31;
  int maskZ = ~maskY;

  return (maskY & y) | (maskZ & z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  /*
  * 1. x = y ========================> true
  * 2. x > 0, y < 0 =================> false
  * 3. x < 0, y > 0 =================> true
  * 4. x > 0, y > 0 || x < 0, y < 0 => analyze
  */

  int equal, isXNotPos, isYPos, isXMinusYPos, analyzePass, xMinusY, signX, signY, signXMinusY;

  xMinusY = x + ((~y) + 1);
  
  signX = x >> 31;
  signY = y >> 31;
  signXMinusY = xMinusY >> 31;

  equal = !(x ^ y);
  isXNotPos = (signX ^ 0);
  isYPos = !(signY ^ 0);
  isXMinusYPos = !(signXMinusY ^ 0);
  analyzePass = ((!isXNotPos & isYPos) | (isXNotPos & !isYPos)) & !isXMinusYPos;

	return equal | (isXNotPos & isYPos) | analyzePass;
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  // x = true  => false
  // x = false => true

  int mask = (x | ((~x) + 1)) >> 31;

  return (~mask & 1) | (mask & 0);
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  int signX = x >> 31;

  /* 
  * x > 0: save all bits of x
  * 
  * x < 0: 11110001 => 00001110
  * => overlook excess ones (in this case, we only need 4 + 1(sign) bits to represent)
  */
  int minXBitPresent = (~signX & x) | (signX & (~x));

  int bit16, bit8, bit4, bit2, bit1, bit0;


  // Determine if there's a 1 in high 16 bits,
  // if so, bit16 = 16(1 << 4 = 2 ** 4), and x right shift 16
  // else, don't move x.
  bit16 = (!(!!(minXBitPresent >> 16) ^ 1)) << 4;
  minXBitPresent >>= bit16;

  // If we shifted when analyzing bit16, we only need to analyze high 8 bits
  // else, we have to analyze high (32 - 8) = 24 bits
  bit8 = (!(!!(minXBitPresent >> 8) ^ 1)) << 3;
  minXBitPresent >>= bit8;

  bit4 = (!(!!(minXBitPresent >> 4) ^ 1)) << 2;
  minXBitPresent >>= bit4;

  bit2 = (!(!!(minXBitPresent >> 2) ^ 1)) << 1;
  minXBitPresent >>= bit2;

  bit1 = !(!!(minXBitPresent >> 1) ^ 1);
  minXBitPresent >>= bit1;

  bit0 = minXBitPresent;

  return bit16 + bit8 + bit4 + bit2 + bit1 + bit0 + 1;  // +1 is for the sign bit
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {

  /*
  * float (31 ~ 0)
  *   - sign = 31
  *   - exp = 30 ~ 23 
  *   - frac = 22 ~ 0
  * 
  * 1. Normalize
  *   - exp are not all 0 or 1(255)
  * 2. Denormalize
  *   - exp are all 0
  * 3. Pos infinte
  *   - exp are all 1, frac are all 0, and sign = 0
  * 4. Neg infinite
  *   - exp are all 1, frac are all 0, and sign = 1
  * 5. NaN
  *   - exp are all 1, but frac are not all 0 or 1
  */

  int sign, exp, frac;

  sign = uf >> 31;
  exp = (uf >> 23) - (sign << 8);
  frac = uf - ((exp + (sign << 8)) << 23);

  if(exp == 255) {
    return uf;
  }

  if(exp == 0) {
    // Denormalize
    frac *= 2;
    return ((sign << 31) + (exp << 23) + (frac));
  }

  // Normalize
  exp += 1;
  return ((sign << 31) + (exp << 23) + (frac));
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {

  int sign, exp, frac, error, result, E, M;

  sign = uf >> 31;
  exp = (uf >> 23) - (sign << 8);
  frac = uf - ((exp + (sign << 8)) << 23);
  error = 1 << 31;

  if(exp == 255) {
    // Special value
    return error;
  }

  if(exp == 0 && frac == 0) {
    return 0;
  }

  if(exp == 0) {
    // Denormalize
    // E = 1 - Bias = -126
    // M = 0.xx < 1
    return 0;
  }

  E = exp - 127;
  M = frac + (1 << 23); // Implied leading 1

  // Overflow
  if(E > 31) {
    return error;
  }
  if(E < 0) {
    return 0;
  }

  if(E >= 23) {
    // M * (2 ** E), since 1.xx => 1 xx we have to use (E - 23) in lieu of E
    result = M << (E - 23);
  }
  else {
    result = M >> (23 - E);
  }

  result = sign == 0 ? result : (~result + 1);

  return result;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  int posInfinite, exp;

  posInfinite = 0x7f800000;

  if(x < 0) {
    return 0;
  }

  if(x > 128){
    return posInfinite;
  }
  
  // E = exp - bias = x
  exp = x + 127;

  return (exp << 23);
}