#Find e to the Nth Digit
#No theoretical limit

from decimal import *

fret = Decimal(1)
def ffac(n):
    global fret
    fret *= n
    return fret

def maxdiff(n):
    tpow = 10**n
    x = 1
    while True:
        if tpow > x:
            tpow = tpow//x
            x += 1
        else:
            return x+1

n = int(input());
getcontext().prec = n+50
temp = 1
for x in range(1,maxdiff(n)):
    temp += Decimal(1)/ffac(x)

answer = round(temp, n);

print(answer)
