from random import randint

n = 5
for n in range(1, 10):
    print(2*n)
    for i in range(1, n+1):
        for j in range(i, n+1):
            if (i == j):
                continue
            print(f"{i} {j} {1}")
    # link each to a new node
    for i in range(1, n+1):
        new = n+i
        print(f"{i} {new} {999}")
    print(0)
