# Teste 3: Lógica e Comparação (SLT)
# Testa: AND, OR, SLL, SLT

li $t0, 12          # Binário: 1100
li $t1, 10          # Binário: 1010

# 1. AND Bit-a-bit
and $t2, $t0, $t1   # 1100 AND 1010 = 1000 (8 decimal)
                    # $t2 deve ser 8

# 2. OR Bit-a-bit
or $t3, $t0, $t1    # 1100 OR 1010 = 1110 (14 decimal)
                    # $t3 deve ser 14

# 3. Shift Left Logic (SLL) - Multiplicar por 2
sll $t4, $t1, 1     # Desloca bits de 10 (1010) p/ esquerda -> 10100 (20)
                    # $t4 deve ser 20

# 4. Set Less Than (SLT) - Comparação
slt $t5, $t1, $t0   # 10 < 12? Sim (Verdadeiro).
                    # $t5 deve ser 1.

# Verifica se $t0 (12) é menor que $t1 (10)
slt $t6, $t0, $t1   # 12 < 10? Não (Falso).
                    # $t6 deve ser 0.

# 5. Encerrar
li $v0, 10
syscall