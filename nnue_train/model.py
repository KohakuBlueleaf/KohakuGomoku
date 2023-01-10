import torch
import torch.nn as nn


class NNUE(nn.Module):
    def __init__(self, intput_dim=225, halfkp=256, hidden=16):
        super(NNUE, self).__init__()
        self.input_dim = intput_dim
        self.halfkp = halfkp
        self.linear1 = nn.Linear(self.input_dim, self.halfkp)
        self.linear2 = nn.Linear(self.halfkp*2, hidden)
        self.linear3 = nn.Linear(hidden, hidden)
        self.linear4 = nn.Linear(hidden, 1)

    def forward(self, x):
        x1, x2 = torch.chunk(x, 2, dim=1)
        y11 = torch.clamp(self.linear1(x1), 0, 1)
        y12 = torch.clamp(self.linear1(x2), 0, 1)
        y1 = torch.cat((y11, y12), 1)
        
        y2 = torch.clamp(self.linear2(y1), 0, 1)
        y3 = torch.clamp(self.linear3(y2), 0, 1)
        y4 = self.linear4(y3)
        return y4
    
    def load_param(self):
        w1, b1 = (i.cpu().detach().numpy() for i in self.linear1.parameters())
        w2, b2 = (i.cpu().detach().numpy() for i in self.linear2.parameters())
        w3, b3 = (i.cpu().detach().numpy() for i in self.linear3.parameters())
        w4, b4 = (i.cpu().detach().numpy() for i in self.linear4.parameters())
        
        return ((w1, b1), (w2, b2), (w3, b3), (w4, b4))


class EMA:
    def __init__(self, model: nn.Module, decay: float = 0.9999):
        self.model = model
        self.decay = decay
        self.step = 0
        self.shadow = {}
        self.backup = {}

    def register(self):
        for name, param in self.model.named_parameters():
            if param.requires_grad:
                self.shadow[name] = param.data.clone()

    def update(self):
        decay = min(self.decay, (self.step+1)/(self.step+10))
        for name, param in self.model.named_parameters():
            if param.requires_grad:
                assert name in self.shadow
                new_average = (1.0 - decay) * param.data + decay * self.shadow[name]
                self.shadow[name] = new_average.clone()
        self.step += 1

    def apply_shadow(self):
        for name, param in self.model.named_parameters():
            if param.requires_grad:
                assert name in self.shadow
                self.backup[name] = param.data
                param.data = self.shadow[name]

    def restore(self):
        for name, param in self.model.named_parameters():
            if param.requires_grad:
                assert name in self.backup
                param.data = self.backup[name]
        self.backup = {}

if __name__ == '__main__':
    mynet = NNUE(225, 50)