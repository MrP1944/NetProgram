import math
def hypotenuse(a,b):
	return math.sqrt(a**2 + b**2)
x,y = input().split()
x = int(x)
y = int(y)
print('%d %d -> %.1f' %(x ,y, hypotenuse(x,y)))
