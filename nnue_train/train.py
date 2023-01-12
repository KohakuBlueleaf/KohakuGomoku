print('Initializing... ', end='', flush = True)
from math import ceil, log10
from time import time
from pickle import dump

import matplotlib.pyplot as plt
import numpy as np
import torch
import torch.utils.data as Data

from model import NNUE, EMA


# setting for training
SEED = 1
DEVICE = 'cuda'
TEST_SIZE = 50000
BATCH = 128
EPOCH = 15
PERIOD = 1

torch.manual_seed(SEED)


#initialize model, optimizer and loss funciton.
net = NNUE(121, 64, 16).to(DEVICE)
ema = EMA(net, 0.9999)
ema.register()
opt = torch.optim.AdamW(net.parameters(), lr = 2e-3)
lr_sch = torch.optim.lr_scheduler.StepLR(opt, 1, 0.9)
loss_func = torch.nn.functional.mse_loss
print('done!')


def log_weight(net: NNUE, loss, step):
    (w1, b1), (w2, b2), (w3, b3), (w4, b4) = net.load_param()
    print(
        f'|Step: {step:07}|Loss: {loss:7.5f}'
        f'|{np.max(w1):.2f} {np.min(w1):.2f} {np.average(np.abs(w1)):.2f}|'
        f'{np.max(w2):.2f} {np.min(w2):.2f} {np.average(np.abs(w2)):.2f}|'
        f'{np.max(w3):.2f} {np.min(w3):.2f} {np.average(np.abs(w3)):.2f}|'
        f'{np.max(w4):.2f} {np.min(w4):.2f} {np.average(np.abs(w4)):.2f}|',
        end='\r',
        flush=True
    )


def main():
    #loading dataset from npy file.
    print('dataset loading... ', end='', flush = True)
    data = torch.from_numpy(np.load('dataset/data.npy'))
    target = torch.from_numpy(np.load('dataset/target.npy'))
    data = data.to(DEVICE)
    target = target.type(torch.float32).to(DEVICE)
    print('done!')
    
    #split the dataset for train and validate
    print('building dataset... ', end='', flush = True)
    train_x, test_x = data[:-TEST_SIZE], data[-TEST_SIZE:]
    train_t, test_t = target[:-TEST_SIZE], target[-TEST_SIZE:]
    
    dataset = Data.TensorDataset(
        train_x, train_t,
    )
    loader = Data.DataLoader(
        dataset = dataset,
        batch_size = BATCH,
        shuffle = True, drop_last = True
    )
    print('done!')
    
    #print information
    print('start training')
    print('='*50)
    print(f'Dataset Size: {len(data)}')
    print(f'Train Size: {len(train_x)}')
    print(f'Batch Size: {BATCH}')
    print(f'Epoch iters: {len(train_x)//BATCH}')
    print(f'EPOCH: {EPOCH}')
    print('='*50)
    
    
    #log information of weights(for quantize)
    loss_t = loss_func(net(test_x.type(torch.float32)), test_t)
    print(
        f'| Start Training!'
        f'| Loss_T: {loss_t: 5.3f}|'
    )
    
    ema_loss = 0
    step = 0
    for e in range(EPOCH):
        t_s = time()
        total_loss = 0
        total = 0
        for batch_x, batch_t in loader:
            opt.zero_grad()
            out = net(batch_x.type(torch.float32))
            loss = loss_func(out, batch_t)
            
            loss.backward()
            opt.step()
            ema.update()
            
            total_loss += loss
            total += 1
            
            if ema_loss==0:
                ema_loss = loss
            else:
                decay = min(0.9999, (step+1)/(step+10))
                ema_loss = ema_loss*decay + loss*(1-decay)
            
            step += 1
            if total%100==0:
                ema.apply_shadow()
                log_weight(net, ema_loss, step)
                ema.restore()
        lr_sch.step()
        
        ema.apply_shadow()
        loss_x = total_loss / total
        loss_t = loss_func(net(test_x.type(torch.float32)), test_t)
        ema.restore()
        t_e = time()
        print()
        print(
            f'| Epoch: {e+1:>{int(ceil(log10(EPOCH)))+1}}'
            f'| Loss_X: {loss_x:6.4f}| Loss_T: {loss_t: 6.4f}'
            f'| {len(loader)/(t_e-t_s):4.1f} iters/sec'
            f'| {(t_e-t_s):4.1f} sec/Epoch|'
            +(' '*30)
        )
        if (e+1)%PERIOD == 0:
            with open(f'weight/weights_{step}step.pypick', 'wb') as f:
                ema.apply_shadow()
                dump(net.load_param(), f)
                ema.restore()
    
    print()
    print('='*50)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print()