import numpy as np
from matplotlib import pyplot as plt
import numba as nb
import math

class LineDetection:
    #params are self, and the data dimensions
    def __init__(self, dim):
        # Make sure parameters are valid
        assert math.log(dim, 2)//1 == math.log(dim, 2), "Dimensions are not a power of 2"

        # Setup public variables
        self.dim = dim
        self.V = []
        self.D = []
    
    def NaiveDetect(self, data):
        todo = [data]
        for _ in range( int(math.log(self.dim, 2))):
            outdo = []
            for S in todo:
                Nf, Nt = S.shape

                vert = S[0:Nf:2] + S[1:Nf+1:2]
                
                diag = np.array ([
                        S[2*i,i:-Nf//2+i-1] + 
                        S[2*i+1,i+1:-Nf//2+i] 
                    for i in range(Nf//2)
                ])                
                
                outdo.append(vert)
                outdo.append(diag)

            todo = outdo

        i = 0
        for S in todo:
            data[i][0:S.shape[1]] = S
            data[i][S.shape[1]:data.shape[1]] = 0
            i += 1
        
        return data

    def Detect(self, data, offset=1):  
        isFirst = len(self.V) == 0

        todo = [data]
        id = 0
        for _ in range( int(math.log(self.dim, 2))):
            outdo = []
            for S in todo:
                Nf = S.shape[0]

                if isFirst:
                    vert = S[0:Nf:2] + S[1:Nf+1:2]
                    self.V.append(vert)
                else:
                    vert = self.V[id][0:Nf, offset:self.V[id].shape[1]]
                    nVert = S[0:Nf:2, len(S)-offset:len(S)] + S[1:Nf+1:2, len(S)-offset:len(S)]
                    self.V[id] = np.concatenate((vert, nVert), 1)

                diag = np.array ([
                        S[2*i,i:-Nf//2+i-1] + S[2*i+1,i+1:-Nf//2+i] 
                    for i in range(Nf//2)
                ])                
                
                outdo.append(self.V[id])
                outdo.append(diag)

                id += 1

            todo = outdo

        i = 0
        for S in todo:
            data[i][0:S.shape[1]] = S
            data[i][S.shape[1]:data.shape[1]] = 0
            i += 1
        
        return data