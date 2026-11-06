func soma:
  param a
  param b
  t0 = a
  t1 = b
  t2 = t0 + t1
  return t2
endfunc soma
func fatorial:
  param n
  t3 = n
  t4 = t3 <= 1
  ifz t4 goto L0
  return 1
L0:
  t5 = n
  t6 = n
  t7 = t6 - 1
  arg t7
  t8 = call fatorial, 1
  t9 = t5 * t8
  return t9
endfunc fatorial
func mostrar:
  param valor
  t10 = valor
  print t10
  return
endfunc mostrar
func main:
  arg 5
  t11 = call fatorial, 1
  arg 2
  arg t11
  t12 = call soma, 2
  arg t12
  call mostrar, 1
  return 0
endfunc main
