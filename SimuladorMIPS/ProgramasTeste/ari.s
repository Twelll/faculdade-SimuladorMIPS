# Teste 1: Calculadora Simples
# Testa: LI, ADD, SUB, MULT, SYSCALL

# 1. Carregar valores (LI)
li $t1, 10          
li $t2, 5           

# 2. Soma (ADD) 
add $t3, $t1, $t2   # $t3 = 15

# 3. Subtração (SUB) 
sub $t4, $t1, $t2   # $t4 = 5

# 4. Multiplicação (MULT) 
mult $t1, $t2       
                  

# 5. Imprimir o resultado da Soma ($t3) na tela
li $v0, 1          
add $a0, $t3, $zero
syscall            

# 6. Encerrar
li $v0, 10          
syscall