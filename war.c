#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define NUM_TERRITORIES 5
#define MAX_NAME 50
#define MAX_MISSION 100

typedef struct {
    char name[MAX_NAME];
    char army_color[MAX_NAME];
    int troops;
    int id; 
} Territory;

typedef struct {
    char description[MAX_MISSION];
    int type; 
    char target_color[MAX_NAME]; 
    int conquest_target;      
} Mission;


// Função para rolar um dado
int roll_die() {
    return rand() % 6 + 1;
}

// Inicia a missão. 
void initialize_mission(Mission *mission, Territory *map) {
    srand(time(NULL)); 
    int type = rand() % 2 + 1; 
    mission->type = type;

    if (type == 1) { 
        const char *targets[] = {"AZUL", "AMARELO", "VERDE"};
        int color_index = rand() % 3;
        
        strncpy(mission->target_color, targets[color_index], MAX_NAME - 1);
        snprintf(mission->description, MAX_MISSION, "Destruir o exército %s.", mission->target_color);
    } else { 
        mission->conquest_target = 3;
        snprintf(mission->description, MAX_MISSION, "Conquistar %d territórios (ter 3 ou mais) do exército RED.", mission->conquest_target);
    }
}

// Exibe o mapa
int display_map(const Territory *map) {
    printf("\n--- STATUS ATUAL DO MAPA ---\n");
    printf("| ID | %-10s | %-10s | %-6s |\n", "Território", "Exército", "Tropas");
    printf("|----|------------|------------|--------|\n");
    for (int i = 0; i < NUM_TERRITORIES; i++) {
        printf("| %-2d | %-10s | %-10s | %-6d |\n",
               map[i].id, map[i].name, map[i].army_color, map[i].troops);
    }
    printf("----------------------------\n");

    return 1; // retorna algo sempre
}

// Verificação de Missão

int check_mission(const Mission *mission, const Territory *map) {
    if (mission->type == 1) { 
        for (int i = 0; i < NUM_TERRITORIES; i++) {
            if (strcmp(map[i].army_color, mission->target_color) == 0) {
                return 0; 
            }
        }
        return 1; 
    } else { 
        int red_count = 0;
        for (int i = 0; i < NUM_TERRITORIES; i++) {
            if (strcmp(map[i].army_color, "RED") == 0) {
                red_count++;
            }
        }
        return red_count >= mission->conquest_target;
    }
}

int main() {
    // Inicialização 
    Territory *map = (Territory *)calloc(NUM_TERRITORIES, sizeof(Territory));
    if (map == NULL) {
        perror("Erro de alocação de memória. O programa será encerrado.");
        return 1;
    }

    Mission mission;
    int option;
    int game_active = 1;

    const char *names[] = {"BRASIL", "CANADA", "PERU", "CHILE", "MEXICO"};
    const char *colors[] = {"VERMELHO", "AZUL", "AMARELO", "VERDE", "VERMELHO"};
    const int troops_initial[] = {3, 2, 4, 2, 3};

    // Loop simples para preencher a struct
    for (int i = 0; i < NUM_TERRITORIES; i++) {
        map[i].id = i + 1;
        strncpy(map[i].name, names[i], MAX_NAME - 1);
        strncpy(map[i].army_color, colors[i], MAX_NAME - 1);
        map[i].troops = troops_initial[i];
    }
    
    // Inicia a missão
    initialize_mission(&mission, map);

    printf("==========================================\n");
    printf("     Desafio WAR Estruturado – Conquista de Territórios\n");
    printf("  SEU EXÉRCITO É O VERMELHO.\n");
    printf("  MISSÃO: %s\n", mission.description);
    printf("==========================================\n");

    //Loop do jogo
    while (game_active) {
        display_map(map);

        printf("\n--- MENU ---\n");
        printf("1 - ATACAR\n");
        printf("2 - VERIFICAR MISSÃO\n");
        printf("0 - SAIR\n");
        
        printf("Escolha sua acao: ");
        if (scanf("%d", &option) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            option = -1; 
        }


        switch (option) {
            case 1: {
                int attacker_id, defender_id;
                
                printf("\n** TURNO DE ATAQUE **\n");
                printf("ATACANDO TERRITÓRIO ID (1 to 5): ");
                scanf("%d", &attacker_id);
                printf("DEFENDENDO TERRITÓRIO ID (1 to 5): ");
                scanf("%d", &defender_id);

                int i_a = attacker_id - 1;
                int i_d = defender_id - 1;

                if (i_a < 0 || i_a >= NUM_TERRITORIES || i_d < 0 || i_d >= NUM_TERRITORIES || i_a == i_d) {
                    printf("ID inválido, tente de novo\n");
                    break;
                }
                
                if (strcmp(map[i_a].army_color, "VERMELHO") != 0) {
                    printf("So pode atacar com o territorio do seu exercito\n");
                    break;
                }
                if (strcmp(map[i_d].army_color, "VERMELHO") == 0) {
                    printf("Voce nao pode atacar o territorio do seu proprio exercito\n");
                    break;
                }
                if (map[i_a].troops < 2) {
                    printf("O atacante precisa de pelo menos 2 tropas para atacar.\n");
                    break;
                }

                // Lógica da Batalha 
                printf("\n--- BATALHA: %s ATAQUES %s ---\n", map[i_a].name, map[i_d].name);

                int die_a = roll_die();
                int die_d = roll_die();

                printf("Dados rolados: Atacante: %d | Defensor: %d\n", die_a, die_d);

                if (die_a > die_d) {
                    printf("ATACANTE VENCE! %s perde 1 tropa.\n", map[i_d].name);
                    map[i_d].troops--;
                } else { 
                    printf("DEFENSOR VENCE! %s perde 1 tropa.\n", map[i_a].name);
                    map[i_a].troops--;
                }

                // Verifica Conquista
                if (map[i_d].troops == 0) {
                    printf("\n!!! CONQUISTA !!! O territorio %s foi tomado!\n", map[i_d].name);
                    // O atacante move 1 tropa para o novo território
                    map[i_a].troops--;
                    map[i_d].troops = 1;
                    // Muda a cor do exército para o do atacante
                    strncpy(map[i_d].army_color, map[i_a].army_color, MAX_NAME - 1);
                }

                break;
            }
            case 2: {
                printf("\n** VERIFICANDO MISSAO **\n");
                if (check_mission(&mission, map)) {
                    printf("******************************************\n");
                    printf("!!! VITORIA !!! MISSAO COMPLETA!\n");
                    printf("******************************************\n");
                    game_active = 0; 
                } else {
                    printf("A missao ainda nao foi completa. Continue batalhando!\n");
                }
                break;
            }
            case 0:
                printf("\nSaindo do jogo. Volte logo!\n");
                game_active = 0;
                break;
            default:
                printf("Opcao invalida. Escolha 0, 1, or 2.\n");
                break;
        }
    }

    //Limpeza 
    printf("\nLiberando memoria alocada...\n");
    free(map);
    map = NULL;
    return 0;
}