#Find PI to the Nth Digit
#No theoretical limit

from decimal import *

n = int(input());
calculation_factor = int(n*1.66944908)+1;
getcontext().prec = 100 + calculation_factor

radius = Decimal(1)
circumfrance = Decimal(0)
temp = 2*radius*radius
temp = temp.sqrt();

for i in range(1,100 + calculation_factor):
    new_side = radius - ((radius*radius)-((temp/2)**2)).sqrt()
    temp = ((temp/2)**2) + (new_side*new_side)
    temp = temp.sqrt()

temp = temp*(2**(100 + calculation_factor))

answer = round(temp, n);

print(answer)
