#Fibonacci Sequence

n = int(input())
last = 0
current = 1
print(1, end=' ')
for i in range(0,n):
    print(last+current, end='')
    if(n==last+current):
        break;
    if i != n:
        print(" ",end='')
    temp = current;
    current +=last;
    last = temp;
