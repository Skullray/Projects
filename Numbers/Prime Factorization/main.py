#Prime Factorization

from math import sqrt
from sys import stdout

def isprime(n):
    for i in range(2,int(sqrt(n))+1):
        if(n%i == 0):
            return False
    return True

n = int(input())
tn = n
first = True
for i in range(2,int(n/2)+1):
    if(n%i == 0):
        if(isprime(i)):
            if(first):
                first = False
            else:
                print(" ", end = '')

            print(i, end = '')
            stdout.flush()
        n = n/i
if(isprime(tn)):
    if(first==False):
        print(" ", end = '')
    print(tn, end = '')
