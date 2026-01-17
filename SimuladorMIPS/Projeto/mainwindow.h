/**
 * @file mainwindow.h
 * @brief Definição da classe principal e lógica do Simulador MIPS.
 * @author Thiago Wesley Pasquim de Melo
 * @author Vinicius Gabriel Giordani Barbosa
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Classe principal que integra a GUI com o simulador do processador.
 *
 * Herda de QMainWindow e é responsável por gerenciar os eventos da interface,
 * manter o estado dos componentes do processador (PC, Registradores, Memória)
 * e executar o ciclo de instruções.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Construtor da classe MainWindow.
     * @param parent Ponteiro para o widget pai (padrão é nullptr).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destrutor da classe. Libera a memória da UI.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Lê o código Assembly do editor de texto.
     *
     * Realiza o parsing das instruções, remove comentários e labels,
     * e preenche a memória de instruções.
     */
    void carregarCodigo();

    /**
     * @brief Executa uma única instrução (Ciclo único).
     *
     * Lê a instrução apontada pelo PC, executa a lógica e atualiza a interface.
     * Avança o PC em 1 (word) se não houver saltos.
     */
    void executarPasso();

    /**
     * @brief Executa o programa completo.
     *
     * Executa instruções em loop até encontrar um SYSCALL de exit,
     * chegar ao fim da memória ou atingir o limite de segurança de ciclos.
     */
    void executarTudo();

    /**
     * @brief Reseta o simulador para o estado inicial.
     *
     * Zera o PC, registradores (incluindo HI/LO) e limpa as memórias
     * de dados e instruções.
     */
    void reiniciar();

    /**
     * @brief Abre uma caixa de diálogo para carregar um arquivo externo.
     *
     * Suporta arquivos com extensão .asm, .s e .txt.
     */
    void abrirArquivo();

private:
    Ui::MainWindow *ui; ///< Ponteiro para a interface gráfica gerada pelo Qt Designer.

    /**
     * @struct Instrucao
     * @brief Representa uma instrução Assembly decodificada.
     */
    struct Instrucao {
        QString textoOriginal;  ///< A linha de código original (ex: "add $t0, $t1, $t2").
        QString opcode;         ///< O mnemônico da operação (ex: "ADD", "LW").
        QStringList argumentos; ///< Lista de operandos (ex: ["$t0", "$t1", "$t2"]).
    };

    // --- Componentes do Processador ---

    int pc;                 ///< Program Counter: Índice da instrução atual.
    int hi;                 ///< Registrador HI: Armazena os 32 bits superiores da multiplicação.
    int lo;                 ///< Registrador LO: Armazena os 32 bits inferiores da multiplicação.
    int registradores[32];  ///< Banco de Registradores de Propósito Geral ($0 a $31).

    /**
     * @brief Memória de Dados (RAM).
     * Mapeia um endereço de memória (int) para um valor (int).
     */
    QMap<int, int> memoriaDados;

    /**
     * @brief Memória de Instruções.
     * Vetor que armazena a sequência de instruções carregadas.
     */
    std::vector<Instrucao> memoriaInstrucoes;

    /**
     * @brief Tabela de Mapeamento de Nomes de Registradores.
     * Exemplo: "$t0" -> 8, "$sp" -> 29.
     */
    QMap<QString, int> mapaNomesReg;

    // --- Métodos Auxiliares ---

    /**
     * @brief Inicializa o mapa de nomes dos registradores MIPS padrão.
     * Deve ser chamado no construtor.
     */
    void inicializarMapaRegs();

    /**
     * @brief Converte o nome de um registrador para seu índice numérico.
     * @param nome String do registrador (ex: "$t0").
     * @return Índice do registrador (0-31) ou -1 se inválido.
     */
    int getIndiceReg(QString nome);

    /**
     * @brief Gera a representação binária de 32 bits da instrução.
     *
     * Identifica o tipo da instrução (R, I, J) e monta os campos
     * Opcode, RS, RT, RD, Shamt e Funct conforme necessário.
     *
     * @param instr A estrutura da instrução a ser convertida.
     * @return String contendo 32 caracteres '0' ou '1'.
     */
    QString gerarBinario(Instrucao instr);

    /**
     * @brief Converte um inteiro para sua representação binária textual.
     *
     * @param valor O valor inteiro a ser convertido.
     * @param bits O número de bits da saída (preenche com zeros à esquerda ou trunca).
     * @return String binária formatada.
     */
    QString intParaBinario(int valor, int bits);

    /**
     * @brief Gera um arquivo de texto com o estado final do processador.
     *
     * Salva o arquivo "relatorio.txt" com os valores dos registradores
     * após a execução.
     */
    void gerarRelatorio();

    /**
     * @brief Processa a lógica de uma única instrução.
     *
     * Realiza operações na ULA ou acessos à memória.
     *
     * @param instr A instrução a ser processada.
     * @return true se a execução deve parar (ex: SYSCALL exit ou erro), false caso contrário.
     */
    bool processarInstrucao(Instrucao instr);

    /**
     * @brief Atualiza os elementos visuais da interface.
     *
     * Atualiza a tabela de registradores e labels de status com os valores atuais.
     */
    void atualizarInterface();

    /**
     * @brief Adiciona uma mensagem ao console de log da aplicação.
     * @param msg A mensagem a ser exibida.
     */
    void log(QString msg);
};

#endif // MAINWINDOW_H
