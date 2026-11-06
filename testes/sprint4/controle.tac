func main:
  i = 0
  total = 0
  i = 0
L0:
  t0 = i
  t1 = t0 < 3
  ifz t1 goto L1
  t2 = total
  t3 = i
  t4 = t2 + t3
  total = t4
  t5 = i
  t6 = t5 + 1
  i = t6
  goto L0
L1:
L2:
  t7 = i
  t8 = t7 > 1
  ifz t8 goto L3
  t9 = i
  t10 = t9 - 1
  i = t10
  goto L2
L3:
L4:
  t11 = total
  t12 = t11 + 1
  total = t12
  t13 = total
  t14 = t13 < 5
  ifnz t14 goto L4
  t15 = i
  t16 = t15 == 1
  ifz t16 goto L5
  t17 = total
  print t17
  goto L6
L5:
  print 0
L6:
  return 0
endfunc main
