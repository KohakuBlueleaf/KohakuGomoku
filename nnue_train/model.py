import torch


class ScaledClippedRelu(torch.nn.Module):
    def __init__(self, min_val, max_val, factor):
        super(ScaledClippedRelu, self).__init__()
        self.min_bound = min_val
        self.max_bound = max_val
        self.denomiator = factor

    def forward(self, x):
        divid_y = x // self.denomiator
        clipped_y1 = torch.max(divid_y, torch.ones_like(divid_y) * self.min_bound)
        clipped_y2 = torch.min(clipped_y1, torch.ones_like(clipped_y1) * self.max_bound)
        return clipped_y2


class ClippedRelu(torch.nn.Module):
    def __init__(self, min_val, max_val):
        super(ClippedRelu, self).__init__()
        self.min_bound = min_val
        self.max_bound = max_val

    def forward(self, x):
        clipped_y1 = torch.max(x, torch.ones_like(x)*self.min_bound)
        clipped_y2 = torch.min(clipped_y1, torch.ones_like(clipped_y1)*self.max_bound)
        return clipped_y2


class NNUE(torch.nn.Module):
    def __init__(self, intput_dim, halfkp, hidden=16):
        super(NNUE, self).__init__()
        self.input_dim = intput_dim
        self.halfkp = halfkp
        self.linear1 = torch.nn.Linear(self.input_dim, self.halfkp, True)
        self.linear2 = torch.nn.Linear(self.halfkp*2, hidden, True)
        self.linear3 = torch.nn.Linear(hidden, hidden, True)
        self.linear4 = torch.nn.Linear(hidden, 1, True)
        self.clipped_relu = ClippedRelu(0, 1)

    def forward(self, x):
        x1 = x[:, 0:self.input_dim]                     # first half
        x2 = x[:, self.input_dim:(self.input_dim * 2)]  # second half
        y11 = self.clipped_relu.forward(self.linear1(x1))
        y12 = self.clipped_relu.forward(self.linear1(x2))
        y1 = torch.cat((y11, y12), 1)
        
        y2 = self.clipped_relu.forward(self.linear2(y1))
        y3 = self.clipped_relu.forward(self.linear3(y2))
        y4 = self.linear4(y3)
        return y4
    
    def load_param(self):
        w1, b1 = (i.cpu().detach().numpy() for i in self.linear1.parameters())
        w2, b2 = (i.cpu().detach().numpy() for i in self.linear2.parameters())
        w3, b3 = (i.cpu().detach().numpy() for i in self.linear3.parameters())
        w4, b4 = (i.cpu().detach().numpy() for i in self.linear4.parameters())
        
        return ((w1, b1), (w2, b2), (w3, b3), (w4, b4))


if __name__ == '__main__':
    mynet = NNUE(225, 50)