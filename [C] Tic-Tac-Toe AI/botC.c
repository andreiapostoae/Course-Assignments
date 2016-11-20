/*******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Adriano Grieb
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define MAX_LINE_LENGTH 255


struct
{
  int timebank;
  int time_per_move;
  int botid;
} game_settings;

struct
{
  int macro[3 * 3];
  int field[9 * 9];
} board;


struct move
{
  int x;
  int y;
} ;

void settings(char *, char *);
void update(char *, char *, char *);
void random_action(char *, char *);

int max(int a, int b) {
  if(a > b)
    return a;
  return b;
}

int min(int a, int b) {
  if(a < b)
    return a;
  return b;
}

struct timeval stop, start;
int time_limit;

int winnerMicro(int** micro) {
  int i, j;
  for(i = 0; i < 3; i++) {
    int counteu = 0;
    int countel = 0;

    for(j = 0; j < 3; j++) {
      if(micro[i][j] == game_settings.botid)
        counteu++;
      else
        if(micro[i][j] == 3 - game_settings.botid)
          countel++;
    }

    if(counteu == 3)
      return game_settings.botid;
    if(countel == 3)
      return (3 - game_settings.botid);

    counteu = 0;
    countel = 0;


    for(j = 0; j < 3; j++) {
      if(micro[j][i] == game_settings.botid)
        counteu++;
      else
        if(micro[j][i] == 3 - game_settings.botid)
          countel++;
    }

    if(counteu == 3)
      return game_settings.botid;
    if(countel == 3)
      return (3 - game_settings.botid);

  }
    
    int counteu = 0;
    int countel = 0;

    for(i = 0; i < 3; i++) {
      if(micro[i][i] == game_settings.botid)
        counteu++;
      else
        if(micro[i][i] == 3 - game_settings.botid)
          countel++;
    }

    if(counteu == 3)
      return game_settings.botid;
    if(countel == 3)
      return (3 - game_settings.botid);


    counteu = 0;
    countel = 0;

    for(i = 0; i < 3; i++) {
      if(micro[i][2-i] == game_settings.botid)
        counteu++;
      else
        if(micro[i][2-i] == 3 - game_settings.botid)
          countel++;
    }

    if(counteu == 3)
      return game_settings.botid;
    if(countel == 3)
      return (3 - game_settings.botid);

    for(i = 0; i < 3; i++)
      for(j = 0; j < 3; j++)
        if(micro[i][j] == 0)
          return 0; // daca e inca completabila

    return -1; // daca e draw
}


/* SUPER FUTUTA */
void updateMacro(int move) {
  int num = move / 9;
  int **micro = (int **)malloc(3* sizeof(int*));
  int i,j;
  for(i = 0; i < 3; i++) 
      micro[i] = (int *)malloc(3* sizeof(int));
 
  for(i = 0; i < 3; i++)
    for(j = 0; j < 3; j++)
      micro[i][j] = board.field[9 * num + 3 * i + j];

  for(i = 0; i < 9; i++)
    if(board.macro[i] == -1)
      board.macro[i] = 0;

  int aux = winnerMicro(micro);
  if(aux == 1)
  	board.macro[num] = 1;
  else
  	if(aux == 2)
  		board.macro[num] = 2;


  num = move % 9;
  micro = (int **)malloc(3* sizeof(int*));
  for(i = 0; i < 3; i++) 
      micro[i] = (int *)malloc(3* sizeof(int));
 
  for(i = 0; i < 3; i++)
    for(j = 0; j < 3; j++)
      micro[i][j] = board.field[9 * num + 3 * i + j];

  int future = winnerMicro(micro);
  if(future != 0) {
  	for(i = 0; i < 9; i++)
  		if(board.macro[i] == 0)
  			board.macro[i] = -1;
  } else {
  	board.macro[num] = -1;
  }

}


int gameOver() {
  int i = 0;
  for(i = 0; i < 9; i++)
    if(board.macro[i] == -1)
      return 0;

  return 1;
}

int** listeazaLinii(int** macro) {
    int i;
    
    int** linii = calloc(8, sizeof(int *));
    for(i = 0; i < 8; i++)
      linii[i] = calloc(3, sizeof(int));

    for(i = 0; i < 3; i++) {
      // pentru linii
      linii[0][i] = macro[0][i];
      linii[1][i] = macro[1][i];
      linii[2][i] = macro[2][i];
    }

    for(i = 0; i < 3; i++) {
      // coloane
      linii[3][i] = macro[i][0];
      linii[4][i] = macro[i][1];
      linii[5][i] = macro[i][2];
    }

    // diagonala mare
    linii[6][0] = macro[0][0];
    linii[6][1] = macro[1][1];
    linii[6][2] = macro[2][2];

    // diagonala mica
    linii[7][0] = macro[0][2];
    linii[7][1] = macro[1][1];
    linii[7][2] = macro[2][0];

    return linii;
  }

int wonGame() {
  int **macro = (int **)malloc(3* sizeof(int*));
  int i,j;
  for(i = 0; i < 3; i++) 
      macro[i] = (int *)malloc(3* sizeof(int));
  
  for(i = 0; i < 3; i++)
    for(j = 0; j < 3; j++)
        macro[i][j] = board.macro[3 * i + j];

  int k = winnerMicro(macro);
  if(k == 1)
    return 1;
  if(k == 2)
    return 2;
  return 0;
}

int evaluate() {

  int S = 0;
  int count_1eu = 0;
  int count_2eu = 0;
  int count_1el = 0;
  int count_2el = 0;

  int i, j, mic;



  int **macro = (int **)malloc(3* sizeof(int*));
    for(i = 0; i < 3; i++) 
      macro[i] = (int *)malloc(3* sizeof(int));

  for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
          macro[i][j] = board.macro[3 * i + j];

  int** linii = listeazaLinii(macro);

  for(i = 0; i < 8; i++) {
      int count_eu = 0;
      int count_el = 0;
      for(j = 0; j < 3; j++) {
        if(linii[i][j] == game_settings.botid) {
          count_eu++;
        } else {
          if(linii[i][j] == 3 - game_settings.botid) {
            count_el++;
          }
        }
      }

      if(count_eu == 3)
      	return 32000;

      if(count_el == 3)
      	return -32000;

      if(count_eu == 1 && count_el == 0)
        S += 500;

      if(count_eu == 2 && count_el == 0)
        S += 2000;

      if(count_el == 1 && count_eu == 0)
        S -= 500;

      if(count_el == 2 && count_eu == 0)
        S -= 2000;

  }

  for(mic = 0; mic < 9; mic++) {
    int **micro = (int **)malloc(3* sizeof(int*));
    for(i = 0; i < 3; i++) 
      micro[i] = (int *)malloc(3* sizeof(int));

    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
          micro[i][j] = board.field[9 * mic + 3 * i + j];

    int r = winnerMicro(micro);
    if(r == 0) {
      int** linii = listeazaLinii(micro);
      int p, q;
      for(p = 0; p < 8; p++) {
            int count_eu = 0;
            int count_el = 0;
            for(q = 0; q < 3; q++) {
              if(linii[p][q] == game_settings.botid)
                count_eu++;
              if(linii[p][q] == 3 - game_settings.botid)
                count_el++;
            }
            if(count_eu == 1 && count_el == 0)
              count_1eu++;
            if(count_eu == 2 && count_el == 0)
              count_2eu++;
            if(count_eu == 0 && count_el == 1)
              count_1el++;
            if(count_eu == 0 && count_el == 2)
              count_2el++;
      } 
    }
  } 
   
  S += 20 * (count_1eu - count_1el) + 100 * (count_2eu - count_2el);


  return S; 
}

int* alphaBeta(int depth, int alpha, int beta, int player) {
	int idealMove;
	int temp_score = 0;
	int* result = calloc(2, sizeof(int));

	int time_ended = 0;
	

  	if(depth == 0 || gameOver() != 0) {
  		result[0] = evaluate();
  		result[1] = -1;
  		return result;
  	}

  	int availableMove[100];
  	int current_index = 0;
  	int i, j, k;


  	for(i = 0; i < 81; i++) {
  		if(board.field[i] == 0 && board.macro[i / 9] == -1) {
  			availableMove[current_index++] = i;
  		}
  	}

  	for(i = 0; i < current_index; i++) {
  		int current_move = availableMove[i];
  		board.field[current_move] = player;


  		/* copiez macroul initial */
  		int macro_copy[9];
  		for(j = 0; j < 9; j++)
  			macro_copy[j] = board.macro[j];

  		updateMacro(current_move);

  		if(player == game_settings.botid) {
  			/* daca e randul meu */
  			temp_score = alphaBeta(depth - 1, alpha, beta, 3 - game_settings.botid)[0];

  			if(alpha < temp_score) {
  				alpha = temp_score;
  				idealMove = current_move;
  			}
  		} else {
  			temp_score = alphaBeta(depth - 1, alpha, beta, game_settings.botid)[0];
  			if(beta > temp_score) {
  				beta = temp_score;
  				idealMove = current_move;
  			}
  		}

  		board.field[current_move] = 0;
  		for(j = 0; j < 9; j++)
  			board.macro[j] = macro_copy[j];


  		if(alpha >= beta)
  			break;
  	}

	if(player == game_settings.botid)
		result[0] = alpha;
	else
		result[0] = beta;

	result[1] = idealMove;
	return result; 	
}


int main(int argc, char const *argv[])
{
  char line[MAX_LINE_LENGTH];
  char part[3][MAX_LINE_LENGTH];

  #ifdef DEBUG
  freopen("test.in", "r", stdin);
  #endif
  srand(time(NULL));
  while(fgets(line, MAX_LINE_LENGTH, stdin) != NULL)
  {
    if (!strncmp(line, "action ", 7))
    {
      sscanf(&line[7], "%s %s", part[0], part[1]);
      random_action(part[0], part[1]);
      fflush(stdout);
      continue;
    }
    if (!strncmp(line, "update ", 7))
    {
      sscanf(&line[7], "%s %s %s", part[0], part[1], part[2]);
      update(part[0], part[1], part[2]);
      continue;
    }
    if (!strncmp(line, "settings ", 9))
    {
      sscanf(&line[9], "%s %s", part[0], part[1]);
      settings(part[0], part[1]);
      continue;
    }
  }

  #ifdef DEBUG
  fprintf(stderr, "Game over!\n");
  #endif

  exit(EXIT_SUCCESS);
}

void random_action(char *action, char *value)
{
  assert(action != NULL);
  assert(value != NULL);

  if (!strcmp(action, "move"))
  {
  	int i;
  	int first_move = 1;
  	int x = -1, y = -1;
  	int move;

  	int runda = 0;
  	for(i = 0; i < 81; i++)
  		if(board.field[i] != 0) {
  			first_move = 0;
  			runda++;
  		}

  	int max_depth;
    if(runda < 18)
      max_depth = 6;
    else if(runda < 28)
        max_depth = 7;
       else 
          max_depth = 8;


  	if(first_move == 1) {
  		fprintf(stdout, "place_move 4 4\n");
  	} else {
  		int ok = 1;
  		int* best_move = calloc(2, sizeof(int));
      best_move[0] = -1;
      best_move[1] = -1;

      int availableMove[100];
      int current_index = 0;
      int j, k;
      int dont_alphabeta = 0;


      for(i = 0; i < 81; i++) {
        if(board.field[i] == 0 && board.macro[i / 9] == -1) {
          availableMove[current_index++] = i;
        }
      }

      for(j = 0; j < current_index; j++) {
        int current_move = availableMove[j];
        board.field[current_move] = game_settings.botid;

        int aux[9];
        for(k = 0; k < 9; k++)
          aux[k] = board.macro[k];
        updateMacro(current_move);

        if(evaluate() == 32000) {
          best_move[1] = current_move;
          dont_alphabeta = 1;
          break;
        }

        board.field[current_move] = 0;
        for(k = 0; k < 9; k++)
          board.macro[k] = aux[k];

      }


      if(dont_alphabeta == 0)
        best_move = alphaBeta(max_depth, -1000000, 1000000, game_settings.botid);
  		
  	
      int m = best_move[1]/ 9;
  		int n = best_move[1] % 9;
  		x = ((m % 3) * 3) + (n % 3);
      y = ((m / 3) * 3) + (n / 3);



  		fprintf(stdout, "place_move %d %d\n", x, y);
  	}
    
   

    #ifdef DEBUG
    fprintf(stderr, "available moves: %d pick: %d\n", count, pick);
    #endif

  }
  #ifdef DEBUG
  else
  {
    fprintf(stderr, "unknown action: [%s: %s]\n", action, value);
  }
  #endif
}

void update(char *game, char *update, char *value)
{
  assert(game != NULL);
  assert(update != NULL);
  assert(value != NULL);

  if (strcmp(game, "game"))
  {
    #ifdef DEBUG
    fprintf(stderr, "unknown update: [%s - %s: %s]\n", game, update, value);
    #endif
    return;
  }

  // update game round 1
  if (!strcmp(update, "round"))
  {
    return;
  }
  // update game move 1
  if (!strcmp(update, "move"))
  {
    return;
  }
  // update game macroboard -1,-1,-1,-1,-1,-1,-1,-1,-1
  if (!strcmp(update, "macroboard"))
  {
    sscanf(value, "%d,%d,%d,%d,%d,%d,%d,%d,%d", &board.macro[0],
      &board.macro[1], &board.macro[2], &board.macro[3], &board.macro[4],
      &board.macro[5], &board.macro[6], &board.macro[7], &board.macro[8]);
    return;
  }
  // update game field  0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0
  // no new lines and no spaces after the commas
  if (!strcmp(update, "field"))
  {
    sscanf(value,                   // indexes of the vector (ask why? ;)
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 0  1  2  9  10 11 18 19 20
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 3  4  5  12 13 14 21 22 23
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 6  7  8  15 16 17 24 25 26
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 27 28 29 36 37 38 45 46 47
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 30 31 32 39 40 41 48 49 50
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 33 34 35 42 43 44 51 52 53
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 54 55 56 63 64 65 72 73 74
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 57 58 59 66 67 68 75 76 77
      "%d,%d,%d,%d,%d,%d,%d,%d,%d", // 60 61 62 69 70 71 78 79 80
      &board.field[0],  &board.field[1],  &board.field[2],  &board.field[9],
      &board.field[10], &board.field[11], &board.field[18], &board.field[19],
      &board.field[20], &board.field[3],  &board.field[4],  &board.field[5],
      &board.field[12], &board.field[13], &board.field[14], &board.field[21],
      &board.field[22], &board.field[23], &board.field[6],  &board.field[7],
      &board.field[8],  &board.field[15], &board.field[16], &board.field[17],
      &board.field[24], &board.field[25], &board.field[26], &board.field[27],
      &board.field[28], &board.field[29], &board.field[36], &board.field[37],
      &board.field[38], &board.field[45], &board.field[46], &board.field[47],
      &board.field[30], &board.field[31], &board.field[32], &board.field[39],
      &board.field[40], &board.field[41], &board.field[48], &board.field[49],
      &board.field[50], &board.field[33], &board.field[34], &board.field[35],
      &board.field[42], &board.field[43], &board.field[44], &board.field[51],
      &board.field[52], &board.field[53], &board.field[54], &board.field[55],
      &board.field[56], &board.field[63], &board.field[64], &board.field[65],
      &board.field[72], &board.field[73], &board.field[74], &board.field[57],
      &board.field[58], &board.field[59], &board.field[66], &board.field[67],
      &board.field[68], &board.field[75], &board.field[76], &board.field[77],
      &board.field[60], &board.field[61], &board.field[62], &board.field[69],
      &board.field[70], &board.field[71], &board.field[78], &board.field[79],
      &board.field[80]);
    return;
  }
}

void settings(char *setting, char *value)
{
  assert(setting != NULL);
  assert(value != NULL);

  // settings timebank 10000
  if (!strcmp(setting, "timebank"))
  {
    game_settings.timebank = atoi(value);
    #ifdef DEBUG
    fprintf(stderr, "settings timebank: %d\n", game_settings.timebank);
    #endif
    return;
  }
  // settings time_per_move 500
  if (!strcmp(setting, "time_per_move"))
  {
    game_settings.time_per_move = atoi(value);
    #ifdef DEBUG
    fprintf(stderr, "time_per_move: %d\n", game_settings.time_per_move);
    #endif
    return;
  }
  // settings player_names player1,player2
  if (!strcmp(setting, "player_names"))
  {
    return;
  }
  // settings your_bot player1
  if (!strcmp(setting, "your_bot"))
  {
    return;
  }
  // settings your_botid 1
  if (!strcmp(setting, "your_botid"))
  {
    game_settings.botid = atoi(value);
    #ifdef DEBUG
    fprintf(stderr, "botid: %d\n", game_settings.botid);
    #endif
    return;
  }

  #ifdef DEBUG
  fprintf(stderr, "unknown setting: [%s: %s]\n", setting, value);
  #endif
}
