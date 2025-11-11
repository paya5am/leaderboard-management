stop = 100
prime = True

for n in range(2,stop+1):
    

    for  x in range( 2, n):
        
        if (n%x == 0): prime = False
            
    if prime: print(n)
    prime = True

