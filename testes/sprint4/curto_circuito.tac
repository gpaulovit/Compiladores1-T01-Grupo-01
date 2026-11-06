func efeito:
  print 99
  return 1
endfunc efeito
func main:
  x = 1
  ifz 0 goto L0
  t1 = call efeito, 0
  ifz t1 goto L0
  t0 = 1
  goto L1
L0:
  t0 = 0
L1:
  print t0
  ifnz 1 goto L2
  t3 = call efeito, 0
  ifnz t3 goto L2
  t2 = 0
  goto L3
L2:
  t2 = 1
L3:
  print t2
  t4 = x
  t5 = x
  t6 = t5 + 1
  x = t6
  t7 = t4 + t5
  print t7
  t8 = x
  print t8
  return 0
endfunc main
