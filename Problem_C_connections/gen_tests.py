from random import randint

# n = 5
# for n in range(1, 10):
#     print(2*n)
#     for i in range(1, n+1):
#         for j in range(i, n+1):
#             if (i == j):
#                 continue
#             print(f"{i} {j} {1}")
#     # link each to a new node
#     for i in range(1, n+1):
#         new = n+i
#         print(f"{i} {new} {999}")
#     print(0)

# #circles
# for n in range(2, 10):
#     print(n)

#     for i in range(1, n):
#         print(f"{i} {i+1} 1")
#     print(f"{n} 1 1")

#     print(0)

# circles servers
# for n in range(2, 10):
#     print(2*n)
#     for i in range(1, n):
#         print(f"{i} {i+1} 1")
#     print(f"{n} 1 1")

#     for i in range(1, n+1):
#         print(f"{i} {n+i} 999")

#     print(0)

# disconnected circle servers


def circle_servers(s, n, f):
    for i in range(s, n):
        f.write(f"{i} {i+1} {randint(1,10)}\n")
        # print(f"{i} {i+1} {randint(1,10)}")
    # print(f"{n} {s} {randint(1,10)}")
    f.write(f"{n} {s} {randint(1,10)}\n")

    for i in range(s, n+1):
        # print(f"{i} {n+i-s+1} 999")
        f.write(f"{i} {n+i-s+1} 999\n")


n = 22
with open(f"./inputs/{n}_circle_servers.txt", "w") as f:
    f.write(f"{2*n}\n")
    # print(2*n)
    circle_servers(1, n, f)
    f.write(f"0\n")
print(0)

# n = 5
# print(4*n)
# for i in range(1, 4*n+1, 4):
#     print(f"{i} {i+1} {1}")
#     print(f"{i+1} {i+2} {2}")
#     print(f"{i+2} {i+3} {3}")
# print(0)

# random connections

# n = 20
# links = 20
# print(n)
# for i in range(1, links):
#     print(f'{randint(1,n)} {randint(1,n)} {randint(1,100)}')
# print(0)
