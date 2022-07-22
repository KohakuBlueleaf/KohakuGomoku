from typing import Union
from pickle import load

import numpy as np


Array = Union[
    list[int],
    list[float],
    list['Array'],
]


SCALE_A = 2**12
SCALE_W = 2**11
SCALE_F = 2**8
TEMPLATE = """#pragma once
const {}

const {}
"""


def quantize(x: np.ndarray, factor: int) -> np.ndarray:
    '''Quantize to int32'''
    return (x*factor).astype(np.int32)


def listlize(x: np.ndarray) -> Array:
    '''multi dimension ndarray to multi dimension list'''
    y: Array = []
    for v in x:
        if isinstance(v, np.ndarray):
            y.append(listlize(v))
        else:
            y.append(v)
    return y


def to_c_array(
        x: np.ndarray, 
        var_name: str = 'x', 
        dtype: str = 'int',
    ) -> str:
    shape = x.shape
    shape_s = ''.join(f'[{i}]' for i in shape)
    output_str = str(listlize(x)).replace('[', '{').replace(']', '}')
    output_str = output_str.replace('}, {', '},\n{')
    
    return f'{dtype} {var_name}{shape_s} = {{\n{output_str[1:-1]}\n}};'


def main():
    with open('weight/weights.pypick', 'rb') as f:
        (w1, b1), (w2, b2), (w3, b3), (w4, b4) = load(f)
    
    w1_q = quantize(w1.T, SCALE_A)
    b1_q = quantize(b1, SCALE_A).reshape(1, -1)
    w2_q = quantize(w2, SCALE_W)
    b2_q = quantize(b2, SCALE_W).reshape(1, -1)
    w3_q = quantize(w3, SCALE_W)
    b3_q = quantize(b3, SCALE_W).reshape(1, -1)
    w4_q = quantize(w4, SCALE_F)
    b4_q = quantize(b4, SCALE_F).reshape(1, -1)
    
    linears = [
        TEMPLATE.format(to_c_array(w1_q, "w1"), to_c_array(b1_q, "b1")),
        TEMPLATE.format(to_c_array(w2_q, "w2"), to_c_array(b2_q, "b2")),
        TEMPLATE.format(to_c_array(w3_q, "w3"), to_c_array(b3_q, "b3")),
        TEMPLATE.format(to_c_array(w4_q, "w4"), to_c_array(b4_q, "b4")),
    ]
    
    for i, v in enumerate(linears):
        with open(f'weight/linear{i+1}.hpp', 'w', encoding='utf-8') as f:
            f.write(v)


if __name__ == '__main__':
    main()