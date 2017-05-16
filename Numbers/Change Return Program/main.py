from decimal import *

cost = Decimal(input("Cost: "))
mGiven = Decimal(input("Money Given: "))

mToReturn = mGiven - cost;

nPenny = 0
nNickel = 0
nDime = 0
nQuater = 0

nQuater = int(mToReturn/Decimal(0.25))
mToReturn -= Decimal(nQuater)*(Decimal(25)/Decimal(100))

nDime = int(mToReturn/Decimal(0.1))
mToReturn -= Decimal(nDime)*(Decimal(1)/Decimal(10))

nNickel = int(mToReturn/Decimal(0.05))
mToReturn -= Decimal(nNickel)*(Decimal(5)/Decimal(100))

nPenny = int(mToReturn/(Decimal(1)/Decimal(100)))

print("Number of Quaters Required: ", nQuater)
print("Number of Dime Required: ", nDime)
print("Number of Nickel Required: ", nNickel)
print("Number of Penny Required: ", nPenny)
