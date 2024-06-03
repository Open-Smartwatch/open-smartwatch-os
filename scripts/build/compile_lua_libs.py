import "os"


print("Compiling lua bytecode with luac ...")
os.system("cd src.lua/libs/; for i in $(ls *.lua); do print $i; ../build/luac -s -o ../data/lua/libs/$i ; done ")
