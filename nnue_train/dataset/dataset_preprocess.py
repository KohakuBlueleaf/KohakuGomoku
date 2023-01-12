from io import TextIOWrapper
from time import time
import numpy as np


SCORE_COEF = 10000
PLY_COEF = 0.25
FACTOR = 1


def calc_target(score, ply, res):
    return (
        int(score)/(SCORE_COEF)
        + (int(ply)**0.5)/-15*int(res) * PLY_COEF
    )*FACTOR


def dataset_loader(
    datas: TextIOWrapper
) -> tuple[np.ndarray, np.ndarray]:
    print()
    train = []
    target = []
    
    total = 0
    while (line := datas.readline()):
        ply, score, res, board = line.split()
        if abs(int(score)) == 31111:
            continue
        train.append(np.fromiter(board, np.int8))
        target.append([calc_target(score, ply, res)])
        total += 1
        
        if (total)%10000 == 0:
            print(total, end='\r')
    
    print(total, end='\r')
    train = np.array(train)
    target = np.array(target).astype(np.float32)
    
    return (
        train,
        target,
    )


def target_loader(datas: TextIOWrapper) -> np.ndarray:
    target = []
    
    total = 0
    while (line := datas.readline()):
        ply, score, res, _ = line.split()
        if abs(int(score)) == 31111:
            continue
        target.append([calc_target(score, ply, res)])
        total += 1
        
        if (total)%10000 == 0:
            print(total, end='\r')
    
    target = np.array(target)
    return target


def main():
    print('Generating dataset... ', end='', flush=True)
    t0 = time()
    with open('data.txt', 'r') as f:
        all_x, all_t = dataset_loader(f)
    t1 = time()
    print(f'done! cost: {t1-t0:.2f}s')
    
    print('Saving... ', end='', flush=True)
    t0 = time()
    np.save('data.npy', all_x)
    np.save('target.npy', all_t)
    t1 = time()
    print(f'done! cost: {t1-t0:.2f}s')
    
    print(all_x.shape, all_t.shape)
    print(np.max(all_t), np.min(all_t))
    print(len(all_t))


if __name__ == '__main__':
    main()