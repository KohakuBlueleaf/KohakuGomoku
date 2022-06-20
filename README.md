# KohakuGomoku

## Requirments
Compiler: should support c++14.<br>
Other: for `make merge`, see [this](https://www.npmjs.com/package/cpp-merge).<br>
System: Been tested on Windows, MacOS, most of Linux distribution.
(Tested under the lastest version at 2022/06)

## How to run
First clone this repo and go into it.
```bash
git clone https://github.com/KohakuBlueleaf/KohakuGomoku.git
cd KohakuGomoku
```

And then run make command.
```bash
make all
```

If you only want to build some specific policy, use this.
```bash
make main #necessary!!!
make <POLICY>
```
for example
```bash
make alphabeta #will create player_alphabeta.exe at build/ folder
```

Finally
```bash
cd build
./main ./<POLICY> ./<POLICY>
```

### Merge to one file
**Make sure you have installed cpp-merge**

If you want to merge all cpp/hpp file into one file(For a make target).
You can use
```bash
make merge
```
to build all merge file.

And if you want to only merge specific target, use below command.
```bash
make merge_<target>
```
