// $Id: bigint.cpp,v 1.225 2014-04-29 01:28:15-07 - - $
//
// Partner: Jonathan Lu (jalu@ucsc.edu)
// Partner: Yong Ning Hu (cryhu@ucsc.edu)

#include <cassert>
#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"


bigint::bigint() {
   negative = false;
}

bigint::bigint (const bigint& that) {
   *this = that;
}

bigint& bigint::operator= (const bigint& that) {
   if (this == &that) return *this;
   this->negative = that.negative;
   this->big_value = that.big_value;
   return *this;
}

bigint::~bigint() {
   big_value.clear();
}

bigint::bigint (long that) {
   negative = (that < 0 ? true: false);
   if(that == 0) big_value.push_back(0);
   while(that > 0) {
      int digit = that%10;
      big_value.push_back(digit);
      that = that/10;
   }
   this->trimzero(); 
}

bigint::bigint (const string& that) {
   assert (that.size() > 0);
   auto itor = that.cbegin();
   negative = false;
   if (*itor == '_') {
      negative = true;
   }
   string:: const_reverse_iterator ritor = that.rbegin();
   string:: const_reverse_iterator rend = that.rend();
   for(; ritor != rend; ++ritor) {
      if(*ritor == '_') break;
      big_value.push_back(*ritor - '0');
   }
   this->trimzero();
   DEBUGF ('e', big_value.size());
}

void bigint::trimzero () {
   auto itor = this->big_value.rbegin();
   auto end = this->big_value.rend();
   while(itor != end) {
      if((int)*itor == 0) {
         ++itor;
         this->big_value.pop_back();
      } else break;
   }
   if(this->big_value.empty()) {
      this->big_value.push_back(0);
      this->negative = false;
   }
}

int bigint::do_big_comp(const bigint& right) const{
   if(big_value.size() < right.big_value.size()) return -1;
   if(right.big_value.size() < big_value.size()) return 1;
   for(int i = right.big_value.size()-1; i >= 0; --i) {
      if(big_value[i] < right.big_value[i]) return -1;
      if(right.big_value[i] < big_value[i]) return 1;
   }
   return 0;
}

bigvalue_t bigint::do_bigadd (const bigvalue_t& left, 
                      const bigvalue_t& right) const{
   digit_t carry = 0;
   digit_t sum = 0;
   bigvalue_t result;
   bigvalue_t::const_iterator litor = left.cbegin();
   bigvalue_t::const_iterator ritor = right.cbegin();
   bigvalue_t::const_iterator lend = left.cend();
   bigvalue_t::const_iterator rend = right.cend();
   while(litor != lend || ritor != rend) {
      digit_t leftSum = (litor == lend ? 0: *litor);
      digit_t rightSum = (ritor == rend ? 0: *ritor);
      sum = (leftSum + rightSum + carry)%10;
      result.push_back(sum);
      carry = (leftSum + rightSum + carry)/10;
      if(litor != lend) ++litor;
      if(ritor != rend) ++ritor;
      if(litor == lend && ritor == rend) {
         if(carry != 0) result.push_back(carry);
      }
   } 
   return result;
}

bigvalue_t bigint::do_bigsub(const bigvalue_t& left, 
                     const bigvalue_t& right) const{
   digit_t sum = 0;
   bigvalue_t result = left;
   bigvalue_t::iterator litor = result.begin();
   bigvalue_t::const_iterator ritor = right.cbegin();
   bigvalue_t::iterator lend = result.end();
   bigvalue_t::const_iterator rend = right.cend();
   while(litor != lend || ritor != rend) {
      digit_t leftSum = (litor == lend ? 0: *litor);
      digit_t rightSum = (ritor == rend ? 0: *ritor);
      if(leftSum < rightSum) { 
         leftSum = leftSum + 10;
         bool borrowing = true;
         while(borrowing) {
            bigvalue_t::iterator lender = litor;
            ++lender;
            while(*lender == 0) {
               *lender = 9;
               ++lender;
            }
            *lender = *lender - 1;
            borrowing = false;
         }
      } 
      sum = leftSum - rightSum;
      *litor = sum;
      if(litor != lend) ++litor;
      if(ritor != rend) ++ritor;
   }
   return result;
}

bigvalue_t bigint::do_bigmul(const bigvalue_t& left,
                             const bigvalue_t& right) const{
   bigvalue_t result;
   result.assign(left.size() + right.size(), 0);
   int carry = 0;
   for(size_t i = 0; i < left.size(); i++) {
      carry = 0;
      for(size_t j = 0; j < right.size(); j++) {
         int sum = result[i+j] + (left[i]*right[j]) + carry;
         result[i+j] = sum%10;
         carry = sum/10;
      }
      result[i+right.size()] = carry;
   }
   return result;
}


bigint bigint::operator+ (const bigint& that) const {
   bigint result;
   if((this->negative == true && that.negative == true) ||
      (this->negative == false && that.negative == false)) {
      result.big_value = this->do_bigadd(this->big_value,
                                         that.big_value);
      if(this->negative == true)result.negative = true;
   } else {
      if(that.do_big_comp(*this) < 0) {
         result.big_value = this->do_bigsub(this->big_value,
                                            that.big_value);
         if(this->negative == true) {
            result.negative = true;
         }
      } else {
         result.big_value = this->do_bigsub(that.big_value,
                                            this->big_value);
         if(this->negative != true) {
            result.negative = true;
         }
      }
   }
   result.trimzero();
   return result;
}

bigint bigint::operator- (const bigint& that) const {
   bigint result;
   if((this->negative == true && that.negative == false) ||
      (this->negative == false && that.negative == true)) {
      result.big_value = this->do_bigadd(this->big_value,
                                         that.big_value);
      if(this->negative == true) {
         result.negative = true;
      }
   } else {
      if(that.do_big_comp(*this) < 0) {
         result.big_value = this->do_bigsub(this->big_value, 
                                            that.big_value);
         if(this->negative == true && that.negative == true) {
            result.negative = true;
         }
      } else {
         result.big_value = this->do_bigsub(that.big_value,
                                            this->big_value);
         if(this->negative != true && that.negative != true) {
            result.negative = true;
         }
      }
   }
   result.trimzero();
   return result;
}

bigint bigint::operator-() const {
   bigint result = *this;
   result.negative = !(this->negative);
   return result;
}

long bigint::to_long() const {
   if (*this <= bigint (numeric_limits<long>::min())
    or *this > bigint (numeric_limits<long>::max()))
               throw range_error ("to_long: out of range");
   long result = 0;
   long tenMul = 1;
   bigvalue_t::const_iterator itor = this->big_value.cbegin();
   bigvalue_t::const_iterator end = this->big_value.cend();
   while(itor != end) {
      result = result + (*itor)*tenMul;
      tenMul = tenMul * 10;
      ++itor;
   }
   return result;
}

bool abs_less (const bigint& left, const bigint& right) {
   return left.do_big_comp(right) < 0;
}

//
// Multiplication algorithm.
//
bigint bigint::operator* (const bigint& that) const {
   bigint result;
   if(this->negative xor that.negative) result.negative = true;
   result.big_value = this->do_bigmul(this->big_value, that.big_value);
   result.trimzero();
   return result;
}

//
// Division algorithm.
//

void mul_by_2 (bigint& multiplier) {
   if(multiplier.big_value.empty()) return;
   bigvalue_t &value = multiplier.big_value;
   digit_t carry = 0;
   for(size_t i = 0; i < value.size(); ++i) {
      digit_t sum = value[i] * 2 + carry;
      value[i] = sum % 10;
      carry = sum / 10;
   }
   if(carry != 0) value.push_back(carry);
}

void div_by_2 (bigint& divisor) {
   if(divisor.big_value.empty()) return;
   for(unsigned int i = 0; i < divisor.big_value.size(); ++i){
      int result = divisor.big_value[i] / 2;
      if(i < divisor.big_value.size() - 1
            && divisor.big_value[i + 1] % 2 == 1) result += 5;
      divisor.big_value[i] = result;
   }
   divisor.trimzero();
}


bigint::quotient_remainder bigint::divide (const bigint& that) const {
   if (that == 0) throw domain_error ("divide by 0");
   typedef bigint unumber;
   static unumber zero = 0;
   if (that == 0) throw domain_error ("bigint::divide");
   unumber divisor = that;
   unumber quotient = 0;
   unumber remainder = *this;
   unumber power_of_2 = 1;
   while (abs_less (divisor, remainder)) {
      mul_by_2 (divisor);
      mul_by_2 (power_of_2);
   }
   while (abs_less (zero, power_of_2)) {
      if (!abs_less (remainder, divisor)) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      div_by_2 (divisor);
      div_by_2 (power_of_2);
   }
   return {quotient, remainder};
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result = divide(that).first;
   if(negative != that.negative) result.negative = true;
   return divide (that).first;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = divide(that).second;
   if(negative != that.negative) result.negative = true;
   return divide (that).second;
}

bool bigint::operator== (const bigint& that) const {
   return (negative == that.negative and do_big_comp(that) == 0);
}

bool bigint::operator< (const bigint& that) const {
   if(this->negative == true && that.negative == true) {
      return abs_less(that, *this);
   }
   if(this->negative == true && that.negative == false) {
      return true;
   }
   if(this->negative == false && that.negative == false) {
      return abs_less(*this, that);
   }
   
   return false;
}


ostream& operator<< (ostream& out, const bigint& that) {
   if(that.negative) out << "-";
   auto itor = that.big_value.rbegin();
   int count = 0;
   int width = 69;
   for(; itor != that.big_value.rend(); ++itor) {
      out << static_cast<unsigned>(*itor);
      ++count;
      if(count == width) {
         out << "\\" << endl;
         count = 0;
      }
   }
   return out;
}

bigint pow (const bigint& base, const bigint& exponent) {
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   if (base == 0) return 0;
   bigint base_copy = base;
   long expt = exponent.to_long();
   bigint result = 1;
   if (expt < 0) {
      base_copy = 1 / base_copy;
      expt = - expt;
   }
   while (expt > 0) {
      if (expt & 1) { //odd
         result = result * base_copy;
         --expt;
      }else { //even
         base_copy = base_copy * base_copy;
         expt /= 2;
      }
   }
   DEBUGF ('^', "result = " << result);
   return result;
}
