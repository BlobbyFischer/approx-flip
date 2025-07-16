def gen(l,m,n,order):
    text=""
    for i in range(l):
        for j in range(m):
            for k in range(n):
                if order>0:
                    text+="e"
                    if order>1:
                        text+=f"^{order}"
                    text+="*"
                text+=f"(a{i*m+j})(b{j*n+k})(c{k*l+i})\n"
    with open(f"{l}{m}{n}-{order}.exp",'w') as f:
        f.write(text)
