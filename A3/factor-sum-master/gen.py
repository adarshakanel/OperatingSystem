import random
with open("test2.txt","w") as f:
    for i in range(100):
        x = random.randint(0,(2**(63-2)))
        f.write(str(x) + "\n")
