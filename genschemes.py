def gen(l,m,n,order,N=4):
    text=""
    for i in range(l):
        for j in range(m):
            for k in range(n):
                if order>0:
                    text+="e"
                    if order>1:
                        text+=f"^{order}"
                    text+="*"
                text+=f"(a{i*N+j})(b{j*N+k})(c{k*N+i})\n"
    with open(f"{l}{m}{n}-{order}.exp",'w') as f:
        f.write(text)

def expand(oldl,oldm,oldn,l,m,n,order,filename,N=4):
    text=""
    for i in range(l):
        for j in range(m):
            for k in range(n):
                if i>=oldl or j>=oldm or k>=oldn:
                    if order>0:
                        text+="e"
                        if order>1:
                            text+=f"^{order}"
                        text+="*"
                    text+=f"(a{i*N+j})(b{j*N+k})(c{k*N+i})\n"
    with open(filename,'a') as f:
        f.write(text)
