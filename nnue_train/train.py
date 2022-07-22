from math import ceil, log10
from time import time
from pickle import dump

import matplotlib.pyplot as plt
import numpy as np
import torch
import torch.utils.data as Data

from model import NNUE


# setting for training
SEED = 1
DEVICE = 'cuda'
TEST_SIZE = 10000
BATCH = 8192
EPOCH = 300
PERIOD = 5

torch.manual_seed(SEED)


#initialize model, optimizer and loss funciton.
print('Initializing... ', end='', flush = True)
net = NNUE(225, 250, 10).to(DEVICE)
opt = torch.optim.SGD(net.parameters(), lr = 0.0025, momentum=0.9)
loss_func = torch.nn.MSELoss()
print('done!')


def log_weight(net: NNUE):
    (w1, b1), (w2, b2), (w3, b3), (w4, b4) = net.load_param()
    print(
        f'|{np.max(w1):.2f} {np.min(w1):.2f} {np.average(np.abs(w1)):.2f}|'
        f'{np.max(w2):.2f} {np.min(w2):.2f} {np.average(np.abs(w2)):.2f}|'
        f'{np.max(w3):.2f} {np.min(w3):.2f} {np.average(np.abs(w3)):.2f}|'
        f'{np.max(w4):.2f} {np.min(w4):.2f} {np.average(np.abs(w4)):.2f}|'
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
        shuffle = True,
        drop_last = False
    )
    print('done!')
    
    #print information
    print('start training')
    print('='*50)
    print(f'Dataset Size: {len(data)}')
    print(f'Train Size: {len(train_x)}')
    print(f'Batch Size: {BATCH}')
    print(f'EPOCH: {EPOCH}')
    print('='*50)
    
    
    #log information of weights(for quantize)
    log_weight(net)
    loss_t = loss_func(net(test_x.type(torch.float32)), test_t)
    print(
        f'| Start Training!'
        f'| Loss_T: {loss_t: 5.3f}|'
    )
    
    training_loss = []
    target_loss = []
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
            
            total_loss += loss
            total += 1
            if total%100==0:
                log_weight(net)
        
        loss_x = total_loss / total
        loss_t = loss_func(net(test_x.type(torch.float32)), test_t)
        training_loss.append(float(loss_x))
        target_loss.append(float(loss_t))
        t_e = time()
        
        print(
            f'| Epoch: {e+1:>{int(ceil(log10(EPOCH)))+1}}'
            f'| Loss_X: {loss_x:5.3f}| Loss_T: {loss_t: 5.3f}'
            f'| {len(loader)/(t_e-t_s):4.1f} iters/sec'
            f'| {(t_e-t_s):4.1f} sec/Epoch|',
        )
        if (e+1)%PERIOD == 0:
            with open('weight/weights.pypick', 'wb') as f:
                dump(net.load_param(), f)
            print()
    
    print()
    print('='*50)
    
    epoch = np.arange(1, EPOCH+1)
    plt.subplots_adjust(
        left = 0.035, bottom = 0.065,
        right = 0.965, top = 0.935,
    )
    plt.plot(epoch, training_loss)
    plt.plot(epoch, target_loss)
    plt.show()


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print()