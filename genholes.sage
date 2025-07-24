def gen1(l,m,n,N=4):
    text=""
    for i in range(l):
        for j in range(m):
            for k in range(n):
                if i%2==1 or j%2==1:
                    text+=f"e*(a{i*N+j})(b{j*N+k})(c{k*N+i})\n"
    print(text)

def expand1(oldl,oldm,oldn,l,m,n,filename,N=4):
    text=""
    for i in range(l):
        for j in range(m):
            for k in range(n):
                if (i%2==1 or j%2==1) and (i>=oldl or j>=oldm or k>=oldn):
                    text+=f"e*(a{i*N+j})(b{j*N+k})(c{k*N+i})\n"
    with open(filename,'a') as f:
        f.write(text)

def gen2(l,m,n,N=4):
    text=""
    for i in range(l):
        for j in range(m):
            for k in range(n):
                if i==0 or j==0:
                    text+=f"e*(a{i*N+j})(b{j*N+k})(c{k*N+i})\n"
    print(text)

def expand2(oldl,oldm,oldn,l,m,n,filename,N=4):
    text=""
    for i in range(l):
        for j in range(m):
            for k in range(n):
                if (i==0 or j==0) and (i>=oldl or j>=oldm or k>=oldn):
                    text+=f"e*(a{i*N+j})(b{j*N+k})(c{k*N+i})\n"
    with open(filename,'a') as f:
        f.write(text)

def gen(l,m,n,order=1,aholes=[],bholes=[],N=4):
    text=""
    for i in range(l):
        for j in range(m):
            for k in range(n):
                if not( (i,j) in aholes or (j,k) in bholes):
                    text+=f"e^{order}*(a{i*N+j})(b{j*N+k})(c{k*N+i})\n"
    with open(f"{l}{m}{n}-{len(aholes)}{len(bholes)}.exp",'w') as f:
        f.write(text)

def get(Ts,Ss,res=1e-15):
    f(w) = sum([i**(w/3) for i in Ss]) - sum([i**(w/3) for i in Ts])
    if f(2)==0:
        return 2
    if f(2)>0 or f(3)<0:
        raise ValueError
    lo=float(2)
    hi=float(3)
    while (hi-lo)/2>res:
        m=(lo+hi)/2
        fm=f(m)
        if fm==0:
            return m
        if fm>0:
            hi=m
        else:
            lo=m
    return (hi+lo)/2

