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
                
                outdo.append(self.__calcVert(S, 0, Nt))
                outdo.append(
                    self.__calcDiag(S, 
                        lambda i: i,
                        lambda i: Nt-Nf//2+i-1
                    )
                )

            todo = outdo
        
        return self.__todoToData(todo, data)
        
    def Detect(self, data, offset=1):  
        isFirst = len(self.V) == 0

        todo = [data]
        id = 0
        for _ in range( int(math.log(self.dim, 2))):
            outdo = []
            for S in todo:
                Nf, Nt = S.shape

                if isFirst:
                    self.V.append( self.__calcVert(S, 0, Nt) )

                    self.D.append(
                        self.__calcDiag(S, 
                            lambda i: i,
                            lambda i: Nt-Nf//2+i-1
                        )
                    )

                else:
                    self.V[id] = np.concatenate(
                        (
                            self.V[id][0:Nf, offset:self.V[id].shape[1]], 
                            self.__calcVert(S, Nt-offset, Nt)
                        ), 
                    1)

                    Dl = self.D[id].shape[1]
                    l = Dl - offset
                    
                    if l < 0:
                        self.D[id] = self.__calcDiag(S, 
                            lambda i: i,
                            lambda i: Nt-Nf//2+i-1
                        )  

                    else: 
                        self.D[id] = np.concatenate((
                            self.D[id][0:Nf, offset:Dl], 
                            self.__calcDiag(S, 
                                lambda i: i+l,
                                lambda i: Nt-Nf//2+i-1
                            )   
                        ), 1)

                outdo.append(self.V[id])
                outdo.append(self.D[id])

                id += 1

            todo = outdo
        
        return self.__todoToData(todo, data)
    
    def __calcDiag(self, S, s, e):
        return np.array ([
            S[ 2*i, s(i) : e(i)] + S[ 2*i+1, s(i)+1 : e(i)+1] for i in range(S.shape[0]//2)
        ])

    def __calcVert(self, S, s, e):
        return S[0:S.shape[0]:2, s:e] + S[1:S.shape[0]+1:2, s:e]
    
    def __todoToData(self, todo, data):
        i = 0
        for S in todo:
            data[i][0:S.shape[1]] = S
            data[i][S.shape[1]:data.shape[1]] = 0
            i += 1
        
        return data
