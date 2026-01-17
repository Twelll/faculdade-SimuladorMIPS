/**
 * @file mainwindow.cpp
 * @author Thiago Wesley Pasquim de Melo
 * @author Vinicius Gabriel Giordani Barbosa
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabelaRegs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->btnArquivo, &QPushButton::clicked, this, &MainWindow::abrirArquivo);
    connect(ui->btnPasso, &QPushButton::clicked, this, &MainWindow::executarPasso);
    connect(ui->btnExecutar, &QPushButton::clicked, this, &MainWindow::executarTudo);
    connect(ui->btnResetar, &QPushButton::clicked, this, &MainWindow::reiniciar);

    inicializarMapaRegs();
    reiniciar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Configuração Inicial
void MainWindow::inicializarMapaRegs() {
    QStringList nomes = {
        "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
        "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
        "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
    };
    for(int i=0; i < nomes.size(); i++) {
        mapaNomesReg[nomes[i]] = i;
    }
}

int MainWindow::getIndiceReg(QString nome) {
    return mapaNomesReg.value(nome, -1);
}

void MainWindow::reiniciar() {
    pc = 0;
    hi = 0;
    lo = 0;

    // Zera registradores e memória
    for(int i=0; i<32; i++) registradores[i] = 0;
    memoriaDados.clear();
    memoriaInstrucoes.clear();

    atualizarInterface();
    log("--- Simulador Reiniciado ---");
    ui->lblInstrAtual->setText("Aguardando...");
    ui->lblBinario->setText("Binário: --");
}

//Leitura e Execução
void MainWindow::carregarCodigo() {
    if (!memoriaInstrucoes.empty() && pc > 0) return;

    memoriaInstrucoes.clear();
    QStringList linhas = ui->txtCodigo->toPlainText().split('\n');

    for (QString linha : linhas) {
        linha = linha.trimmed();
        if (linha.isEmpty() || linha.startsWith("#") || linha.startsWith(".")) continue;
        if (linha.contains("#")) linha = linha.split("#")[0].trimmed();

        // Remove labels
        if (linha.endsWith(":")) continue;
        if (linha.contains(":")) linha = linha.split(":")[1].trimmed();

        QString linhaLimpa = linha.replace(",", " ");
        QStringList partes = linhaLimpa.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        if (partes.isEmpty()) continue;

        Instrucao novaInstr;
        novaInstr.textoOriginal = linha;
        novaInstr.opcode = partes[0].toUpper();

        for (int i = 1; i < partes.size(); ++i) {
            novaInstr.argumentos.append(partes[i]);
        }

        memoriaInstrucoes.push_back(novaInstr);
    }

    if (!memoriaInstrucoes.empty()) {
        log("Código carregado. Total de instruções: " + QString::number(memoriaInstrucoes.size()));
    }
}

void MainWindow::executarPasso() {
    if (memoriaInstrucoes.empty()) carregarCodigo();

    if (pc >= (int)memoriaInstrucoes.size()) {
        QMessageBox::information(this, "Fim", "Execução finalizada.");
        return;
    }

    Instrucao atual = memoriaInstrucoes[pc];

    // Atualiza visual com o binário CORRIGIDO
    ui->lblInstrAtual->setText("Instr: " + atual.textoOriginal);
    ui->lblBinario->setText("Bin: " + gerarBinario(atual));

    bool parar = processarInstrucao(atual);
    atualizarInterface();

    if (parar) {
        // Gera relatório se parou por exit
        gerarRelatorio();
    } else {
        pc++;
    }
}

void MainWindow::executarTudo() {
    if (memoriaInstrucoes.empty()) carregarCodigo();

    int limiteSeguranca = 5000;
    int ciclos = 0;

    while (pc < (int)memoriaInstrucoes.size() && ciclos < limiteSeguranca) {
        if (processarInstrucao(memoriaInstrucoes[pc])) {
            break;
        }
        pc++;
        ciclos++;
    }

    atualizarInterface();
    log("Execução completa.");
    gerarRelatorio(); // Gera relatório ao final incondicionalmente
}


// --- Processamento
bool MainWindow::processarInstrucao(Instrucao instr) {
    QString op = instr.opcode;
    QStringList args = instr.argumentos;

    registradores[0] = 0; // $zero sempre 0

    try {
        // Aritmética
        if (op == "ADD") {
            int rd = getIndiceReg(args[0]);
            int rs = getIndiceReg(args[1]);
            int rt = getIndiceReg(args[2]);
            registradores[rd] = registradores[rs] + registradores[rt];
        }
        else if (op == "SUB") {
            registradores[getIndiceReg(args[0])] = registradores[getIndiceReg(args[1])] - registradores[getIndiceReg(args[2])];
        }
        else if (op == "ADDI") {
            registradores[getIndiceReg(args[0])] = registradores[getIndiceReg(args[1])] + args[2].toInt();
        }
        else if (op == "LI") {
            int reg = getIndiceReg(args[0]);
            if (reg > 0) registradores[reg] = args[1].toInt();
        }
        else if (op == "MULT") {
            long long res = (long long)registradores[getIndiceReg(args[0])] * registradores[getIndiceReg(args[1])];
            lo = res & 0xFFFFFFFF;
            hi = (res >> 32) & 0xFFFFFFFF;
        }
        // Lógica
        else if (op == "AND") {
            registradores[getIndiceReg(args[0])] = registradores[getIndiceReg(args[1])] & registradores[getIndiceReg(args[2])];
        }
        else if (op == "OR") {
            registradores[getIndiceReg(args[0])] = registradores[getIndiceReg(args[1])] | registradores[getIndiceReg(args[2])];
        }
        else if (op == "SLL") {
            registradores[getIndiceReg(args[0])] = registradores[getIndiceReg(args[1])] << args[2].toInt();
        }
        else if (op == "SLT") {
            registradores[getIndiceReg(args[0])] = (registradores[getIndiceReg(args[1])] < registradores[getIndiceReg(args[2])]) ? 1 : 0;
        }
        else if (op == "SLTI") {
            // slti $rt, $rs, immediate
            int rt = getIndiceReg(args[0]);
            int rs = getIndiceReg(args[1]);
            int imm = args[2].toInt();
            registradores[rt] = (registradores[rs] < imm) ? 1 : 0;
        }
        // Memória
        else if (op == "LUI") {
            registradores[getIndiceReg(args[0])] = (args[1].toInt() << 16);
        }
        else if (op == "LW") {
            QString offset = args[1].split('(')[0];
            QString base = args[1].split('(')[1].replace(")", "");
            int endereco = registradores[getIndiceReg(base)] + offset.toInt();
            registradores[getIndiceReg(args[0])] = memoriaDados.value(endereco, 0);
        }
        else if (op == "SW") {
            QString offset = args[1].split('(')[0];
            QString base = args[1].split('(')[1].replace(")", "");
            int endereco = registradores[getIndiceReg(base)] + offset.toInt();
            memoriaDados[endereco] = registradores[getIndiceReg(args[0])];
            log("Memória alterada: " + QString::number(endereco) + " = " + QString::number(registradores[getIndiceReg(args[0])]));
        }
        else if (op == "SYSCALL") {
            int v0 = registradores[2];
            if (v0 == 1) {
                log("[SYSCALL] Saída: " + QString::number(registradores[4]));
            } else if (v0 == 10) {
                log("[SYSCALL] Fim.");
                return true; // Avisa para parar
            }
        }
    } catch (...) {
        log("Erro na instrução: " + instr.textoOriginal);
        return true;
    }
    return false;
}

// --- Interface e Utilitários ---
void MainWindow::atualizarInterface() {
    if (ui->tabelaRegs->rowCount() != 35) ui->tabelaRegs->setRowCount(35);

    QMapIterator<QString, int> it(mapaNomesReg);
    while (it.hasNext()) {
        it.next();
        int idx = it.value();

        if (!ui->tabelaRegs->item(idx, 0)) ui->tabelaRegs->setItem(idx, 0, new QTableWidgetItem(it.key()));
        if (!ui->tabelaRegs->item(idx, 1)) ui->tabelaRegs->setItem(idx, 1, new QTableWidgetItem());
        if (!ui->tabelaRegs->item(idx, 2)) ui->tabelaRegs->setItem(idx, 2, new QTableWidgetItem());

        ui->tabelaRegs->item(idx, 1)->setText(QString::number(registradores[idx]));
        ui->tabelaRegs->item(idx, 2)->setText("0x" + QString::number(registradores[idx], 16).toUpper());
    }

    auto setExtra = [&](int row, QString label, int val) {
        if (!ui->tabelaRegs->item(row, 0)) ui->tabelaRegs->setItem(row, 0, new QTableWidgetItem(label));
        if (!ui->tabelaRegs->item(row, 1)) ui->tabelaRegs->setItem(row, 1, new QTableWidgetItem());
        ui->tabelaRegs->item(row, 1)->setText(QString::number(val));
    };

    setExtra(32, "PC", pc * 4);
    setExtra(33, "HI", hi);
    setExtra(34, "LO", lo);
}

void MainWindow::log(QString msg) {
    ui->txtLog->append(msg);
}

void MainWindow::abrirArquivo() {
    QString caminho = QFileDialog::getOpenFileName(this, "Abrir Código", "", "Assembly (*.asm *.txt *.s)");
    if (!caminho.isEmpty()) {
        QFile arq(caminho);
        if (arq.open(QIODevice::ReadOnly | QIODevice::Text)) {
            ui->txtCodigo->setText(arq.readAll());
            reiniciar();
        }
    }
}

// Conversor auxiliar para binário
QString MainWindow::intParaBinario(int valor, int bits) {
    QString bin = QString::number(valor, 2);
    if (valor < 0) {
        // Complemento de dois
        bin = QString::number((unsigned int)valor, 2);
    }
    while (bin.length() < bits) bin = "0" + bin;
    if (bin.length() > bits) bin = bin.right(bits);
    return bin;
}

// Geração completa de 32 bits
QString MainWindow::gerarBinario(Instrucao instr) {
    QString op = instr.opcode.toUpper().trimmed();
    QStringList args = instr.argumentos;

    // Mapas de códigos
    QString opcode = "000000";
    QString rs = "00000";
    QString rt = "00000";
    QString rd = "00000";
    QString shamt = "00000";
    QString funct = "000000";
    QString imm = "0000000000000000"; // 16 bits

    // --- TIPO R ---
    if (op == "ADD" || op == "SUB" || op == "AND" || op == "OR" || op == "SLT" || op == "MULT") {
        opcode = "000000";

        // Tratamento para MULT (apenas rs, rt)
        if (op == "MULT") {
            rs = intParaBinario(getIndiceReg(args[0]), 5);
            rt = intParaBinario(getIndiceReg(args[1]), 5);
            funct = "011000"; // 24
        } else {
            // Padrão: add rd, rs, rt
            rd = intParaBinario(getIndiceReg(args[0]), 5);
            rs = intParaBinario(getIndiceReg(args[1]), 5);
            rt = intParaBinario(getIndiceReg(args[2]), 5);

            if (op == "ADD") funct = "100000"; // 32
            if (op == "SUB") funct = "100010"; // 34
            if (op == "AND") funct = "100100"; // 36
            if (op == "OR")  funct = "100101"; // 37
            if (op == "SLT") funct = "101010"; // 42
        }
        return opcode + rs + rt + rd + shamt + funct;
    }

    // --- TIPO R (SLL - Shift) ---
    if (op == "SLL") {
        opcode = "000000";
        // sll rd, rt, shamt
        rd = intParaBinario(getIndiceReg(args[0]), 5);
        rt = intParaBinario(getIndiceReg(args[1]), 5);
        shamt = intParaBinario(args[2].toInt(), 5);
        funct = "000000"; // 0
        return opcode + "00000" + rt + rd + shamt + funct;
    }

    // --- TIPO I ---
    // Formato: op(6) rs(5) rt(5) imm(16)
    if (op == "ADDI" || op == "LW" || op == "SW" || op == "LUI" || op == "SLTI") {

        if (op == "ADDI") {
            opcode = "001000";
            rt = intParaBinario(getIndiceReg(args[0]), 5);
            rs = intParaBinario(getIndiceReg(args[1]), 5);
            imm = intParaBinario(args[2].toInt(), 16);
        }
        else if (op == "SLTI") {
            opcode = "001010";
            rt = intParaBinario(getIndiceReg(args[0]), 5);
            rs = intParaBinario(getIndiceReg(args[1]), 5);
            imm = intParaBinario(args[2].toInt(), 16);
        }
        else if (op == "LUI") {
            opcode = "001111";
            rt = intParaBinario(getIndiceReg(args[0]), 5);
            rs = "00000";
            imm = intParaBinario(args[1].toInt(), 16);
        }
        else if (op == "LW" || op == "SW") {
            if (op == "LW") opcode = "100011";
            else opcode = "101011";

            // arg: $t0, 4($t1)
            // rt = $t0
            rt = intParaBinario(getIndiceReg(args[0]), 5);

            QString offsetStr = args[1].split('(')[0];
            QString baseStr = args[1].split('(')[1].replace(")", "");

            rs = intParaBinario(getIndiceReg(baseStr), 5);
            imm = intParaBinario(offsetStr.toInt(), 16);
        }

        return opcode + rs + rt + imm;
    }

    // Syscall
    if (op == "SYSCALL") return "00000000000000000000000000001100";

    return "11111111111111111111111111111111"; // Erro/Desconhecido
}

//Geração de Relatório
void MainWindow::gerarRelatorio() {
    QFile file("relatorio.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "=== RELATORIO DE EXECUCAO MIPS ===\n";
        out << "Status Final dos Registradores:\n\n";

        QMapIterator<QString, int> it(mapaNomesReg);
        while (it.hasNext()) {
            it.next();
            int idx = it.value();
            out << it.key() << "\t: " << registradores[idx] << "\n";
        }
        out << "\nPC: " << pc * 4 << "\n";
        out << "HI: " << hi << "\n";
        out << "LO: " << lo << "\n";
        file.close();

        log("Relatório 'relatorio.txt' gerado com sucesso.");
        QMessageBox::information(this, "Relatório", "Arquivo 'relatorio.txt' gerado na pasta do projeto.");
    } else {
        log("Erro ao criar relatório.");
    }
}
