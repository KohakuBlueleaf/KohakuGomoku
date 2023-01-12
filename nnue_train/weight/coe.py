import numpy as np
import pickle


COE_TEMPLATE = '''; shape: {} depth x {} width
memory_initialization_radix = 16;
memory_initialization_vector = 
{};
'''


def load_weight(file):
    with open(file, 'rb') as f:
        (w1, b1), (w2, b2), (w3, b3), (w4, b4) = pickle.load(f)
    
    w1 = (w1*127).T.astype(np.int16)
    b1 = (b1*127).astype(np.int16)
    
    w2 = (w2*64).T.astype(np.int8)
    b2 = (b2*127*64).astype(np.int16)
    
    w3 = (w3*64).T.astype(np.int8)
    b3 = (b3*127*64).astype(np.int16)
    
    w4 = (w4*64).T.astype(np.int8)
    b4 = (b4*127*64).astype(np.int16)
    
    return (w1, b1), (w2, b2), (w3, b3), (w4, b4)


def bytes_str(x: np.ndarray):
    return ''.join([int.to_bytes(int(i), (x.dtype==np.int16)+1, 'big', signed=True).hex() for i in x.flatten()])


def main():
    (w1, b1), (w2, b2), (w3, b3), (w4, b4) = load_weight('./weights_mse-0.0550-0.0637.pypick')
    
    w1_bytes = ''
    w1_bytes += int.to_bytes(1, 1, 'big').hex()
    w1_bytes += bytes_str(b1)
    w1_bytes += ',\n'
    for i in range(121):
        w1_bytes += int.to_bytes(i+2, 1, 'big').hex()
        w1_bytes += bytes_str(w1[i, :])
        w1_bytes += ',\n'
    w1_coe = COE_TEMPLATE.format(121+1, 8+w1.shape[1]*16, w1_bytes[:-2])
    with open('./w1.coe', 'w') as f:
        f.write(w1_coe)
    
    
    w2_bytes = ''
    w2 = np.split(w2, w2.shape[0]//16, 0)
    for i in range(len(w2)):
        w2_bytes += int.to_bytes(i+1, 1, 'big').hex()
        w2_bytes += bytes_str(w2[i])
        w2_bytes += ',\n'
    w2_coe = COE_TEMPLATE.format(len(w2), 8+w2[0].size*8, w2_bytes[:-2])
    b2_coe = COE_TEMPLATE.format(1, 8+b2.size*16, f'01{bytes_str(b2)}')
    with open('./w2.coe', 'w') as f:
        f.write(w2_coe)
    with open('./b2.coe', 'w') as f:
        f.write(b2_coe)
    
    
    w3_coe = COE_TEMPLATE.format(1, 8+w3.size*8, f'01{bytes_str(w3)}')
    b3_coe = COE_TEMPLATE.format(1, 8+b3.size*16, f'01{bytes_str(b3)}')
    with open('./w3.coe', 'w') as f:
        f.write(w3_coe)
    with open('./b3.coe', 'w') as f:
        f.write(b3_coe)
    
    
    x_4 = np.array([[36, -127, 9, 99, 13, -115, 101, 18, 1, 13, 118, 61, -19, -116, -7, -58]])
    print(x_4.dot(w4)+b4)
    w4_coe = COE_TEMPLATE.format(1, 8+w4.size*8, f'01{bytes_str(w4)}')
    b4_coe = COE_TEMPLATE.format(1, 8+b4.size*16, f'01{bytes_str(b4)}')
    with open('./w4.coe', 'w') as f:
        f.write(w4_coe)
    with open('./b4.coe', 'w') as f:
        f.write(b4_coe)


if __name__ == '__main__':
    main()