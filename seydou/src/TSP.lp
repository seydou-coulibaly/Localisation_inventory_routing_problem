\ENCODING=ISO-8859-1
\Problem name: TSP

Minimize
 obj: 30.2324329156619 x2 + 30.6104557300279 x3 + 70.178344238091 x4
      + 30.2324329156619 x6 + 60.802960454241 x8 + 100.26464980241 x9
      + 30.6104557300279 x11 + 60.802960454241 x12 + 39.6232255123179 x14
      + 70.178344238091 x16 + 100.26464980241 x17 + 39.6232255123179 x18
Subject To
 c1:  x1 + x2 + x3 + x4  = 1
 c2:  x6 + x7 + x8 + x9  = 1
 c3:  x11 + x12 + x13 + x14  = 1
 c4:  x16 + x17 + x18 + x19  = 1
 c5:  x1 + x6 + x11 + x16  = 1
 c6:  x2 + x7 + x12 + x17  = 1
 c7:  x3 + x8 + x13 + x18  = 1
 c8:  x4 + x9 + x14 + x19  = 1
 c9:  x20  = 1
 c10: 3 x2 + x5 - x10 <= 2
 c11: 3 x3 + x5 - x15 <= 2
 c12: - x5 + 3 x6 + x10 <= 2
 c13: 3 x8 + x10 - x15 <= 2
 c14: - x5 + 3 x11 + x15 <= 2
 c15: - x10 + 3 x12 + x15 <= 2
 c16: x1  = 0
 c17: x7  = 0
 c18: x13  = 0
 c19: x19  = 0
Bounds
 0 <= x1 <= 1
 0 <= x2 <= 1
 0 <= x3 <= 1
 0 <= x4 <= 1
 2 <= x5 <= 4
 0 <= x6 <= 1
 0 <= x7 <= 1
 0 <= x8 <= 1
 0 <= x9 <= 1
 2 <= x10 <= 4
 0 <= x11 <= 1
 0 <= x12 <= 1
 0 <= x13 <= 1
 0 <= x14 <= 1
 2 <= x15 <= 4
 0 <= x16 <= 1
 0 <= x17 <= 1
 0 <= x18 <= 1
 0 <= x19 <= 1
 1 <= x20 <= 4
Binaries
 x1  x2  x3  x4  x6  x7  x8  x9  x11  x12  x13  x14  x16  x17  x18  x19 
Generals
 x5  x10  x15  x20 
End
