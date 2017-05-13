from timeit import default_timer

def floor():
    for _ in range(10**7):
        43656456354678//2

def normal():
    for _ in range(10**7):
        43656456354678/2

t1 = default_timer()
floor()
t2 = default_timer()
normal()
t3 = default_timer()

print("Floor =  ", end=' ')
print(t2 - t1)
print("Normal = ", end=' ')
print(t3 - t2)
