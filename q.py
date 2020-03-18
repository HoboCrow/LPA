f = open("./test1.txt", "r")
n = (int)(f.readline())
a = []
for i in range(n):
    a.append(abs((int)(f.readline())))
print(max(a), max(a)**3)
