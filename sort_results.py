

f = open("resultados.csv","r")
lines = list(f)
for i in range(len(lines)):
    lines[i] = lines[i].strip('"/andres\n')
lines = sorted(lines)
f.close()
f2 = open("resultados.csv","w")
for line in lines:
    f2.write(line)
    f2.write("\n")
f2.close()