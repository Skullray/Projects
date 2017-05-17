import numpy as np

binary_arr = np.zeros(64)

def pr():
    global binary_arr;
    for i in range(0,64):
        print(binary_arr[i])

ch = str(input("Convert to Binary(B/b) or Decimal(D/d): "))

if ch == 'B' or ch == 'b':
    num = float(input("Number: "))
    if round(num,0) == num:
        num = int(num)
        if num>0:
            binary_arr[0] = 0
        else:
            binary_arr[0] = 1
        for i in range(0,63):
            binary_arr[63-i] = num % 2
            num = (num - binary_arr[63-i])/2
            if num == 0:
                break;
    else:

else if ch == 'D' or ch == 'd':
    num = int(input("Number: "))
    ch = str(input("Is the number Integer(I/i) or Decimal(D/d): "))
    if ch == 'I' or ch == 'i':

    else if ch == 'D' or ch == 'd':
