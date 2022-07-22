def count_lines(fname):
    '''
    fast way to count line in a file.
    this function is used for check the size of the dataset
    which is generated from the selfplay. 
    '''
    def _make_gen(reader):
        while True:
            b = reader(2 ** 16)
            if not b: break
            yield b
    with open(fname, "rb") as f:
        count = sum(buf.count(b"\n") for buf in _make_gen(f.raw.read))
    return count