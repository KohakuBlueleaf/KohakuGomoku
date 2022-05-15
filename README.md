# KohakuGomoku

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