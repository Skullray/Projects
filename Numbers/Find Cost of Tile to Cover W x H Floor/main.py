w = float(input("Enter Width of the room: "))
l = float(input("Enter Length of the room: "))

c = float(input("Cost of each tile: "))

if round(w*l,0) >= w*l:
    a = round(w*l,0);
else:
    a= round(w*l,0) + 1   

print("total cost = " ,c*round(w*l,0))


