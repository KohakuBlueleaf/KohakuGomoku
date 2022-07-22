import numpy as np
from dataset_preprocess import target_loader


def main():
    with open('data.txt', 'r') as f:
        all_t = target_loader(f)
    np.save('target.npy', all_t)


if __name__ == '__main__':
    main()