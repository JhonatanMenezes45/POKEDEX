

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOME_DO_ARQUIVO "pokedex.csv"

/* ---------- O REGISTRO: UM POKEMON ---------- */
struct Pokemon {
    int  numeroPokedex;   /* chave unica */
    char nome[30];
    char tipo[20];
    char treinador[30];
};

/* ---------- O NO DA ARVORE ---------- */
struct No {
    struct Pokemon pokemon;       /* o registro completo fica dentro do no */
    struct No *filhoEsquerdo;     /* numeros MENORES que o do pai */
    struct No *filhoDireito;      /* numeros MAIORES que o do pai */
};

/* ---------- LEITURA DO TECLADO ---------- */

/* Pede um numero inteiro e repete a pergunta ate digitar um numero valido */
int lerNumeroInteiro(const char *pergunta) {
    int numero;
    int caractere;

    printf("%s", pergunta);
    while (scanf("%d", &numero) != 1) {
        /* digitou letra: limpa a linha e pergunta de novo */
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
        }
        printf("Valor invalido. %s", pergunta);
    }
    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }
    return numero;
}

/* ---------- OPERACOES DA ARVORE ---------- */

/* Cria um no novo na memoria com o Pokemon dentro */
struct No *criarNo(struct Pokemon pokemon) {
    struct No *novoNo = (struct No *)malloc(sizeof(struct No));
    if (novoNo == NULL) {
        printf("erro: sem memoria\n");
        exit(1);
    }
    novoNo->pokemon = pokemon;
    novoNo->filhoEsquerdo = NULL;
    novoNo->filhoDireito = NULL;
    return novoNo;
}

/* INSERIR: desce pela arvore ate achar o lugar vazio.
 * Menor vai para a esquerda, maior vai para a direita.
 * Se o numero ja existe, nao insere e avisa pelo "conseguiuInserir". */
struct No *inserirNaArvore(struct No *raiz, struct Pokemon pokemon, int *conseguiuInserir) {
    if (raiz == NULL) {
        *conseguiuInserir = 1;
        return criarNo(pokemon);
    }
    if (pokemon.numeroPokedex < raiz->pokemon.numeroPokedex) {
        raiz->filhoEsquerdo = inserirNaArvore(raiz->filhoEsquerdo, pokemon, conseguiuInserir);
    } else if (pokemon.numeroPokedex > raiz->pokemon.numeroPokedex) {
        raiz->filhoDireito = inserirNaArvore(raiz->filhoDireito, pokemon, conseguiuInserir);
    } else {
        *conseguiuInserir = 0;   /* numero repetido */
    }
    return raiz;
}

/* BUSCAR: compara com o no atual e vai para esquerda ou direita.
 * Devolve o no encontrado, ou NULL se nao existe. */
struct No *buscarNaArvore(struct No *raiz, int numeroProcurado) {
    if (raiz == NULL) {
        return NULL;
    }
    if (numeroProcurado == raiz->pokemon.numeroPokedex) {
        return raiz;
    }
    if (numeroProcurado < raiz->pokemon.numeroPokedex) {
        return buscarNaArvore(raiz->filhoEsquerdo, numeroProcurado);
    }
    return buscarNaArvore(raiz->filhoDireito, numeroProcurado);
}

void mostrarPokemon(struct Pokemon pokemon) {
    printf("No %d | %s | Tipo: %s | Treinador: %s\n",
           pokemon.numeroPokedex, pokemon.nome, pokemon.tipo, pokemon.treinador);
}

/* LISTAR (percurso em ordem): esquerda -> raiz -> direita.
 * Isso mostra os numeros em ordem crescente. */
void listarEmOrdem(struct No *raiz) {
    if (raiz != NULL) {
        listarEmOrdem(raiz->filhoEsquerdo);
        mostrarPokemon(raiz->pokemon);
        listarEmOrdem(raiz->filhoDireito);
    }
}

/* EXIBIR A ARVORE (percurso em pre-ordem): raiz -> esquerda -> direita.
 * O "nivelDoNo" controla quantos espacos de recuo cada linha recebe. */
void exibirArvore(struct No *raiz, int nivelDoNo, const char *lado) {
    int espaco;
    if (raiz != NULL) {
        for (espaco = 0; espaco < nivelDoNo; espaco++) {
            printf("    ");
        }
        printf("[%s] %d - %s\n", lado, raiz->pokemon.numeroPokedex, raiz->pokemon.nome);
        exibirArvore(raiz->filhoEsquerdo, nivelDoNo + 1, "esquerda");
        exibirArvore(raiz->filhoDireito, nivelDoNo + 1, "direita");
    }
}

/* CONTAR: 1 (este no) + quantos tem na esquerda + quantos tem na direita */
int contarPokemons(struct No *raiz) {
    if (raiz == NULL) {
        return 0;
    }
    return 1 + contarPokemons(raiz->filhoEsquerdo) + contarPokemons(raiz->filhoDireito);
}

/* Acha o menor numero de uma arvore: desce sempre pela esquerda */
struct No *acharMenorNo(struct No *raiz) {
    while (raiz->filhoEsquerdo != NULL) {
        raiz = raiz->filhoEsquerdo;
    }
    return raiz;
}

/* REMOVER: primeiro procura o no (igual a busca). Quando acha, ha 3 casos:
 * 1) sem filhos (folha): so libera o no.
 * 2) com 1 filho: o filho ocupa o lugar do no removido.
 * 3) com 2 filhos: copia o sucessor (menor numero da direita) para este no
 *    e remove o sucessor, que cai no caso 1 ou 2. */
struct No *removerDaArvore(struct No *raiz, int numeroProcurado, int *conseguiuRemover) {
    struct No *noAuxiliar;
    struct No *sucessor;

    if (raiz == NULL) {
        *conseguiuRemover = 0;   /* nao achou */
        return NULL;
    }
    if (numeroProcurado < raiz->pokemon.numeroPokedex) {
        raiz->filhoEsquerdo = removerDaArvore(raiz->filhoEsquerdo, numeroProcurado, conseguiuRemover);
    } else if (numeroProcurado > raiz->pokemon.numeroPokedex) {
        raiz->filhoDireito = removerDaArvore(raiz->filhoDireito, numeroProcurado, conseguiuRemover);
    } else {
        /* achou o no a remover */
        if (raiz->filhoEsquerdo == NULL) {          /* caso 1 ou 2: sem filho esquerdo */
            noAuxiliar = raiz->filhoDireito;
            free(raiz);
            *conseguiuRemover = 1;
            return noAuxiliar;
        }
        if (raiz->filhoDireito == NULL) {           /* caso 2: so tem filho esquerdo */
            noAuxiliar = raiz->filhoEsquerdo;
            free(raiz);
            *conseguiuRemover = 1;
            return noAuxiliar;
        }
        /* caso 3: dois filhos */
        sucessor = acharMenorNo(raiz->filhoDireito);
        raiz->pokemon = sucessor->pokemon;
        raiz->filhoDireito = removerDaArvore(raiz->filhoDireito, sucessor->pokemon.numeroPokedex, conseguiuRemover);
    }
    return raiz;
}

/* Libera todos os nos: primeiro os filhos, depois o proprio no (pos-ordem) */
void liberarArvore(struct No *raiz) {
    if (raiz != NULL) {
        liberarArvore(raiz->filhoEsquerdo);
        liberarArvore(raiz->filhoDireito);
        free(raiz);
    }
}

/* ---------- ARQUIVO ---------- */

/* Grava em PRE-ORDEM (raiz primeiro). Assim, ao ler o arquivo de volta e
 * inserir linha por linha, a arvore fica com a mesma forma de antes. */
void gravarEmPreOrdem(struct No *raiz, FILE *arquivo) {
    if (raiz != NULL) {
        fprintf(arquivo, "%d;%s;%s;%s\n",
                raiz->pokemon.numeroPokedex, raiz->pokemon.nome,
                raiz->pokemon.tipo, raiz->pokemon.treinador);
        gravarEmPreOrdem(raiz->filhoEsquerdo, arquivo);
        gravarEmPreOrdem(raiz->filhoDireito, arquivo);
    }
}

void salvarNoArquivo(struct No *raiz) {
    FILE *arquivo = fopen(NOME_DO_ARQUIVO, "w");
    if (arquivo == NULL) {
        printf("erro: nao foi possivel abrir o arquivo\n");
        exit(1);
    }
    gravarEmPreOrdem(raiz, arquivo);
    fclose(arquivo);
}

/* Le o arquivo linha por linha e insere cada Pokemon na arvore */
struct No *carregarDoArquivo(struct No *raiz) {
    struct Pokemon pokemon;
    int conseguiuInserir;
    FILE *arquivo = fopen(NOME_DO_ARQUIVO, "r");

    if (arquivo == NULL) {
        return raiz;   /* arquivo ainda nao existe: arvore comeca vazia */
    }
    while (fscanf(arquivo, "%d;%29[^;];%19[^;];%29[^\n]\n",
                  &pokemon.numeroPokedex, pokemon.nome,
                  pokemon.tipo, pokemon.treinador) == 4) {
        raiz = inserirNaArvore(raiz, pokemon, &conseguiuInserir);
    }
    fclose(arquivo);
    return raiz;
}

/* ---------- OPCOES DO MENU ---------- */

struct No *opcaoCadastrar(struct No *raiz) {
    struct Pokemon novoPokemon;
    int conseguiuInserir;

    novoPokemon.numeroPokedex = lerNumeroInteiro("Numero da Pokedex: ");
    if (buscarNaArvore(raiz, novoPokemon.numeroPokedex) != NULL) {
        printf("Esse numero ja esta cadastrado. Cadastro recusado.\n");
        return raiz;
    }
    printf("Nome: ");
    scanf(" %29[^\n]", novoPokemon.nome);
    printf("Tipo: ");
    scanf(" %19[^\n]", novoPokemon.tipo);
    printf("Treinador: ");
    scanf(" %29[^\n]", novoPokemon.treinador);

    raiz = inserirNaArvore(raiz, novoPokemon, &conseguiuInserir);
    if (conseguiuInserir) {
        printf("Pokemon cadastrado!\n");
    }
    return raiz;
}

void opcaoBuscar(struct No *raiz) {
    int numeroProcurado = lerNumeroInteiro("Numero a buscar: ");
    struct No *noEncontrado = buscarNaArvore(raiz, numeroProcurado);

    if (noEncontrado == NULL) {
        printf("O numero %d nao existe na Pokedex.\n", numeroProcurado);
    } else {
        mostrarPokemon(noEncontrado->pokemon);
    }
}

void opcaoListar(struct No *raiz) {
    if (raiz == NULL) {
        printf("A Pokedex esta vazia.\n");
    } else {
        listarEmOrdem(raiz);
    }
}

void opcaoExibirArvore(struct No *raiz) {
    if (raiz == NULL) {
        printf("A arvore esta vazia.\n");
    } else {
        exibirArvore(raiz, 0, "raiz");
    }
}

/* Operacao extra 1: contar quantos Pokemon estao cadastrados */
void opcaoContar(struct No *raiz) {
    printf("Total de Pokemon cadastrados: %d\n", contarPokemons(raiz));
}

/* Operacao extra 2: remover um Pokemon pelo numero */
struct No *opcaoRemover(struct No *raiz) {
    int conseguiuRemover = 0;
    int numeroProcurado = lerNumeroInteiro("Numero a remover: ");

    raiz = removerDaArvore(raiz, numeroProcurado, &conseguiuRemover);
    if (conseguiuRemover) {
        printf("Pokemon removido!\n");
    } else {
        printf("O numero %d nao existe na Pokedex.\n", numeroProcurado);
    }
    return raiz;
}

/* ---------- PROGRAMA PRINCIPAL ---------- */

int main(void) {
    struct No *raizDaArvore = NULL;   /* arvore comeca vazia */
    int opcaoEscolhida;

    raizDaArvore = carregarDoArquivo(raizDaArvore);

    do {
        printf("\n===== POKEDEX =====\n");
        printf("1 - Cadastrar Pokemon\n");
        printf("2 - Buscar Pokemon\n");
        printf("3 - Listar todos (ordem crescente)\n");
        printf("4 - Exibir a arvore\n");
        printf("5 - Contar Pokemon\n");
        printf("6 - Remover Pokemon\n");
        printf("0 - Sair\n");
        opcaoEscolhida = lerNumeroInteiro("Escolha: ");

        switch (opcaoEscolhida) {
            case 1: raizDaArvore = opcaoCadastrar(raizDaArvore); break;
            case 2: opcaoBuscar(raizDaArvore); break;
            case 3: opcaoListar(raizDaArvore); break;
            case 4: opcaoExibirArvore(raizDaArvore); break;
            case 5: opcaoContar(raizDaArvore); break;
            case 6: raizDaArvore = opcaoRemover(raizDaArvore); break;
            case 0: break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcaoEscolhida != 0);

    salvarNoArquivo(raizDaArvore);
    liberarArvore(raizDaArvore);
    printf("Dados gravados em %s. Ate logo!\n", NOME_DO_ARQUIVO);
    return 0;
}
