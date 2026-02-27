# Laboratório de Programação Paralela: Pthreads, Sincronização e GIL

Nesta atividade, vamos explorar a criação de fluxos de execução paralelos utilizando a biblioteca POSIX Threads (pthreads) na linguagem C.

Abordaremos a divisão de trabalho (worksharing), os perigos do compartilhamento de memória (Condições de Corrida / Race Conditions) e os mecanismos de proteção (Regiões Críticas e Mutexes). Ao final, faremos uma análise comparativa com o modelo de threads da linguagem Python.

## Diretrizes para o Relatório (Evidências)

O relatório não deve ser apenas um "copia e cola" de código. Ele deve ser um documento analítico (em PDF) contendo:

Especificações do Ambiente: Descreva a máquina onde os testes foram executados (Processador, quantidade de núcleos físicos e lógicos, Sistema Operacional, quantidade de RAM).

Prints de Execução: Capturas de tela do terminal mostrando a compilação e execução dos códigos.

Análise de Desempenho: Sempre que aplicável, compare o tempo de execução da versão Sequencial (Baseline) com a versão Paralela. Calcule o Speedup ($S = T_{sequencial} / T_{paralelo}$).

Trechos de Código Relevantes: Não cole o código inteiro no PDF. Destaque apenas a parte onde as threads são criadas, a divisão de trabalho e a região crítica. Lembre-se de submeter os arquivos .c completos no GitHub.

Atenção à Compilação em C: Lembre-se de compilar seus programas linkando a biblioteca pthread:
`gcc meu_programa.c -o meu_programa -lpthread -Wall`

## Parte 1: O Problema da Conta Corrente (Obrigatório)

Neste experimento, vamos reproduzir um problema clássico de condição de corrida e, em seguida, aplicar a solução correta.

Imagine uma conta bancária compartilhada onde depósitos e saques ocorrem simultaneamente de forma massiva.

### Fase 0: O Baseline Sequencial

Utilize o arquivo `contacorrente.c` (fornecido no repositório). Implemente funções de depósito e saque rodando em um laço muito grande (ex: $100.000.000$ de iterações). No final, o saldo deve ser exatamente o esperado matematicamente. Imprima o saldo e o tempo de execução.

## Fase 1: O Caos (Race Condition)

Crie o arquivo `contacorrentef1.c`. Transforme as funções de saque e depósito para rodarem em threads separadas e simultâneas.

O que observar: Como ambas as threads acessam e modificam a variável global saldo ao mesmo tempo (leitura-modificação-escrita), os resultados ao final da execução serão incorretos e imprevisíveis. Execute várias vezes e registre no relatório que os valores finais mudam a cada execução.

### Fase 2: A Ordem (Mutex)

Crie o arquivo `contacorrentef2.c`. Utilize a estrutura `pthread_mutex_t` para proteger a região crítica (a linha de código que altera o saldo).

O que observar: O resultado final voltará a ser determinístico e matematicamente correto. Meça o tempo de execução. Ele foi maior ou menor que o da Fase 1? Explique o porquê no seu relatório.

## Parte 2: Menu de Problemas (Escolha 3)

Abaixo estão 5 problemas clássicos de programação paralela de complexidade semelhante. Você deve escolher 3 deles para implementar usando Pthreads. Para cada um dos escolhidos, seu programa deve receber o número de threads via argumento de linha de comando: `$ ./programa <entrada> <num_threads>`. Seu relatório deve conter os tempos de execução para 1, 2, 4 e 8 threads.

### Opção A: Busca de Números Primos

Faça um programa que encontre e conte todos os números primos em uma faixa de valores de $1$ a $K$.

Desafio: Divida a faixa de busca uniformemente entre as $N$ threads. Lembre-se que verificar números grandes demora mais que verificar números pequenos (cuidado com o balanceamento de carga!).

### Opção B: Cálculo de Pi pelo Método de Monte Carlo

O valor de $\pi$ pode ser estimado sorteando pontos aleatórios dentro de um quadrado de lado 2 e verificando quantos caem dentro de um círculo de raio 1 inscrito neste quadrado.

Desafio: Divida o número total de "dardos" (iterações) entre as threads. Dica: Não use a função `rand()` padrão, pois ela tem um lock interno que mata o paralelismo. Use `rand_r()` e passe uma "semente" diferente para cada thread.

### Opção C: Soma de Vetores Gigantes

Crie dois vetores $A$ e $B$ com milhões de posições, preenchidos com valores aleatórios. Calcule um vetor $C$ onde $C[i] = A[i] + B[i]$.

Desafio: Divida o índice do vetor (ex: de $0$ a $N/2$ para a thread 1, e $N/2+1$ a $N$ para a thread 2). Compare o tempo de realizar isso sequencialmente vs. paralelamente

### Opção D: Busca de Padrões em Texto

Gere um vetor gigantesco de caracteres aleatórios (ex: 500 MB em memória). Peça para o programa contar quantas vezes a letra 'X' ou um padrão específico de 2 letras aparece.

Desafio: Cada thread varre uma parte do vetor de texto. Ao final, elas precisam somar seus resultados parciais em uma variável global de contagem total (protegida por Mutex, ou somada na thread principal após o `pthread_join`).

### Opção E: Multiplicação de Matriz por Vetor

Crie uma matriz $M$ ($10.000 \times 10.000$) e um vetor $V$ ($10.000$). Multiplique-os resultando em um vetor $R$.

Desafio: Paralelize o processo dividindo as linhas da matriz entre as threads. Cada thread computa os resultados de uma fatia de linhas do vetor $R$ resultante.

## Parte 3: O Desafio do Python e o GIL (Global Interpreter Lock)

Muitos desenvolvedores acreditam que apenas adicionar threads a um código o fará executar mais rápido em processadores modernos (Multi-core). Para testar essa teoria, vamos sair da linguagem C e usar Python.

Abaixo temos dois códigos que realizam um trabalho intenso de CPU (contagem regressiva).

### Código 1: Sequencial (seq.py)

```
import time

def contagem_pesada(n):
    while n > 0:
        n -= 1

if __name__ == "__main__":
    n = 100_000_000
    inicio = time.time()
    
    # Executa a contagem duas vezes sequencialmente
    contagem_pesada(n)
    contagem_pesada(n)
    
    fim = time.time()
    print(f"Tempo Sequencial: {fim - inicio:.4f} segundos")
```

### Código 2: Paralelo com Threads (threads.py)

```
import time
import threading

def contagem_pesada(n):
    while n > 0:
        n -= 1

if __name__ == "__main__":
    n = 100_000_000
    inicio = time.time()
    
    # Cria duas threads, cada uma faz metade do trabalho (1 contagem pesada)
    t1 = threading.Thread(target=contagem_pesada, args=(n,))
    t2 = threading.Thread(target=contagem_pesada, args=(n,))
    
    t1.start()
    t2.start()
    
    t1.join()
    t2.join()
    
    fim = time.time()
    print(f"Tempo com Threads: {fim - inicio:.4f} segundos")
```


#### Tarefa do Desafio:

Execute ambos os scripts Python no seu computador.

Analise os tempos. O código com threads executou na metade do tempo? Foi mais rápido, igual, ou mais lento?

**Pesquisa Obrigatória para o Relatório**: Pesquise sobre o GIL (Global Interpreter Lock) do CPython. Explique no seu relatório o que é o GIL e por que ele afeta programas Python limitados por CPU (CPU-bound) que usam threads.

Bônus: Cite qual biblioteca ou módulo do Python você deveria usar no lugar de threading para realmente obter paralelismo neste tipo de problema matemático.

## Critérios de Avaliação

Para que o relatório seja considerado satisfatório e obtenha a nota máxima, ele será avaliado com base nos seguintes critérios:

1. Completude da Tarefa (30%): O aluno deve comprovar a implementação, execução e avaliação de todas as atividades obrigatórias solicitadas. Isso inclui as Fases 0, 1 e 2 da Conta Corrente, a implementação completa de 3 opções selecionadas do Menu de Problemas e a execução do Desafio em Python.

2. Clareza e Documentação do Código (20%): Os arquivos de código (.c e .py) submetidos devem estar limpos, organizados e indentados. É exigido o uso de variáveis com nomes semânticos e, principalmente, comentários claros e objetivos explicando as áreas-chave do paralelismo (como a lógica de divisão de trabalho das threads e a proteção das regiões críticas).

3. Qualidade do Relatório e Evidências (25%): O documento em PDF deve conter as especificações completas do hardware e software utilizados. Deve apresentar as capturas de tela (prints) legíveis das execuções no terminal, demonstrando claramente as variações nos testes (como as execuções com 1, 2, 4 e 8 threads para as opções do Menu).

4. Análise Crítica e Conclusões (25%): O relatório não deve apenas exibir números. O aluno deve analisar criticamente o desempenho: calcular o Speedup corretamente, organizar os tempos em tabelas/gráficos e justificar de maneira técnica o porquê de um código ter sido mais lento ou mais rápido (explicando o overhead do mutex ou detalhando teoricamente como o GIL restringe as threads no Python).

---

Bom laboratório! Mantenham o foco nas evidências e na interpretação dos resultados de tempo!
