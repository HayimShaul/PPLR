import math
import argparse


parser = argparse.ArgumentParser()
parser.add_argument('--m', type=int, help="modulo.", default=101)
parser.add_argument('--r', type=int, help="numerator.", default=3)
parser.add_argument('--s', type=int, help="denominator.", default=5)

args = parser.parse_args()

m = args.m
r = args.r
s = args.s


def powermod(a, b, n):
    r = 1
    while b > 0:
        if b & 1 == 1:# odd number
            r = r * a % n
            b = b - 1

        b /= 2
        a = a * a % n
    return r

def inv(x):
    return powermod(x, m-2, m)


def wang_orig(c):
    (u1,u2,u3) = (1, 0, m)
    (v1,v2,v3) = (0, 1, c)

    while math.sqrt(m/2) <= v3:
        q = int(u3/v3)
        (r1,r2,r3) = (u1-q*v1, u2-q*v2, u3-q*v3)
        (u1,u2,u3) = (v1,v2,v3)
        (v1,v2,v3) = (r1,r2,r3)

    return (v3, v2)


def extra_iterations(c):
    (u1,u2,u3) = (1, 0, m)
    (v1,v2,v3) = (0, 1, c)

    while math.sqrt(m/2) <= v3:
        print("u = ({},{},{})   v = ({},{},{})".format(u1,u2,u3, v1,v2,v3))
        q = int(u3/v3)
        (r1,r2,r3) = ((u1-q*v1)%m, (u2-q*v2)%m, (u3-q*v3)%m)
        (u1,u2,u3) = (v1,v2,v3)
        (v1,v2,v3) = (r1,r2,r3)

    for i in xrange(5):
        print("u = ({},{},{})   v = ({},{},{})".format(u1,u2,u3, v1,v2,v3))
        q = int(u3/v3)
        (r1,r2,r3) = ((u1-q*v1)%m, (u2-q*v2)%m, (u3-q*v3)%m)
        (u1,u2,u3) = (v1,v2,v3)
        (v1,v2,v3) = (r1,r2,r3)

    return (v3, v2)

def power2_floor(x):
    if x < 1:
        return 0
    p = 1
    while p*2 < x:
        p = p*2
    return p


def power_2_substraction(c):
    (u1,u2,u3) = (1, 0, m)
    (v1,v2,v3) = (0, 1, c)

    print("u = ({},{},{})   v = ({},{},{})".format(u1,u2,u3, v1,v2,v3))
    while math.sqrt(m/2) <= v3:
        q = power2_floor(int(u3/v3))
        print("q = {}   while u3/v2={}".format(q, int(u3/v3)))
        (r1,r2,r3) = ((u1-q*v1)%m, (u2-q*v2)%m, (u3-q*v3)%m)
        (u1,u2,u3) = (v1,v2,v3)
        (v1,v2,v3) = (r1,r2,r3)
        print("u = ({},{},{})   v = ({},{},{})".format(u1,u2,u3, v1,v2,v3))

    return (v3, v2)







if r > math.sqrt(m/2):
    print("nominator cannot be bigger than {}".format(math.sqrt(m/2)))
    exit(1)
if s > math.sqrt(m/2):
    print("denominator cannot be bigger than {}".format(math.sqrt(m/2)))
    exit(1)

t = (r * inv(s)) % m

print("wang orig: {}".format(wang_orig(t)))
#print("extra iterations: {}".format(extra_iterations(t)))
print("power 2 substraction: {}".format(power_2_substraction(t)))

