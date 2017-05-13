from math import sqrt

def isprime(n):
    for i in range(2,int(sqrt(n))+1):
        if(n%i == 0):
            return False
    return True

print(2)

n = input("Do You Want The Next Prime(Y/n): ")
if(n in "Yy"):
    x = 3
    while True:
        if(isprime(x)):
            print(x)
            n = input("Do You Want The Next Prime(Y/n): ")
            if(n in "Nn"):
                break
        x += 1
