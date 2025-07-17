def group_mul_tensor(G,order=1,filename=0):
    elements = list(G)
    index = {g:i for i,g in enumerate(elements)}
    text = ""
    for i,g in enumerate(elements):
        for j,h in enumerate(elements):
            l = g * h
            k = index[l]
            text += f"e^{order}*(a{i})(b{j})(c{k})\n"
    if filename == 0:
        filename = f"{G}_{order}_mul_tensor.exp"
    with open(filename,'w') as f:
        f.write(text)
