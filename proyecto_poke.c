///// bibliotecas /////
	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#include<time.h>
	#include<math.h>

///// estructuras /////
typedef struct{
	char name[20];
	int m_type; /// el es tipo del ataque (planta, fuego, etc...)
	int m_power; // (move power) es el ataque base del movimiento
	int m_category; // physical (0), special(1), status (2). Categoria del movimiento
	int pp; // es la cantidad de veces que un mismo ataque se puede usar
	int max_pp;
	int accuracy; // la precision base del movimiento
	int sc[2]; // (status condition) es el estado alterado que un movimiento puede entregar al blanco del movimiento ([0] no volatil y [1] volatil)
	int sc_accuracy; // la precision del estado alterado que el movimiento puede entregar (si es que el movimiento puede antregar un estado alterado para empezar)
	int statch[7]; // son los niveles que un movimiento puede vajar o subir las estadisticas del pokemon objetico.
	int statch_accuracy; // la precision del cambio en los niveles de las estadisticas que el movimiento puede causar (si es que el movimiento puede causar un cambio en las estadisticas para empezar)
	int selfch[11]; // self change
	int selfch_accuracy; //precision de selfch
	int flinch_accuracy; // precision de flincheo (es 0 en caso de que no haga flinch)
} Move;
typedef struct{
	int p_code;
	char name[13];
	float type_multiplier[19]; // las debilidades o fortalezas de los movimientos que un pokemon soporta
	Move move[4]; // los 4 movimientos que cada pokemon tiene
	int status[9]; // los estados alterados que un pokemon tiene (solo el [0] se mantiene cuando se hace un switch)
	int stat_change[7]; // los niveles positivos o negativos que cambiados en un pokemon
	int type1; // el 1er tipo de un pokemon
	int type2; // el 2do tipo de un pokemon
	int habilidad; // la habilidad pasiba del pokemon
	int max_hp; // la vida maxima que el pokemon puede tener
	int hp; // la vida que el pokemon tiene actualmente
	int attackstat[2]; // la estadistica de ataque de un pokemon ([0] physical, [1] especial)
	int defensestat[2]; // la estadistica de defensa de un pokemon ([0] physical, [1] especial)
	int speed; // la velocidad del un pokemon
} Pokemon;

///// var globales /////
	int pkmn_format;
	int flinch; //para ver si el pkmn es afectado por flinch o no
	int Current_Turn;
	Pokemon team[12]; // los 2 teams de los dos jugadores van aqui
	Pokemon pkmn[2]; // los dos pokemon en combate de los dos entrenadores van aqui

///// funciones /////

	///// funciones globales /////
		void delay(unsigned int mseconds){
			clock_t goal = mseconds*1000 + clock ();
			while (goal > clock());
		}

		void slow_text_printing(char *frase, int print_speed, int delay_continue, int enter){ // printea texto de forma lenta (en la duda entergarle a "enter" un 1)
			for(int i=0; i < strlen(frase); ++i){                                            // frase: lo que se imprimira lentamente
				putchar(frase[i]);                                                          // print_speed: que tan rapido se imprimira
				fflush(stdout);                                                            // delay_continue: tiempo que se demora en continuar la battalla
				delay(print_speed);                                                       // enter: si es 0 no continuara 1 espacio mas abajo,
			}                                                                            // si es 1 continuara un espacio mas abajo y si es 2 entonces esperara a que se presione enter
			if(enter == 2){
				char a;
				printf("*");
				scanf("%c", &a);// este scanf es para detener el programa y esperar un caracter
				return;
			}
			delay(delay_continue);
			if(enter) printf("\n");
		}

		void phrase_positioning(char* buffer, int positioning, int printing){ // printing == 0 para que se posicione para la impresion de slow_text_printing
			int spaces = floor(strlen(buffer)/2);
			for(int i=spaces; i < positioning; ++i) printf(" ");
			if(printing) printf("%s", buffer);
		}

		void hp_bar(int player){
			int i;
			float dos_porciento = ((float) pkmn[player].max_hp) / ((float) 50);
			int cant_asteriscos = floor(((float) pkmn[player].hp) /((float) dos_porciento));
			printf("[");
			if(pkmn[player].hp == pkmn[player].max_hp) for(i = 0; i < 50; ++i) printf("#");
			else{
				for(i = 0; i < cant_asteriscos; ++i) printf("#");
				for(i = cant_asteriscos; i < 50; ++i) printf("-");
			}
			if(cant_asteriscos == 0 && pkmn[player].hp > 0) printf("#");
			printf("]");
		}

		void clear_window_and_show_hp(int player){ // tambien manda los valores hp, pp y status[0] a sus respectivos lugares en team
			char buffer [30];
			system("clear");
			for(int i=0; i < 2; ++i){ // actualiza los datos del pokemon que esta en batalla al arreglo de estruct pokemon: Team
				if(pkmn[i].hp < 0) pkmn[i].hp = 0;
				team[i*pkmn_format].hp = pkmn[i].hp;
				if(pkmn[i].status[0] >= 10) team[i*pkmn_format].status[0] = 10;
				else team[i*pkmn_format].status[0] = pkmn[i].status[0];
				for(int j=0; j < 4; ++j) team[i*pkmn_format].move[j].pp = pkmn[i].move[j].pp;
			}
			hp_bar(0);
			if(player == 1) printf("                         >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>                       ");
			else            printf("                         <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<                       ");
			hp_bar(1);
			printf("\n");
		}

		int scanf_check(int lim_inf, int lim_sup){
			int variable;
			do{
				scanf("%d", &variable);
			} while(variable < lim_inf || variable > lim_sup);
			return variable;
		}

		double round_sp(double numero, int decimales){ // redondea al "decimales" decimal (si "decimales" = 0 devuelve el numero truncado)
			if(decimales == 0) return floor(numero);
			return (round(pow(10, decimales) * numero) / pow(10, decimales));
		}

		double rng(float success, int answer_number){ // devuelve un numero o una "respuesta" de forma random
			double n_rand = round_sp(((double) rand() / (double) RAND_MAX) * 101, 3);// success: probabilidad de que un evento ocurra
			if(n_rand > 100) n_rand = 100;                                          // answer_number: '0' devuelve una respuesta de si o no. 1 devuelve un numero random
			//printf(" rand(%.3f): %.3lf\n", success, n_rand); // revision123
			if(answer_number) return (n_rand); // dos utilidades, primera: retornar un numero rand double
			if(success >= n_rand) return 1; // segunda: retornar 1 o 0 en caso de verdadero y falso respectivamente(algo occure o no occure)
			return 0;
		}

		void rand_turn(int* r_turn){ // decide el orden de los turnos de forma random
			int first = rng(50, 0);
			for(int i=0; i < 2; ++i) r_turn[i] = fabs(first - i);
			//printf("%d, %d\n", r_turn[0], r_turn[1]); // revision123
		}

		void name_large(int large){
			for(int i=large; i < 18; ++i)
				printf(" ");
		}

		void struggle(int player){
			int i;
			strcpy(pkmn[player].move[0].name,"Struggle");
			pkmn[player].move[0].m_type=18;
			pkmn[player].move[0].m_power=50;
			pkmn[player].move[0].m_category=0;
			pkmn[player].move[0].pp=1;
			pkmn[player].move[0].max_pp=1;
			pkmn[player].move[0].accuracy=910;
			pkmn[player].move[0].sc[0]=0;
			pkmn[player].move[0].sc[1]=0;
			pkmn[player].move[0].sc_accuracy=0;
			for(i = 0; i < 7; ++i) pkmn[player].move[0].statch[0]=0;
			pkmn[player].move[0].statch_accuracy=0;
			for(i = 0; i < 11; ++i) pkmn[player].move[0].selfch[0]=0;
			pkmn[player].move[0].selfch[8]=25;
			pkmn[player].move[0].selfch_accuracy=100;
			pkmn[player].move[0].flinch_accuracy=0;
		}

	///// preparecion /////
		void format(){ // seleccion del formato de batalla
			char buffer[100];
		    do{
		        system("clear");
		        sprintf(buffer,"What battle format are you going to pick?\n");
		        phrase_positioning(buffer, 92, 1);
				for(int i=0; i < 6; ++i){
					sprintf(buffer,"%d vs %d\n", i+1, i+1);
					phrase_positioning(buffer, 92, 1);
				}
			    sprintf(buffer,"Random [7]\n");
			    phrase_positioning(buffer, 92, 1);
			    scanf("%d", &pkmn_format);
		    } while(pkmn_format<1||pkmn_format>7);
		    if(pkmn_format == 7) pkmn_format = 1 + rand() % 6;
		}

		void filtro(char* aux){ // quita los '_' por ' '(_ a espacio)
			for(int i=0; i < strlen(aux); ++i)
				if(aux[i] == '_') aux[i] = ' ';
		}

		void effectiveness(int pkmntype1, int pkmntype2, float* typeres_pkmn){ //tu le colocas los tipos de pokemon y un puntero para modificar su arreglo
			int i;
			float typeres_chart[19][19];
			FILE* chart = fopen("chart.txt","r+");
			for(i = 0; i < 19; ++i){ //later we will see individual cases
				fscanf(chart,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &typeres_chart[0][i], &typeres_chart[1][i], &typeres_chart[2][i], &typeres_chart[3][i],
					&typeres_chart[4][i], &typeres_chart[5][i], &typeres_chart[6][i], &typeres_chart[7][i], &typeres_chart[8][i], &typeres_chart[9][i], &typeres_chart[10][i], 
					&typeres_chart[11][i], &typeres_chart[12][i], &typeres_chart[13][i], &typeres_chart[14][i], &typeres_chart[15][i], &typeres_chart[16][i],
					&typeres_chart[17][i], &typeres_chart[18][i]);
			}
			for(i = 0; i < 19; ++i) typeres_pkmn[i] = typeres_chart[pkmntype1][i] * typeres_chart[pkmntype2][i];
			fclose(chart);
		}

		void pokedefault(Pokemon* pokemon){ // inicializa al pokemon con 0s en (caso) todo int
			int i;
			for(i = 0; i<9; ++i) pokemon[0].status[i] = 0;
			for(i = 0; i<7; ++i) pokemon[0].stat_change[i] = 0;
			pokemon[0].habilidad = 0;
			pokemon[0].max_hp = pokemon[0].hp;
			for(i = 0; i<4; ++i) pokemon[0].move[i].max_pp = pokemon[0].move[i].pp;
			effectiveness(pokemon[0].type1, pokemon[0].type2, pokemon[0].type_multiplier);
		}

		void name_selection(char name[2][30]){ // inicializa los nombres de los jugadores
			char buffer[100];
			for(int i=0; i < 2; ++i){
				sprintf(buffer,"Player %d:\n", i+1);
				phrase_positioning(buffer, 92, 1);
				sprintf(buffer,"What's your name?\n");
				phrase_positioning(buffer, 92, 1);
				printf("                                                                                   ");
				scanf("%[^\n]", name[i]);
				getchar();
			}
		}

		void pokemon_database(FILE* database, Pokemon* pokemon, int npkmn){ //extrae la informacion del archivo de texto que tiene almacenada la informacion de los pokemones
			int i, j;
			for(i = 0; i < npkmn; ++i){
				// leyendo los datos de database.txt
				fscanf(database,"%s", pokemon[i].name);
				fscanf(database,"%d %d", &pokemon[i].type1, &pokemon[i].type2);
				fscanf(database,"%d %d %d %d %d %d", &pokemon[i].hp, &pokemon[i].attackstat[0], &pokemon[i].defensestat[0], &pokemon[i].attackstat[1], &pokemon[i].defensestat[1], &pokemon[i].speed);
				for(j = 0; j < 4; ++j){
					fscanf(database,"%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", pokemon[i].move[j].name, &pokemon[i].move[j].m_type, &pokemon[i].move[j].m_power, &pokemon[i].move[j].m_category,
					&pokemon[i].move[j].pp, &pokemon[i].move[j].accuracy, &pokemon[i].move[j].sc[0], &pokemon[i].move[j].sc[1], &pokemon[i].move[j].sc_accuracy, &pokemon[i].move[j].statch[0],
					&pokemon[i].move[j].statch[1], &pokemon[i].move[j].statch[2], &pokemon[i].move[j].statch[3], &pokemon[i].move[j].statch[4], &pokemon[i].move[j].statch[5],
					&pokemon[i].move[j].statch[6], &pokemon[i].move[j].statch_accuracy, &pokemon[i].move[j].selfch[0],
					&pokemon[i].move[j].selfch[1], &pokemon[i].move[j].selfch[2], &pokemon[i].move[j].selfch[3], &pokemon[i].move[j].selfch[4], &pokemon[i].move[j].selfch[5],
					&pokemon[i].move[j].selfch[6], &pokemon[i].move[j].selfch[7], &pokemon[i].move[j].selfch[8], &pokemon[i].move[j].selfch[9], &pokemon[i].move[j].selfch[10], &pokemon[i].move[j].selfch_accuracy, &pokemon[i].move[j].flinch_accuracy);
					filtro(pokemon[i].move[j].name);
				}
				pokedefault(&pokemon[i]);
				filtro(pokemon[i].name);
				// revisando los datos de database.txt, ademas los imprime para que el usuario pueda revizar los pokemones
				/*for(j = 0; j < 4; ++j) filtro(pokemon[i].move[j].name);
				printf("%s\n", pokemon[i].name);
				printf("%d %d\n", pokemon[i].type1, pokemon[i].type2);
				printf("%d %d %d %d %d %d\n", pokemon[i].hp, pokemon[i].attackstat[0], pokemon[i].defensestat[0], pokemon[i].attackstat[1], pokemon[i].defensestat[1], pokemon[i].speed);
				for(j = 0; j < 4; ++j){
					printf("%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", pokemon[i].move[j].name, pokemon[i].move[j].m_type, pokemon[i].move[j].m_power, pokemon[i].move[j].m_category,
					pokemon[i].move[j].pp, pokemon[i].move[j].accuracy, pokemon[i].move[j].sc[0], pokemon[i].move[j].sc[1], pokemon[i].move[j].sc_accuracy, pokemon[i].move[j].statch[0],
					pokemon[i].move[j].statch[1], pokemon[i].move[j].statch[2], pokemon[i].move[j].statch[3], pokemon[i].move[j].statch[4], pokemon[i].move[j].statch[5],
					pokemon[i].move[j].statch[6], pokemon[i].move[j].statch_accuracy, pokemon[i].move[j].selfch[0],
					pokemon[i].move[j].selfch[1], pokemon[i].move[j].selfch[2], pokemon[i].move[j].selfch[3], pokemon[i].move[j].selfch[4], pokemon[i].move[j].selfch[5],
					pokemon[i].move[j].selfch[6], pokemon[i].move[j].selfch[7], pokemon[i].move[j].selfch[8], pokemon[i].move[j].selfch[9], pokemon[i].move[j].selfch[10], pokemon[i].move[j].selfch_accuracy, pokemon[i].move[j].flinch_accuracy);				
				}
				printf("\n");*/
			}
		}

		void pokemon_selection(char name[2][30], int* cPU1, int* cPU2){ // aqui se hace la seleccion de los pokemon para cada jugador (y el nombre de los jugadores)
			int i, j, k, l, o;
			char buffer[1000];
			FILE* database;
			database = fopen("database.txt","r+");
			int npkmn = 15; //numero de pokemones en la base de datos, si se agrega o se quita algun pokemon cambiar este numero
			Pokemon pokemones[npkmn]; // almacena los pokemones de la database
			pokemon_database(database, pokemones, npkmn);
			fclose(database);

			//seleccion de pokemon y de nombre
				format();
				printf("                                                                                   Player 1 is a CPU?\n                                                                                          no[1]\n                                                                                          yes[2]\n");
				*cPU1 = scanf_check(1,2) - 1;
        		printf("                                                                                   Player 2 is a CPU?\n                                                                                          no[1]\n                                                                                          yes[2]\n");
				*cPU2 = scanf_check(1,2) - 1;
				getchar();
				name_selection(name);
				for(i = 0; i < pkmn_format; ++i){ // pide el pokemon respectivo para player1 o player2
					char p[4];
					switch(i){
					case 0: strcpy(p, "1st"); break;
					case 1: strcpy(p, "2nd"); break;
					case 2: strcpy(p, "3rd"); break;
					case 3: strcpy(p, "4th"); break;
					case 4: strcpy(p, "5th"); break;
					case 5: strcpy(p, "6th"); break;
					}
					for(j = 0; j < 2; ++j){
						do{
							system("clear");
							sprintf(buffer,"%s:\n", name[j]);
							phrase_positioning(buffer, 92, 1);
							sprintf(buffer,"Choose your %s pokemon\n", p);
							phrase_positioning(buffer, 92, 1);
							for(k = 0; k < npkmn; ++k){
								printf("                                                                     ");
								for(l = 0; l < 3 && k < npkmn; ++l, ++k){
									sprintf(buffer,"%s[%d]", pokemones[k].name, k);
									printf("%s",buffer);
									name_large(strlen(buffer));
								}
								printf("\n");
								--k;
							}
							printf("                                                                     Random[%d]\n", npkmn);
							if(*cPU1 && j == 0) o = npkmn;
							else if(*cPU2 && j == 1) o = npkmn;
							else scanf("%d", &o);
						} while(o < 0 || o > npkmn);
						if(o < npkmn) team[i+(pkmn_format*j)] = pokemones[o]; // coloca los pokemones en el team ([0,npkmn-1])
						else team[i+(pkmn_format*j)] = pokemones[(1 + rand()%(npkmn - 1))];
					}
				}
				pkmn[0] = team[0]; // informacion que se usara en la batalla pokemon
				pkmn[1] = team[pkmn_format];
		}

	// player es el pokemon que esta actuando
	///// damage calculation /////
		double statch_multiplier(int player, int stat_checking){ // calcula la precision y la evasion del pkmn que ataca y del que resive el ataque (respectivamente)
			//printf("statch_multiplier of %s is:%d\n",pkmn[player].name,pkmn[player].stat_change[stat_checking]);
			if(stat_checking == 6){ // si es 6 entonces se esta pidiendo la accuracy del pkmn que esta atacando
				if(pkmn[player].stat_change[stat_checking] >= 0) return ((3 + (double) pkmn[player].stat_change[stat_checking]) / 3);
				return(3 / (3+ fabs((double) pkmn[player].stat_change[stat_checking])));
			} // si no, entonces se esta buscando la evasion del pkmn que esta resiviendo el ataque
			if(stat_checking == 5){
				if(pkmn[1 - player].stat_change[stat_checking] >= 0) return ((3 + (double) pkmn[1 - player].stat_change[stat_checking]) / 3);
				return(3 / (3+ fabs((double) pkmn[1 - player].stat_change[stat_checking])));
			}
			if(pkmn[player].stat_change[stat_checking] >= 0) return ((2 + (double) pkmn[player].stat_change[stat_checking]) / 2);
			return(2 / (2+ fabs((double) pkmn[player].stat_change[stat_checking])));
		}

		double burn(int player, int move_category){ // si el pkmn esta quemado y el ataque utilizado por el es de tipo fisico entonces el ataque se reduce en 50%
			if(pkmn[player].status[0] == 5 && move_category) return 0.5;
			return 1;
		}

		double type_effectiveness(int player, int move_selected){ // la efectividad de tipo (si el ataque es de agua y el oponente es de fuego entonces x2 al ataque... eso)
			if(player) return pkmn[0].type_multiplier[pkmn[1].move[move_selected].m_type];
			return pkmn[1].type_multiplier[pkmn[0].move[move_selected].m_type];
		}

		double stab(int player, int move_selected){ // si el movimiento del pokemon atacante es del mismo tipo que alguno de los posibles tipos del pokemon atacante, entonces se aplica stab
			if(pkmn[player].type1 == pkmn[player].move[move_selected].m_type || pkmn[player].type2 == pkmn[player].move[move_selected].m_type) return 1.5; 
			return 1;
		}

		double crit(int** crit_checking){ // deside si es critico //posibilidad de 1/16 (6.25%). El critico es un multiplicador de 1.5 si ocurre. Si no, entonces es 1
			if(rng(6.25, 0)){
				**crit_checking = 1;
				return 1.5;
			}
			return 1;
		}

		int confuse_damage(int player){ // entrega el danyo final hecho por el ataque
			//printf("Random (confuse) multiplier"); // revision123
			double randomMultiplier = (double) (85 + (int) rng(100, 1) % 16) / 100;
			double burnMultiplier = burn(player, 0);
			double MODIFIER = randomMultiplier * burnMultiplier;
			double movebasedamage = 40;
			double playerAttackstat = pkmn[player].attackstat[0] * statch_multiplier(player, 0);
			double targetDefensestat = pkmn[player].defensestat[0] * statch_multiplier(player, 1);
			double attackVSdefensestats = playerAttackstat / targetDefensestat;
			double DAMAGE = (((((2 * 100) / 5) + 2) * movebasedamage * attackVSdefensestats) / 50 + 2) * MODIFIER;
			return DAMAGE;
		}

		int attack(int player, int move_selected, double typeEfectivenessMultiplier, int* crit_checking){ // entrega el danyo final hecho por el ataque
			int category = pkmn[player].move[move_selected].m_category;
			//printf("Random multiplier");  // revision123
			double randomMultiplier = (double) (85 + (int) rng(100, 1) % 16) / 100;
			//printf("Random multiplier: %.3lf\n", randomMultiplier);  // revision123
			//printf("Crit"); // revision123
			double critMultiplier = crit(&crit_checking);
			//printf("Crit multiplier: %.3lf\n", critMultiplier); // revision123
			double stabMultiplier = stab(player, move_selected);
			//printf("Stab multiplier: %.3lf\n", stabMultiplier); // revision123
			//printf("Type effectiveness multiplier: %.2lf\n", typeEfectivenessMultiplier); // revision123
			double burnMultiplier = burn(player, category);
			//printf("Burn multiplier: %.3lf\n", burnMultiplier); // revision123
			double MODIFIER = randomMultiplier * critMultiplier * stabMultiplier * typeEfectivenessMultiplier * burnMultiplier;
			//printf("modifier: %.3lf\n", MODIFIER); // revision123
			double movebasedamage = pkmn[player].move[move_selected].m_power;
			//printf("Move base damage: %.3lf\n", movebasedamage); // revision123
			double playerAttackstat = (pkmn[player].attackstat[category] * statch_multiplier(player, category*2)); // Revisa si el ataque es fisico o no para ver que stat usar
			//printf("player attack stat: %.3lf\n", playerAttackstat); // revision123
			double targetDefensestat = (pkmn[1 - player].defensestat[category] * statch_multiplier(1 - player, category*2 + 1)); // Revisa si el ataque es fisico o no para ver que stat usar
			//printf("Target defense stat: %.3lf\n", targetDefensestat); // revision123
			double attackVSdefensestats = playerAttackstat / targetDefensestat;
			//printf("attack / defense: %.3lf\n", attackVSdefensestats); // revision123
			double DAMAGE = (((((2 * 100) / 5) + 2) * movebasedamage * attackVSdefensestats) / 50 + 2) * MODIFIER; // Damage Formula
			//printf("damage: %.3lf\n", floor(DAMAGE)); // revision123
			return DAMAGE;
		}

		int receiving_damage(int player,int damage){
			if(player) pkmn[0].hp -= damage;
			else pkmn[1].hp -= damage;
		}

	///// checking's /////
		int team_hp_checking(int player){ // revisa si hay algun pkmn en el equipo al que aun le quede hp
			int fainted = pkmn_format;
			for(int i=0; i < pkmn_format; ++i)
				if(team[(player*pkmn_format) + i].hp == 0) fainted--;
			if(fainted) return 1; //caso en que aun queden pokemones vivos
			return 0;
		}

		int fainting_switch(int player, int cPU1, int cPU2){
			int i, positioning;
			char buffer[200];
			if(player) positioning = 160;
			else positioning = 24;
			do{
				if(cPU1 && player == 0) i = 2 + rand() % (pkmn_format - 1);
				else if(cPU2 && player == 1) i = 2 + rand() % (pkmn_format - 1);
				else{
					do{
						phrase_positioning("choose another Pokemon:\n", positioning, 1);
						for(i = 0; i < pkmn_format; ++i){
							sprintf(buffer,"%s %d / %d\n", team[i+(player*pkmn_format)].name, team[i+(player*pkmn_format)].hp, team[i+(player*pkmn_format)].max_hp);
							phrase_positioning(buffer, positioning, 1);
						}
						scanf("%d", &i);
						if(team[(i + (player*pkmn_format)) - 1].hp == 0){
							phrase_positioning("That pokemon doesn't have enough hp to go to battle", 92, 0);
							slow_text_printing("That pokemon doesn't have enough hp to go to battle", 46, 100, 1);
							i = 0;
						}
					} while(i < 1 || i > pkmn_format);
				}
			} while(team[(i + (player*pkmn_format)) - 1].hp == 0); // este es para asegurarse de que el pokemon que la cPU saque, este vivo
			return (i+3);
		}

		int fainting_cheking(char name[2][30], int player, int cPU1, int cPU2){ // revisa si cualquiera de los dos pokemon se desmayo y los manda al switch de pokemon
			char buffer[50];
			clear_window_and_show_hp(player);
			//printf("fainted"); // revision123
			sprintf(buffer, "%s's %s fainted!", name[player], pkmn[player].name);
			phrase_positioning(buffer, 92, 0);
			slow_text_printing(buffer, 46, 500, 1);
			if(team_hp_checking(player)) return fainting_switch(player, cPU1, cPU2);
			return 0;
		}

		int hit_checking(int player, int move_selected, int target){ // calcula la probabilidad de acertar un movimiento y despues decide si ese movimiento acertara o no
			double chance_of_hiting = (double) pkmn[player].move[move_selected].accuracy * (statch_multiplier(player, 6) / statch_multiplier(1 - player, 5));
			//printf("hit"); // revision123
			return rng(chance_of_hiting, 0);
		}

		int PAR_speed(int player){ // decide si el pkmn esta o no paralisado (para decidir si pierde o no velocidad)
			if(pkmn[player].status[0] == 7) return 2;
			return 1;
		}

		int volatile_status(int player){ // estados como confusion y enamoramiento van aqui
			int go = 1;
			char buffer[50];

			if(pkmn[player].status[1] >= 1){ // confused checking
				pkmn[player].status[1]--;

				if(pkmn[player].status[1] != 0) go = 0; // compruebo si sigo confundido
				//printf("confuse"); // revision123

				if(go == 0 && rng(50, 0) == 0) go = 1; // si sigo confundido entonces se comprueba si el pokemon se golpeara a si mismo o si atacara

				sprintf(buffer,"%s is confused!", pkmn[player].name);
				phrase_positioning(buffer, 92, 0); 
				slow_text_printing(buffer, 46, 3000, 1);

				if(go && pkmn[player].status[1] != 0) return 1; // si sigue confundido pero no se golpeo a si mismo

				if(go){
					sprintf(buffer,"%s's confusion is gone!", pkmn[player].name);
					phrase_positioning(buffer, 92, 0);
					slow_text_printing(buffer, 46, 1500, 1);
					return 1;
				}

				if(go == 0){
					phrase_positioning("It's so confused that it hurt itself!", 92, 0);
					slow_text_printing("It's so confused that it hurt itself!", 46, 1000, 1);
					pkmn[player].hp -= confuse_damage(player); // calculo del danyo a si mismo
				}
				return 0;
			}

			return 1;
		}

		int non_volatile_status(int player){ // efectos que perduran con el switch del pkmn (pueden evitar que ataques)
			int go = 1;
			char buffer[50];

			if(pkmn[player].status[0] >= 1 && pkmn[player].status[0] <= 4){ // sleep checking
				pkmn[player].status[0]--;

				if(pkmn[player].status[0] != 0) go = 0;
				sprintf(buffer,"%s is sleeping", pkmn[player].name);
				phrase_positioning(buffer, 92, 0);
				slow_text_printing(buffer, 46, 0, 1);
				phrase_positioning("Z z Z", 92, 0);
				slow_text_printing("Z z Z", 720, 1000, 1);
				phrase_positioning("AND HE IS!!!", 92, 0);
				slow_text_printing("AND HE IS!!!", 46, 700, 1);

				if(go){
					phrase_positioning("FINALLY WAKING UP!", 92, 0);
					slow_text_printing("FINALLY WAKING UP!", 46, 500, 1);
					return 1;
				}
				phrase_positioning("Z z Z", 92, 0);
				slow_text_printing("Z z Z", 720, 200, 1);
				phrase_positioning("still sleeping", 92, 0);
				slow_text_printing("still sleeping", 65, 0, 0);
				slow_text_printing("...", 200, 500, 1);
				return 0;
			}

			if(pkmn[player].status[0] == 6){ // frozen checking
				sprintf(buffer,"%s it's frozen...", pkmn[player].name);
				phrase_positioning(buffer, 92, 0);
				slow_text_printing(buffer, 46, 1000, 1);
				//printf("frozen"); // revision123

				if(rng(20, 0)){
					phrase_positioning("BUT FOR PURE FORCE OF ANGER HE'S NO LONGER FROZEN!!!", 92, 0);
					slow_text_printing("BUT FOR PURE FORCE OF ANGER HE'S NO LONGER FROZEN!!!", 46, 1000, 1);
					return 1;
				}
				phrase_positioning("and it can't attack...", 92, 0);
				slow_text_printing("and it can't attack...", 46, 500, 1);
				return(0);
			}

			if(pkmn[player].status[0] == 7){ // paralisis checking
				//printf("paralisis"); // revision123

				if(rng(25, 0)){
					sprintf(buffer,"%s's fully paralyzed!", pkmn[player].name);
					phrase_positioning(buffer, 92, 0);
					slow_text_printing(buffer, 46, 600, 1);
					return 0;
				}
			}
			return 1;
		}

        int flinching(int player){
            char buffer[50];
            sprintf(buffer,"%s´s flinched and couldn´t move",pkmn[player].name);
            phrase_positioning(buffer, 92, 0);
            slow_text_printing(buffer, 46, 600, 1);
            return 0;
        }
    
        int hit_or_miss(int player, int move_selected){ // revisa si el ataque va a fallar o no. I guess they never miss HUH?
	    	int go = 1;                                  // Tambien lleva a cabo cosas como determinar si siertos estados
        	if(flinch) go = flinching(player);          // alterados van a continuar o no. Y calcula el danyo probocado por confucion.
	   		if(go) go = non_volatile_status(player);
	   		if(go) go = volatile_status(player);
	   		return go;
	    }

		void speed_checking(int* t_order){ // decide el orden de los turnos
			if((pkmn[0].speed * statch_multiplier(0,4) / PAR_speed(0)) > (pkmn[1].speed * statch_multiplier(1,4) / PAR_speed(1))) t_order[0] = 0, t_order[1] = 1;
			else t_order[0] = 1, t_order[1] = 0;
			if((pkmn[0].speed * statch_multiplier(0,4)) == (pkmn[1].speed * statch_multiplier(0,4))){
				int turn[2]; // arreglo aux para decidir quien va primero en caso de que las velocidad sean iguales
				//printf("same speed"); // revision123
				rand_turn(turn); // decide de manera random quien va primero y quien va segundo
				for(int i=0; i < 2; ++i) t_order[i] = turn[i];
			}
		}
		
		void statch_checking(int player, int move_selected){
			char buffer[50];
			char StatChanged[20];
			int aux, auxStat, auxStageChange;
			//statch[7]: [0] attack, [1] defense, [2] spattack, [3] spdefense, [4] speed, [5] evasion, [6] accuracy
			for(int i=0; i < 7; ++i){
				switch(i){
					case 0: strcpy(StatChanged, "attack"); break;
					case 1: strcpy(StatChanged, "defense"); break;
					case 2: strcpy(StatChanged, "special attack"); break;
					case 3: strcpy(StatChanged, "special defense"); break;
					case 4: strcpy(StatChanged, "speed"); break;
					case 5: strcpy(StatChanged, "evasion"); break;
					case 6: strcpy(StatChanged, "accuracy"); break;
				}
				if(pkmn[player].move[move_selected].statch[i] != 0){ // revisar si el movimiento puede variar las stages
					auxStageChange = pkmn[player].move[move_selected].statch[i]; // guarda la cantidad de stages que se aumentaran/disminuiran
					auxStat = pkmn[1-player].stat_change[i]; // guarda el valor que la stat tiene actualmente
					aux = auxStat; // hace una copia de lo anterior
					auxStat += auxStageChange; // le suma/resta las stages al aux que guarda las stats actuales

					if(auxStat < -6) auxStat = -6; // si es menor a -6 lo iguala a -6
					if(auxStat > 6) auxStat = 6; // si es mayor a 6 lo iguala a 6

					pkmn[1-player].stat_change[i] = auxStat; // le entrega su nuevo valor de stats al pokemon
					if(auxStageChange < 0){ // revisa si el cambio de stage es negativo (si entra, entonces lo es)

						if(auxStat == aux) sprintf(buffer,"%s's %s can't be lower",pkmn[1-player].name, StatChanged); // si auxStat es igual a aux (aun despues de la resta de stages) entonces no hubo cambio
						else sprintf(buffer,"%s´s %s has decreased", pkmn[1-player].name, StatChanged); // si no fue lo anterior entonces la stat debio haber disminuido
					}

					else{ // si entra aqui entonces el cambio de stage debe ser positivo
						if(auxStat == aux) sprintf(buffer,"%s's %s can't increase anymore!",pkmn[1-player].name, StatChanged); // igual que en el "if" anterior pero para stages positivas
						else sprintf(buffer,"%s´s %s has increased", pkmn[1-player].name, StatChanged); // igual que el "else" anterior pero para el aumento de la stat
					}
					phrase_positioning(buffer, 92, 0);
					slow_text_printing(buffer, 46, 1000, 1); // printea uno de los 4 casos que haya sido
				}
			}
		}

		void selfch_checking(int player, int move_selected,int damage){
			char buffer[50];
			char StatChanged[20];
			int aux, auxStat, auxStageChange;
			//selfch[11]: [0] attack, [1] defense, [2] spattack, [3] spdefense, [4] speed, [5] evasion, [6] accuracy [7]recovery [8]recoil [9]selfsc[0] [10]selfsc[1]

			for(int i=0; i < 7; ++i){
				switch(i){
					case 0: strcpy(StatChanged, "attack"); break;
					case 1: strcpy(StatChanged, "defense"); break;
					case 2: strcpy(StatChanged, "special attack"); break;
					case 3: strcpy(StatChanged, "special defense"); break;
					case 4: strcpy(StatChanged, "speed"); break;
					case 5: strcpy(StatChanged, "evasion"); break;
					case 6: strcpy(StatChanged, "accuracy"); break;
				}
				if(pkmn[player].move[move_selected].selfch[i] != 0){ // revisar si el movimiento puede variar las stages
					auxStageChange = pkmn[player].move[move_selected].selfch[i]; // guarda la cantidad de stages que se aumentaran/disminuiran
					auxStat = pkmn[player].stat_change[i]; // guarda el valor que la stat tiene actualmente
					aux = auxStat; // hace una copia de lo anterior
					auxStat += auxStageChange; // le suma/resta las stages al aux que guarda las stats actuales

					if(auxStat < -6) auxStat = -6; // si es menor a -6 lo iguala a -6
					if(auxStat > 6) auxStat = 6; // si es mayor a 6 lo iguala a 6

					pkmn[player].stat_change[i] = auxStat; // le entrega su nuevo valor de stats al pokemon
					if(auxStageChange < 0){ // revisa si el cambio de stage es negativo (si entra, entonces lo es)

						if(auxStat == aux) sprintf(buffer,"%s's %s can't be lower",pkmn[player].name, StatChanged); // si auxStat es igual a aux (aun despues de la resta de stages) entonces no hubo cambio
						else sprintf(buffer,"%s´s %s has decreased", pkmn[player].name, StatChanged); // si no fue lo anterior entonces la stat debio haber disminuido
					}

					else{ // si entra aqui entonces el cambio de stage debe ser positivo
						if(auxStat == aux) sprintf(buffer,"%s's %s can't increase anymore!",pkmn[player].name, StatChanged); // igual que en el "if" anterior pero para stages positivas
						else sprintf(buffer,"%s´s %s has increased", pkmn[player].name, StatChanged); // igual que el "else" anterior pero para el aumento de la stat
					}
					phrase_positioning(buffer, 92, 0);
					slow_text_printing(buffer, 46, 1000, 1); // printea uno de los 4 casos que haya sido
				}
			}

			if(pkmn[player].move[move_selected].selfch[7]!=0){ // revisar si hay variacion de vida
				if(pkmn[player].move[move_selected].selfch[7] == 1){ // recupera la mitad de la vida
					pkmn[player].hp+=(pkmn[player].max_hp/2);// recupera la mitad de la vida
					if(pkmn[player].hp>pkmn[player].max_hp) pkmn[player].hp = pkmn[player].max_hp; // si la vida que se recupero pasa la vida maxima, entonces reducirla a la vida maxima
				}
				if(pkmn[player].move[move_selected].selfch[7] == 2) pkmn[player].hp = pkmn[player].max_hp; // el pokemon recupera toda su vida
				sprintf(buffer,"%s regained health",pkmn[player].name); // se imprime el texto
				phrase_positioning(buffer, 92, 0);
				slow_text_printing(buffer, 46, 700, 1);
			}

			if(pkmn[player].move[move_selected].selfch[8]!=0){ // recoil damage
				pkmn[player].hp -= (damage * ((float) pkmn[player].move[move_selected].selfch[8] / 100));
				sprintf(buffer,"%s recieved recoil damage",pkmn[player].name);
				phrase_positioning(buffer, 92, 0);
				slow_text_printing(buffer, 46, 700, 1);
			}

			if(pkmn[player].move[move_selected].selfch[9]!=0){ // estado no volatil
				int sc_effect=pkmn[player].move[move_selected].selfch[9];
				if(sc_effect <= 3){
					sprintf(buffer,"%s its now sleeping!", pkmn[player].name);
					sc_effect = 2 + rand() % 2;
				}
				if(sc_effect == 4){
					sprintf(buffer,"%s its now sleeping!", pkmn[player].name);
					sc_effect = 4;
				}
				if(sc_effect == 5) sprintf(buffer,"%s its burning!", pkmn[player].name);
				if(sc_effect == 6) sprintf(buffer,"%s its frozen!", pkmn[player].name);
				if(sc_effect == 7) sprintf(buffer,"%s its fully paralyzed!", pkmn[player].name);
				if(sc_effect == 8) sprintf(buffer,"%s got poisoned!", pkmn[player].name);
				if(sc_effect == 10) sprintf(buffer,"%s got badly poisoned!", pkmn[player].name);
				phrase_positioning(buffer, 92, 0);
				slow_text_printing(buffer, 46, 700, 1);
				pkmn[player].status[0] = sc_effect; // el pokemon recibe el status condition
			}

			if(pkmn[player].move[move_selected].selfch[10]!=0){ // estado volatil
				int sc_effect=pkmn[player].move[move_selected].selfch[10];
				if(sc_effect == 1 && pkmn[player].status[1] == 0){ // para el estado "confuse"
					pkmn[player].status[1] = 2 + rand() % 3; // calculo de la duracion del status
					sprintf(buffer,"%s its confused!", pkmn[player].name);
				}
				if(sc_effect == 2 && pkmn[player].status[2] == 0){ // "bound" quita 1/8 de la vida maxima al pokemon sufriendo el efecto
					sprintf(buffer,"%s got bounded!", pkmn[player].name);
					pkmn[player].status[2] = 5 + rand() % 1; // calculo de la duracion del status
				}
				if(sc_effect == 3 && pkmn[player].status[3] == 0){ // "leech seed" quita 1/8 de la vida maxima al pokemon sufriendo el efecto y le entrega esa misma cantidad de puntos
					sprintf(buffer,"%s it's being trapped by ominous vines.", pkmn[player].name); 
					pkmn[player].status[3] = 1;
				}
				phrase_positioning(buffer, 92, 0);
				slow_text_printing(buffer, 46, 500, 1);
			}
		}

		void sc_hit_checking(int player, int move_selected, int target){
			int sc_effect = pkmn[player].move[move_selected].sc[0];
			char buffer[50];

			// non volatil // se podria usar switch
			if(sc_effect != 0 && pkmn[target].status[0] == 0){ // mensaje de cuando obtiene un status[0] condition
				if(sc_effect <= 4){
					sprintf(buffer,"%s its now sleeping!", pkmn[target].name);
					sc_effect = 2 + rand() % 2;
				}
				if(sc_effect == 5) sprintf(buffer,"%s its burning!", pkmn[target].name);
				if(sc_effect == 6) sprintf(buffer,"%s its frozen!", pkmn[target].name);
				if(sc_effect == 7) sprintf(buffer,"%s its fully paralyzed!", pkmn[target].name);
				if(sc_effect == 8) sprintf(buffer,"%s got poisoned!", pkmn[target].name);
				if(sc_effect == 10) sprintf(buffer,"%s got badly poisoned!", pkmn[target].name);
				phrase_positioning(buffer, 92, 0);
				slow_text_printing(buffer, 46, 700, 1);
				pkmn[target].status[0] = sc_effect;
				return;
			}

			// volatil
			sc_effect = pkmn[player].move[move_selected].sc[1];
			if(sc_effect >= 1){
				if(sc_effect == 1 && pkmn[target].status[1] == 0){ // para el estado "confuse"
					pkmn[target].status[1] = 2 + rand() % 3; // calculo de la duracion del status
					sprintf(buffer,"%s its confused!", pkmn[target].name);
				}
				if(sc_effect == 2 && pkmn[target].status[2] == 0){ // "bound" quita 1/8 de la vida maxima al pokemon sufriendo el efecto
					sprintf(buffer,"%s got bounded!", pkmn[target].name);
					pkmn[target].status[2] = 5 + rand() % 1; // calculo de la duracion del status
				}
				if(sc_effect == 3 && pkmn[target].status[3] == 0){ // "leech seed" quita 1/8 de la vida maxima al pokemon sufriendo el efecto y le entrega esa misma cantidad de puntos
					sprintf(buffer,"%s it's being trapped by ominous vines.", pkmn[target].name); // al oponente de este
					pkmn[target].status[3] = 1;
				}
				phrase_positioning(buffer, 92, 0);
				slow_text_printing(buffer, 46, 500, 1);
				//si se implementan mas estados volatiles, ellos se pueden codear aqui usando el de "confundido" como plantilla
			}
		}

		int pp_checking(int player){
			int no_pp = 4;
			char buffer[50];
			for(int i=0; i < 4; ++i) if(pkmn[player].move[i].pp == 0) no_pp--;
			if(no_pp) return 0;
			sprintf(buffer,"%s is out of PP",pkmn[player].name);
			phrase_positioning(buffer, 92, 0);
			slow_text_printing(buffer, 46, 1000, 1);
			return 1;
		}

	///// fase de batalla /////
		void combat(int player, int move_selected, int target){ // cuando ambos jugadores escogieron su ataque ambos entran aqui y toman turnos para atacar
			int crit_checking = 0, damage = 0, go;             // se hace tambien el checking de si el pokemon va o no va a atacar
			double effectiveness_checking = 1;                // Los "int"'s al principio son para mandar mensajes espesificos en pantalla (Ej.: It was a critical hit!)
			char buffer[50];
			clear_window_and_show_hp(player);
			go = hit_or_miss(player, move_selected); // si go es 1 el pokemon podra atacar (pero aun esta la posibilidad de que falle el ataque)
			//printf("go: %d\n", go); // revision123

			if(go){
				pkmn[player].move[move_selected].pp--;
				sprintf(buffer,"%s used %s!", pkmn[player].name, pkmn[player].move[move_selected].name);
				phrase_positioning(buffer, 92, 0);
				slow_text_printing(buffer, 46, 1000, 1);
                
				if(pkmn[player].move[move_selected].m_category != 3){
					if(hit_checking(player, move_selected, target)){ // aqui se aplican los efectos del ataque
						effectiveness_checking = type_effectiveness(player, move_selected);
						damage = attack(player, move_selected, effectiveness_checking, &crit_checking); // CALCULO DE DAÑO
						
						if(rng(pkmn[player].move[move_selected].flinch_accuracy,0)) flinch = 1; // chequeo de la probabilidad de flinch
						
						if(pkmn[player].move[move_selected].m_category == 2 && effectiveness_checking != 0){
							damage = 0;
							effectiveness_checking = 1;
							crit_checking = 0;
						}



						switch((int) (effectiveness_checking * 100)){
							case 0: sprintf(buffer,"But is completely uneffective . . .\n"); return;
							case 25: sprintf(buffer,"But its effectiveness is close to zero . . .\n"); break;
							case 50: sprintf(buffer,"But it is not very effective . . .\n"); break;
							case 200: sprintf(buffer,"It's super effective!\n"); break;
							case 400: sprintf(buffer,"It was outstandingly effective!\n"); break;
						}
						if(effectiveness_checking != 1){
							phrase_positioning(buffer, 92, 0);
							slow_text_printing(buffer, 46, 1000, 1);
						}
						if(crit_checking){
							phrase_positioning("Oh damn! It was a crit! RNGsus is on your side my dude!!", 92, 0);
							slow_text_printing("Oh damn! It was a crit! RNGsus is on your side my dude!!", 46, 1500, 1);
						}

						receiving_damage(player, damage);
						
						if(rng(pkmn[player].move[move_selected].selfch_accuracy,0)) selfch_checking(player, move_selected, damage);
						
						if(rng(pkmn[player].move[move_selected].statch_accuracy,0)) statch_checking(player, move_selected);
						
						if(pkmn[target].hp <= 0) return;

						if(pkmn[player].move[move_selected].sc_accuracy != 0){ //after_damage_effects() funcion para optimizar la lectura del codigo
							//printf("status condition"); // revision123
							if(rng(pkmn[player].move[move_selected].sc_accuracy, 0)) sc_hit_checking(player, move_selected, target); // rng es para ver si esque sc achunta
						}

						if(pkmn[player].move[move_selected].m_type == 9 && pkmn[target].status[0] == 6){ // si el ataque es tipo fuego y el pokemon que recibe el daño esta congelado:
							pkmn[target].status[0] = 0;
							sprintf(buffer,"%s is no longer frozen!",pkmn[target].name);
							phrase_positioning(buffer, 92, 0);
							slow_text_printing(buffer, 46, 1500, 1);
						}

					return;
					}
				phrase_positioning("YOU MISSED!!! HOW COULD YOU MISSED! IT WAS ONE METER AWAY FROM YOUUUUUUUU...", 92, 0);
				slow_text_printing("YOU MISSED!!! HOW COULD YOU MISSED! IT WAS ONE METER AWAY FROM YOUUUUUUUU...", 46, 500, 1);
				return;
				}

			selfch_checking(player, move_selected, damage);
			}
		}

		void pokemon_switching(int player, int option, int alive){
			clear_window_and_show_hp(player);
			char buffer[200];
			if(alive){
				sprintf(buffer, "%s come back!", pkmn[player].name);
				phrase_positioning(buffer, 92, 0);
				slow_text_printing(buffer, 46, 2000, 1);
			}
			player *= pkmn_format; // player = 0 -> 0. player = 1 -> pkmn_format. Esto es para poder buscar al pokemon en la 1era posicion de cada equipo
			option -= 4; // Esto es para buscar al pokemon con el que se va a ejecutar el swaping
			Pokemon swap =  team[player]; // swap copia al 1er pokemon de team para poder hacer el swaping
			team[player] = team[option + player]; // el swap dentro del team ocurre
			team[option + player] = swap; // el espacio en team del pokemon que va a entrar ahora, es tomado por el pokemon que ahora esta saliendo del combate
			player /= pkmn_format;
			pkmn[player] = team[player*pkmn_format];
			phrase_positioning("It's your time to shine... ", 92, 0);
			slow_text_printing("It's your time to shine... ", 46, 1000, 1); 
			sprintf(buffer,"%s!", pkmn[player].name);
			phrase_positioning(buffer, 92, 0);
			slow_text_printing(buffer, 46, 1000, 1);
		}

		void status_checking_end(char name[2][30], int cPU1, int cPU2){ // chequeo de efectos de final de turno
			char buffer[100];
			for(int i=0; i < 2; ++i){
				if(pkmn[i].status[0] == 5 || pkmn[i].status[0] >= 8){
					if(pkmn[i].status[0] == 5){ // chequeo para burn, poison y badly poison
						pkmn[i].hp -= (int) ((float) pkmn[i].max_hp * 1/8);
						sprintf(buffer,"%s is suffering from its burns", pkmn[i].name);
					}
					if(pkmn[i].status[0] >= 8){ // comprueba poison
						if(pkmn[i].status[0] >= 10) pkmn[i].status[0]++; // si b_poison, entonces se aumenta el contador
						pkmn[i].hp -= (int) ((float) (pkmn[i].max_hp * ((float) (fabs(pkmn[i].status[0]-10)/16))));
						sprintf(buffer,"%s is taking damage from the poison!", pkmn[i].name);
					}
					phrase_positioning(buffer, 92, 0);
					slow_text_printing(buffer, 46, 1000, 1);
					clear_window_and_show_hp(2);
					if(pkmn[i].hp == 0){
						int F_Cheking = fainting_cheking(name, i, cPU1, cPU2);
						if(F_Cheking){ // Si F_Cheking es mayor a 0 significa que el 
							pokemon_switching(i, F_Cheking, 0); // si hay alguien con vida entonces se va a ejecutar un switch
						}
					}
				}
				if(pkmn[i].status[3] == 1){ // efecto de leech seed
					int Leech = (int) ((float) pkmn[i].max_hp * 1/8);
					pkmn[i].hp -= Leech;
					sprintf(buffer,"%s is losing life because of Leech Seed.", pkmn[i].name);
					phrase_positioning(buffer, 92, 0);
					slow_text_printing(buffer, 46, 1000, 1);
					pkmn[1-i].hp += Leech;
					if(pkmn[1-i].hp > pkmn[1-i].max_hp) pkmn[1-i].hp = pkmn[1-i].max_hp;
					clear_window_and_show_hp(2);
					if(pkmn[i].hp == 0){
						int F_Cheking = fainting_cheking(name, i, cPU1, cPU2);
						if(F_Cheking){ // Si F_Cheking es mayor a 0 significa que el 
							pokemon_switching(i, F_Cheking, 0); // si hay alguien con vida entonces se va a ejecutar un switch
						}
					}
				}
			}
			Current_Turn++;
		}

		void action(int* option, char name[2][30], int cPU1, int cPU2){ // la accion que cada entrenador ejecutara en base a la opcion seleccionada
			int t_order[2], i;
			speed_checking(t_order);
			// revisa si alguno de los entrenadores decidio hacer un switch de pokemon. Acto seguido, ejecuta el switch
			for(i = 0; i < 2; ++i) if(option[t_order[i]] >= 4 && option[t_order[i]] <= 9) pokemon_switching(t_order[i], option[t_order[i]], 1);

			// revisa si alguno de los entrenadores decidio atacar. Acto seguido, ejecuta el ataque
			for(i = 0; i < 2; ++i){
				if(pkmn[t_order[i]].hp > 0){
					if(option[t_order[i]] >= 0 && option[t_order[i]] <= 3) combat(t_order[i], option[t_order[i]], t_order[1-i]);
				}
			}

			clear_window_and_show_hp(2);

			for(i = 0; i < 2; ++i){
				if(pkmn[i].hp == 0){
					int F_Cheking = fainting_cheking(name, i, cPU1, cPU2); // aqui, en el segundo parametro de la funcion, estaba t_order[i] (lo cambie por i). t_order se usa para revisar quien es el mas rapido. Pero no queremos al mas rapido, queremos al que se desmayo.
					if(F_Cheking){ // Si F_Cheking es mayor a 0 significa que el 
						pokemon_switching(i, F_Cheking, 0); // si hay alguien con vida entonces se va a ejecutar un switch (aqui tambien cambie t_order[i])
					}
				}
			}
		}

	///// menus /////
		int RUN(int option){
			option = 0;
			clear_window_and_show_hp(2);
			phrase_positioning("No!", 92, 0);
			slow_text_printing("No!",46, 600, 1);
			phrase_positioning("You want to run? Go back to battle you chicken!", 92, 0);
			slow_text_printing("You want to run? Go back to battle you chicken!",46, 1000, 1);
			return option;
		}

		int pokemon_swap_menu(int player, int cPU1, int cPU2){
			int option;
			int positioning;
			char buffer[200];
			if(player) positioning = 160;
			else positioning = 24;
			clear_window_and_show_hp(player);
			for(int i=0; i < pkmn_format; ++i){
				sprintf(buffer,"%s %d / %d\n", team[i+(player*pkmn_format)].name, team[i+(player*pkmn_format)].hp, team[i+(player*pkmn_format)].max_hp);
				phrase_positioning(buffer, positioning, 1);
			}
			if((cPU1 && player == 0) || (cPU2 && player == 1)) delay(2000);
			do{
				if(cPU1 && player == 0) option = 1 + rand() % pkmn_format; // si el total es 1 entonces vuelve al menu_0 /////////////REVISA ESTOOOOOOOOOOOOO
				else if(cPU2 && player == 1) option = 1 + rand() % pkmn_format; // si el total es 1 entonces vuelve al menu_0
				else{
					option = scanf_check(1, pkmn_format);
				}
			}while(team[(option + (player*pkmn_format)) - 1].hp == 0);
			if(option <= 1) return 0;
			return (option + 4);
		}

		int move_menu(int player, int cPU1, int cPU2){
			char buffer[100];
			int option;
			int positioning;
			clear_window_and_show_hp(player);
			if(player) positioning = 160;
			else positioning = 24;
			if(pkmn[player].move[0].max_pp == 1) pkmn[player].move[0].pp = 0; // el unico ataque con max_pp = 1 en el juego, es struggle
			if(pp_checking(player)){
				struggle(player); // asigna struggle al pokemon
				option = 1;
			}
			else{
				for(int i=0; i < 4; ++i){
					sprintf(buffer,"%s %d/%d\n", pkmn[player].move[i].name, pkmn[player].move[i].pp, pkmn[player].move[i].max_pp);
					phrase_positioning(buffer, positioning, 1);
				}
				if((cPU1 && player == 0) || (cPU2 && player == 1)) delay(2000);
				if(cPU1 && player == 0) option = 1 + rand() % 4;
				else if(cPU2 && player == 1) option = 1 + rand() % 4;
        	else option = scanf_check(0, 4);
			}
			if(pkmn[player].move[option-1].pp == 0) option = 0;
			return option;
		}

		int bagpack(int option){
			option = 0;
			clear_window_and_show_hp(2);
			phrase_positioning("What are you doing?", 92, 0);
			slow_text_printing("What are you doing?",46, 600, 1);
			phrase_positioning("You don't have a backpack!", 92, 0);
			slow_text_printing("You don't have a backpack!",46, 1000, 1);
			return option;
		}

		int level_1_menus(int option, int player, int cPU1, int cPU2){
			if(option == 4) option = RUN(option);
			if(option == 3) option = bagpack(option);
			if(option == 2) option = pokemon_swap_menu(player, cPU1, cPU2);
			if(option == 1) option = move_menu(player, cPU1, cPU2);
			return option;
		}

		int menu_0(int player, int cPU1, int cPU2){
			int option = 0;
			int positioning;
			char buffer[1000];
			char buffer2[1000];
			while(option == 0){
				if(player) positioning = 160;
				else positioning = 24;
				clear_window_and_show_hp(player);
				flinch = 0;
				printf("\n");
				sprintf(buffer,"Fight[1]     PkMn[2]\n");
				sprintf(buffer2," Pack[3]      Run[4]\n");
				phrase_positioning(buffer, positioning, 1);
				phrase_positioning(buffer2, positioning, 1);
				if((cPU1 && player == 0) || (cPU2 && player == 1)) delay(2000);
				if(cPU1 && player == 0) option = rng(15, 0) + 1; // probabilidad de que escoja combat o switch
				else if(cPU2 && player == 1) option = rng(15, 0) + 1; // probabilidad de que escoja combat o switch
				else scanf("%d", &option);
				fflush(stdin); // este fflush arregla un problema que aun desconosco el porque ocurre
				option = level_1_menus(option, player, cPU1, cPU2);
			}
			return option;
		}

///// main /////
int main(){
	///// variables del main /////
		Current_Turn = 1;
		srand(time(NULL));
		char buffer[100];
		char name[2][30];
		int cPU1, cPU2;
		int option[2];
		int turn[2];
		int hola;
	///// 1eros parametros /////
		system("clear");
		printf("-------------------------------------------------------------------------- Lab Fight: Sala de estudio Version ------------------------------------------------------------------------\n\n");
		sprintf(buffer,"Play[1]      Exit[2]\n");
		phrase_positioning(buffer, 92, 1);
		hola = scanf_check(1, 2);
		if(hola == 2) return 0;
		pokemon_selection(name, &cPU1, &cPU2);
		if(cPU1 != 1 || cPU2 != 1) getchar();
	///// texto inicio pelea /////
		system("clear");
		sprintf(buffer,"///BATTLE START///\n");
		phrase_positioning(buffer, 92, 1);
		sprintf(buffer,"%s wants to battle!", name[1]);
		phrase_positioning(buffer, 92, 0);
		slow_text_printing(buffer, 46, 0, 2);
		sprintf(buffer,"%s sent out", name[1]);
		phrase_positioning(buffer, 92, 0);
		slow_text_printing(buffer, 46, 0, 2);
		sprintf(buffer,"%s!", pkmn[1].name);
		phrase_positioning(buffer, 92, 0);
		slow_text_printing(buffer, 46, 2000, 1);
		sprintf(buffer,"Go! %s!", pkmn[0].name);
		phrase_positioning(buffer, 92, 0);
		slow_text_printing(buffer, 46, 3000, 1);

	///// Pelea /////
		while(pkmn[0].hp > 0 && pkmn[1].hp > 0){
			rand_turn(turn);
			for(int i=0; i < 2; ++i) option[turn[i]] = menu_0(turn[i], cPU1, cPU2) - 1;
			action(option, name, cPU1, cPU2);
			status_checking_end(name, cPU1, cPU2);
		}
		clear_window_and_show_hp(2);

	///// fraces de fin de la pelea /////
		if(pkmn[0].hp <= 0) sprintf(buffer,"PkMn Trainer %s is victorious!", name[1]);
		else sprintf(buffer,"PkMn Trainer %s is victorious!", name[0]);
		phrase_positioning(buffer, 92, 0);
		slow_text_printing(buffer, 46, 3000, 1);
		
	return(0);
}

///// notes /////
	/*typing
		0=normal, 1=figthing, 2=flying, 3=poison, 4=ground, 5=rock,
		6=bug, 7=ghost, 8=steel, 9=fire, 10=water, 11=grass, 12=electric,
		13=psichic, 14=ice, 15=dragon, 16=dark, 17=fairy, 18=null.

	non volatile status usa status[0]. (0 = no status condition)
		Asleep (1 a 4), burned = 5, frozen = 6, paralyzed = 7, poisoned = 8, badly poisoned 10+
		Asleep: el pokemon no puede hacer nada durante 1-3 turnos. Termina cuando a atacado 1-3 veces
		Burn: quita 1/8 de la vida maxima del pokemon al final de los turnos. Sus ataques de categoria fisica disminuyen en 50%. Los pokemon de tipo fuego no lo pueden portar
		frozen: 20% de probabilidad de que el pokemon congelado se descongele. El pokemon se descongela si resive un ataque de tipo fuego
		paralisis: 25% de probabilidad de que el pokemon paralisado no ataque. Su velocidad vaja un 50%
		poisoned: quita 1/8 de la vida maxima del pokemon al final de los turnos.
		badly poisoned: quita 1/16 de la vida maxima del pokemon. En cada final de turno se suma otro 1/16 a la perdida total perdida (1/16 -> 2/16 -> ...)

	Confuse usa status[1].
		Confuse: se confunde de 1-4 turnos. Cada turno que esta confundido hay 50% de probabilidad de que se golpee a si mismo.
		El calculo del golpe se hace considerando un movimiento fisico de poder 40, sin typing, calculado con su ataque y defensa y sin probabilidad de critico. Todos los
		demas parametros del calculo del danyo son considerados

	statch[7]: [0] attack, [1] defense, [2] spattack, [3] spdefense, [4] speed, [5] evacion, [6] accuracy
	selfch[11]: [0] attack, [1] defense, [2] spattack, [3] spdefense, [4] speed, [5] evacion, [6] accuracy, [7]recovery, [8]recoil, [9] selfsc[0], [10]selfsc[1]

	///// datos tipo .txt /////
		se tiene que hace un archivo de tipo .txt, darle el nombre entre ///  /// y colocar los datos de abajo dentro del archivo .txt
		///// database.txt /////
			NULL
			18 18
			0 0 0 0 0 0
			NULL 18 0 0 100 910 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			NULL 18 0 0 100 910 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			NULL 18 0 0 100 910 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			NULL 18 0 0 100 910 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Pikachu
			12 18
			1000000 146 116 136 136 216
			Nuzzle 12 20 0 20 100 7 0 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Volt_Tackle 12 120 0 15 100 7 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Iron_Tail 8 100 0 15 75 0 0 0 0 -1 0 0 0 0 0 75 0 0 0 0 0 0 0 0 0 0 0 0 0
			Charge_Beam 12 50 1 16 90 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 70 0
			Jiggilypuff
			0 17
			1000000 126 76 126 86 76
			Rollout 5 30 0 20 90 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Sing 0 0 2 15 55 4 0 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Knock_Off 16 65 0 20 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Dazzling_Gleam 17 80 1 10 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Magmar
			9 18
			1000000 226 150 236 206 222
			Flamethrower 9 90 1 15 100 5 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Confuse_Ray 7 0 2 10 100 0 1 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Clear_Smog 3 50 1 15 910 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Mud-Slap  4 20 1 10 100 0 0 0 0 0 0 0 0 0 -1 100 0 0 0 0 0 0 0 0 0 0 0 0 0
			Ghastly
			7 3
			201 75 96 236 106 196
			Shadow_Ball 7 80 1 24 100 0 0 0 0 0 0 -1 0 0 0 20 0 0 0 0 0 0 0 0 0 0 0 0 0
			Energy_Ball 11 90 1 16 100 0 0 0 0 0 0 -1 0 0 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0
			Sludge_Bomb 3 90 1 16 100 7 0 30 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Toxic 3 0 2 20 910 10 0 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Skarmory
			8 2
			271 196 316 116 176 176
			Drill_Peck 2 80 0 32 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Brave_Bird 2 120 0 24 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 33 0 0 100 0
			Iron_Head 8 80 0 24 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 30
			Return 0 102 0 32 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Azumarill
			10 17
			341 272 196 156 196 136
			Waterfall 10 80 0 24 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 20
			Play_Rough 17 90 0 16 90 0 0 0 -1 0 0 0 0 0 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0
			Ice_Punch 14 75 0 24 100 6 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Bulldoze 4 60 0 32 100 0 0 0 0 0 0 0 -1 0 0 100 0 0 0 0 0 0 0 0 0 0 0 0 0
			Dragonair
			15 18
			263 204 166 176 176 176
			Outrage 15 120 0 16 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Waterfall 10 80 0 24 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 20
			Hidden_Power 8 60 1 24 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Thunderbolt 12 90 1 24 100 7 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Mega_Mawile
			8 17
			241 492 286 146 226 136
			Iron_Head 8 80 0 24 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 30
			Play_Rough 17 90 0 16 90 0 0 0 -1 0 0 0 0 0 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0
			Crunch 16 80 0 24 100 0 0 0 0 -1 0 0 0 0 0 20 0 0 0 0 0 0 0 0 0 0 0 0 0
			Swords_Dance 0 0 3 32 910 0 0 0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 0 0 0 0 100 0
			Greninja
			10 16
			285 195 170 242 178 280
			Surf 10 90 1 24 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Dark_Pulse 16 80 1 24 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 20
			Extrasensory 13 80 1 32 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 10
			Icy_Wind 14 55 1 24 100 0 0 0 0 0 0 0 -1 0 0 100 0 0 0 0 0 0 0 0 0 0 0 0 0
			Gallade
			13 13
			277 286 166 166 266 196
			Swords_Dance 0 0 3 32 910 0 0 0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 0 0 0 0 100 0
			Close_Combat 1 120 0 8 100 0 0 0 0 0 0 0 0 0 0 0 0 -1 0 -1 0 0 0 0 0 0 0 100 0
			Zen_Headbutt 13 80 0 24 90 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 20
			Knock_Off 16 65 0 32 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Volcarona
			9 6
			311 125 166 306 246 236
			Quiver_Dance 0 0 3 32 910 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 100 0
			Flamethrower 9 90 1 15 100 5 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Bug_Buzz 6 90 1 16 100 0 0 0 0 0 0 -1 0 0 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0
			Roost 2 0 3 16 910 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 100 0
			Snorlax
			0 18
			461 256 166 166 256 96
			Stockpile 0 0 3 32 910 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 100 0
			Rest 13 0 3 16 910 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 0 4 0 100 0
			Crunch 16 80 0 24 100 0 0 0 0 -1 0 0 0 0 0 20 0 0 0 0 0 0 0 0 0 0 0 0 0
			Body_Slam 0 85 0 24 100 7 0 30 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Flygon
			4 15
			301 236 196 196 196 236
			Boomburst 0 140 1 16 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Draco_Meteor 15 130 1 8 90 0 0 0 0 0 0 0 0 0 0 0 0 0 -2 0 0 0 0 0 0 0 0 100 0
			Earthquake 4 100 0 16 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Roost 2 0 3 16 910 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 100 0
			Scizor
			6 8
			281 296 236 146 196 196
			Iron_Head 8 80 0 24 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 30
			Superpower 1 120 0 8 100 0 0 0 0 0 0 0 0 0 0 0 -1 0 -1 0 0 0 0 0 0 0 0 100 0
			X-Scissor 6 80 0 24 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Metal_Claw 8 50 0 56 95 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 10 0
			Chulxz_Qlo
			0 18
			200 10 10 10 10 10
			Shulxzzzzz¡¡¡ 15 200 1 20 99 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Shulxzzzzz¡¡¡ 15 200 1 20 99 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Shulxzzzzz¡¡¡ 15 200 1 20 99 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Shulxzzzzz¡¡¡ 15 200 1 20 99 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Profe_Qlo
			6 14
			200 10 10 10 10 10
			PROOOFEEEE¡¡¡ 15 200 1 20 99 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			PROOOFEEEE¡¡¡ 15 200 1 20 99 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			PROOOFEEEE¡¡¡ 15 200 1 20 99 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			PROOOFEEEE¡¡¡ 15 200 1 20 99 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Lucifer_Dios
			8 17
			10000 1000 500 1000 1000 1000
			Leech_Seed 11 0 2 20 90 0 3 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Toxic 3 0 2 20 910 10 0 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Swagger 0 0 2 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
			Debil`s_Arm 8 1000 0 20 100 9 0 5 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 100
			TATA_JOTA
			5 12
			10000 100 100 100 100 50
			La_Juventud 18 100 1 90 910 2 0 100 0 0 -1 0 0 0 0 100 0 0 0 0 0 0 0 2 0 0 0 100 100
			No_Sabe 18 100 1 90 910 2 0 100 0 0 -1 0 0 0 0 100 0 0 0 0 0 0 0 2 0 0 0 100 100
			Lo_Que_Es 18 100 1 90 910 2 0 100 0 0 -1 0 0 0 0 100 0 0 0 0 0 0 0 2 0 0 0 100 100
			La_Muerte 18 100 1 90 910 2 0 100 0 0 -1 0 0 0 0 100 0 0 0 0 0 0 0 2 0 0 0 100 100

		////// chart.txt /////
			1 1 1 1 1 0.5 1 0 0.5 1 1 1 1 1 1 1 1 1 1
			2 1 0.5 0.5 1 2 0.5 0 2 1 1 1 1 0.5 2 1 2 0.5 1
			1 2 1 1 1 0.5 2 1 0.5 1 1 2 0.5 1 1 1 1 1 1
			1 1 1 0.5 0.5 0.5 1 0.5 0 1 1 2 1 1 1 1 1 2 1
			1 1 0 2 1 2 0.5 1 2 2 1 0.5 2 1 1 1 1 1 1
			1 0.5 2 1 0.5 1 2 1 0.5 2 1 1 1 1 2 1 1 1 1
			1 0.5 0.5 0.5 1 1 1 0.5 0.5 0.5 1 2 1 2 1 1 2 0.5 1
			0 1 1 1 1 1 1 2 1 1 1 1 1 2 1 1 0.5 1 1
			1 1 1 1 1 2 1 1 0.5 0.5 0.5 1 0.5 1 2 1 1 2 1
			1 1 1 1 1 0.5 2 1 2 0.5 0.5 2 1 1 2 0.5 1 1 1
			1 1 1 1 2 2 1 1 1 2 0.5 0.5 1 1 1 0.5 1 1 1
			1 1 0.5 0.5 2 2 0.5 1 0.5 0.5 2 0.5 1 1 1 0.5 1 1 1
			1 1 2 1 0 1 1 1 1 1 2 0.5 0.5 1 1 0.5 1 1 1
			1 2 1 2 1 1 1 1 0.5 1 1 1 1 0.5 1 1 0 1 1
			1 1 2 1 2 1 1 1 0.5 0.5 0.5 2 1 1 0.5 2 1 1 1
			1 1 1 1 1 1 1 1 0.5 1 1 1 1 1 1 2 1 0 1
			1 0.5 1 1 1 1 1 2 1 1 1 1 1 2 1 1 0.5 0.5 1
			1 2 1 0.5 1 1 1 1 0.5 0.5 1 1 1 1 1 2 2 1 1
			1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1

	Cryptpad(s)
		Cryptpad 1:
			https://cryptpad.fr/code/#/2/code/edit/n5FfIR+mgpqiNfPuv8FY6XjI/
		cryptpad 2:
			https://cryptpad.fr/code/#/2/code/edit/oMRjAM0fT5lMUO6a7bL4f8nA/
		cryptpad 3 (profe):
			https://cryptpad.fr/code/#/2/code/edit/z-MMq0GWVX2DGvESd7l+fnMM/*/
///// basura /////
/**/