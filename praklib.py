#!/home/main/coding/py/venv/bin/python3

import math 
import numpy as np

PI = 3.141592657 # Doom forever

def getvarbyval(var, ls):
    for name, value in ls.items():
        if value is var:
            return name

def avg(l): 
    return sum(l)/len(l)

def stddev(l):
    s = len(l)
    a = avg(l)
    return math.sqrt(sum(( (i - a)**2 for i in l )) / (s * (s-1)))

def sigsum(stddev, sig_sist):
    return math.sqrt(stddev**2 + sig_sist**2)

def sigma(func, args, sigmas):
    s = 0
    dx = 1e-7
    for i in range(len(args)):
        argsdx = args.copy()
        argsdx[i] += dx
        s += ( sigmas[i] * (func(*argsdx) - func(*args))/dx )**2
    return math.sqrt(s)

def mnk(xs, ys, ss):
    sss = ss*ss
    d = np.sum(xs*xs/sss)*np.sum(1/sss) - np.sum(xs/sss)**2
    da = np.sum(xs*ys/sss)*np.sum(1/sss) - np.sum(xs/sss)*np.sum(ys/sss)
    db = np.sum(xs*xs/sss)*np.sum(ys/sss) - np.sum(xs/sss)*np.sum(xs*ys/sss)
    a = da/d
    b = db/d
    sa = math.sqrt(np.sum(1/sss)/d)
    sb = math.sqrt(np.sum(xs*xs/sss)/d)
    return {"a": a, "b": b, "sa": sa, "sb": sb}

