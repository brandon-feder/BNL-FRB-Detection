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
                
                # outdo.append(vert)
                # outdo.append(vert)
                
                outdo.append(vert)
                outdo.append(diag)

                # outdo.append(diag)
                # outdo.append(diag)

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
                Nf, Nt = S.shape

                if isFirst:
                    vert = S[0:Nf:2] + S[1:Nf+1:2]
                    self.V.append(vert)

                    diag = np.array ([
                            S[ 2*i, i : Nt-Nf//2+i-1] + S[ 2*i+1, i+1 : Nt-Nf//2+i] 
                        for i in range(Nf//2)
                    ])

                    self.D.append(diag)

                else:
                    vert = self.V[id][0:Nf, offset:self.V[id].shape[1]]
                    nVert = S[0:Nf:2, Nt-offset:Nt] + S[1:Nf+1:2, Nt-offset:Nt]
                    self.V[id] = np.concatenate((vert, nVert), 1)

                    Dl = self.D[id].shape[1]
                    l = Dl - offset
                    
                    
                    if l < 0:
                        self.D[id] = np.array ([
                            S[ 2*i, i : Nt-Nf//2+i-1] + S[ 2*i+1, i+1 : Nt-Nf//2+i] 
                            for i in range(Nf//2)
                        ])

                    else:  
                        print("================")
                        # print("    Case 2")

                        diag = self.D[id][0:Nf, offset:l+offset]
                        nDiag = np.ndarray((Nf//2, offset))

                        print(Nt-Nf//2-1)
                        for y in range(Nf//2):
                            print((y, Nt-Nf//2+y-1), end=", ")
                            realX = 0
                            for x in range(y+diag.shape[1], Nt-Nf//2+y-1):
                                print(y, x)
                                temp = S[y][x] + S[y+1][x+1] 
                                nDiag[y][realX] = temp
                                realX += 1
                        
                        print()




                        self.D[id] = np.concatenate((diag, nDiag), 1)

                outdo.append(self.V[id])
                outdo.append(self.D[id])

                # outdo.append(self.V[id])
                # outdo.append(self.V[id])

                # outdo.append(self.D[id])
                # outdo.append(self.D[id])



                # print(id, S.shape, self.V[id].shape, self.D[id].shape)

                id += 1

            todo = outdo

        i = 0
        for S in todo:
            data[i][0:S.shape[1]] = S
            data[i][S.shape[1]:data.shape[1]] = 0
            i += 1
        
        return data