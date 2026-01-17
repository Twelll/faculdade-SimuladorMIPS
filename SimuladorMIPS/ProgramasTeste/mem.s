# Teste 2: Teste de Memória RAM
# Testa: SW, LW, ADDI

# 1. Preparar dados
li $s0, 100         # $s0 será nosso endereço base de memória (posição 100)
li $t0, 555         # Valor a ser salvo
li $t1, 777         # Outro valor

# 2. Salvar na Memória (Store Word - SW)
sw $t0, 0($s0)      # Salva 555 na posição de memória 100
sw $t1, 4($s0)      # Salva 777 na posição de memória 104 (100 + 4)

# 3. Limpar registradores (para provar que vamos ler da memória depois)
li $t0, 0
li $t1, 0

# 4. Ler da Memória (Load Word - LW)
lw $t2, 0($s0)      # Lê da posição 100. $t2 deve virar 555.
lw $t3, 4($s0)      # Lê da posição 104. $t3 deve virar 777.

# 5. Modificar valor lido
addi $t2, $t2, 1    # $t2 = 555 + 1 = 556

# 6. Encerrar
li $v0, 10
syscall