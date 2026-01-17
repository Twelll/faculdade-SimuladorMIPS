# Projeto: Simulador MIPS

**Disciplina:** Arquitetura e Organização de Computadores (UTFPR)

## Explicação do Projeto

O Simulador MIPS é uma ferramenta desenvolvida para auxiliar estudantes e profissionais a entenderem o funcionamento de processadores da arquitetura MIPS. O projeto permite carregar e executar códigos Assembly, visualizar o estado dos registradores, acompanhar a execução passo a passo e testar instruções de memória, aritmética, lógica e sistema. O objetivo é fornecer um ambiente visual e interativo para aprendizado de arquitetura de computadores.

## Autores

* Thiago Wesley Pasquim de Melo
* Vinicius Gabriel Giordani Barbosa

## Ambiente de Desenvolvimento

* **Sistema Operacional:** Linux (Ubuntu recomendado)
* **Linguagem:** C++
* **Framework:** Qt 6 (Widgets)
* **Sistema de Build:** CMake
* **IDE Recomendada:** Qt Creator

---

## Instalando o Qt Creator no Ubuntu (caso necessário)

1. Abra o terminal (Ctrl + Alt + T).
2. Atualize os repositórios:

   ```bash
   sudo apt update
   ```
3. Instale o Qt Creator:

   ```bash
   sudo apt install qtcreator
   ```
4. Instale o compilador e ferramentas de build essenciais:

   ```bash
   sudo apt install build-essential
   ```
5. (Opcional, recomendado) Instale pacotes Qt 6 necessários:

   ```bash
   sudo apt install qt6-base-dev qt6-tools-dev qt6-tools-dev-tools
   ```

Após isso, o Qt Creator estará pronto para abrir e compilar o projeto.

---

## Instalando o CMake no Ubuntu (caso necessário)

1. Abra o terminal (Ctrl + Alt + T).
2. Atualize os repositórios:

   ```bash
   sudo apt update
   ```
3. Instale o CMake:

   ```bash
   sudo apt install cmake
   ```
4. Verifique a instalação:

   ```bash
   cmake --version
   ```

Se aparecer uma versão como "cmake version x(atual)", o CMake está instalado corretamente.

---

## Como Compilar e Executar

### Opção 1: Usando o Qt Creator (Recomendado)

1. Abra o Qt Creator.
2. Vá em `File -> Open File or Project`.
3. Selecione o arquivo `CMakeLists.txt` dentro da pasta do projeto.
4. Quando solicitado para configurar o kit (Configure Project), selecione o Kit Desktop disponível (ex: Desktop Qt 6.x.x GCC 64bit) e clique em `Configure`.
5. Clique no botão de `Play` (seta verde) no canto inferior esquerdo ou pressione `Ctrl+R`.
6. O projeto será compilado e o simulador abrirá automaticamente.

### Opção 2: Via Terminal (Linha de Comando no Linux)

Certifique-se de ter o compilador g++, o CMake e as bibliotecas do Qt instalados.

1. Abra o terminal na pasta do projeto.
2. Crie uma pasta de build e entre nela:

   ```bash
   mkdir build
   cd build
   ```
3. Execute o CMake e depois o Make:

   ```bash
   cmake ..
   make
   ```
4. Execute o simulador gerado:

   ```bash
   ./SimuladorMips
   ```

---

## Como Utilizar o Simulador

### 1. Carregar Código

* Digite o código Assembly diretamente na área de texto à esquerda.
* Ou clique em "Abrir Arquivo" para carregar um `.asm`, `.txt` ou `.s`.
* Linhas como `.data`, `.text` e labels (ex: `main:`) são ignoradas.

### 2. Execução

* **Passo a Passo:** Executa uma instrução por vez.
* **Executar Tudo:** Roda o programa até o fim ou até encontrar `syscall 10`.
* **Resetar:** Reinicia registradores, memória e PC (Program Counter).

### 3. Visualização

* A instrução atual e seu binário aparecem no topo da interface.
* A tabela à direita exibe valores dos registradores ($t0, $s0, etc.) em Decimal e Hexadecimal.
* A área "Saída (Log/Syscall)" mostra prints e mensagens do sistema.

### 4. Instruções Suportadas

* **Aritmética:** ADD, SUB, ADDI, MULT
* **Lógica:** AND, OR, SLL, SLT
* **Memória:** LW, SW, LUI, LI
* **Sistema:** SYSCALL (Suporte para $v0=1 print_int e $v0=10 exit)
