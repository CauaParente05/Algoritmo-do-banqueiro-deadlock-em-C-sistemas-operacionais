# Algoritmo de Banker

Este projeto implementa o Algoritmo de Banker para prevenção de deadlocks, baseado no livro *Operating System Concepts* (Silberschatz et al., 10ª edição). O programa lê a configuração de clientes e recursos a partir de arquivos de entrada, processa uma sequência de requisições e liberações de recursos e gera um arquivo de saída com o resultado de cada operação. O número de clientes e de tipos de recursos é dinâmico, determinado pelos arquivos de entrada e pelos argumentos de linha de comando.

## 💻 Sistema Operacional de Implementação e Suporte

Este projeto foi desenvolvido e testado no **Ubuntu 24.04 LTS (Linux)**.

O código foi escrito em C padrão e é totalmente compatível e executável nativamente em ambientes **Linux, Unix e macOS**.

## 📂 Descrição dos Arquivos Utilizados

* **`banker.h`**: Arquivo de cabeçalho. Declara os protótipos das funções auxiliares `is_safe_state` e `print_system_state`, utilizadas pelo `main.c`.
* **`banker.c`**: Contém a implementação das duas funções auxiliares:
  * `is_safe_state` — executa o algoritmo de segurança do Banker; retorna `true` se o estado atual for seguro e `false` caso contrário.
  * `print_system_state` — imprime a tabela de estado do sistema (MAXIMUM, ALLOCATION, NEED e AVAILABLE) no arquivo de saída.
* **`main.c`**: Ponto de entrada do programa. Responsável por ler os argumentos da linha de comando, carregar `customer.txt` e `commands.txt`, processar cada comando (RQ, RL e \*) e escrever o resultado em `result.txt`. Inclui validação de erros para argumentos inválidos, arquivos ausentes e formatação incorreta.
* **`Makefile`**: Script para automação da compilação. Gerencia as dependências entre os arquivos `.c` e `.h` e gera o executável `banker` na raiz do projeto.

## ⚙️ Como Compilar

Certifique-se de ter o compilador `gcc` e a ferramenta `make` instalados. Na raiz do projeto, utilize os seguintes comandos:

**Para compilar o projeto:**
```
make
```

**Para limpar os arquivos objetos e os executáveis:**
```
make clean
```

## ▶️ Como Executar

Os arquivos `customer.txt` e `commands.txt` devem estar no mesmo diretório do executável. Os argumentos de linha de comando representam o número de instâncias de cada tipo de recurso:

```
./banker <instâncias_recurso_1> <instâncias_recurso_2> ...
```

**Exemplo** (3 tipos de recursos com 10, 5 e 7 instâncias):
```
./banker 10 5 7
```

O resultado será escrito automaticamente em `result.txt` no diretório atual.

### Formato de `customer.txt`

Cada linha representa um cliente; cada valor separado por vírgula representa o máximo de instâncias que aquele cliente pode requisitar de cada tipo de recurso:

```
7,5,3
3,2,2
9,0,2
```

### Formato de `commands.txt`

Cada linha contém um comando seguido do ID do cliente e das quantidades por tipo de recurso:

```
RQ 0 0 2 1    ← requisição de recursos
RL 0 0 1 1    ← liberação de recursos
*             ← imprime o estado atual do sistema
```


### Cenários testados:

* **Sem argumentos na linha de comando:** o programa deve abortar com mensagem de erro e código de saída não-zero.
* **Arquivos de entrada ausentes:** o programa detecta que `customer.txt` ou `commands.txt` não existem e encerra com erro.
* **Mismatch de recursos:** número de colunas em `customer.txt` diferente da quantidade de argumentos passados; o programa deve abortar com erro.
* **Exemplo completo do enunciado:** usa exatamente os arquivos de entrada descritos no PDF e verifica alocações, liberação, tabela de estado e os três tipos de negação (recursos insuficientes, estado inseguro e exceder o máximo).
* **Liberação libera recursos:** após um `RL`, uma requisição que antes seria negada por falta de recursos passa a ser aceita.