// // Copyright 2016 theaigames.com (developers@theaigames.com)

//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at

//        http://www.apache.org/licenses/LICENSE-2.0

//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
//    For the full copyright and license information, please view the LICENSE
//    file that was distributed with this source code.

package bot;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

/**
 * BotStarter class
 * 
 * Magic happens here. You should edit this file, or more specifically
 * the makeTurn() method to make your bot do more than random moves.
 * 
 * @author Jim van Eeden <jim@starapple.nl>
 */

public class BotStarter {
    
	public static final int MAX_DEPTH = 4;

    /**
     * Makes a turn. Edit this method to make your bot smarter.
     * Currently does only random moves.
     *
     * @return The column where the turn was made.
     */
	

	public Move makeTurn(Field field) {
		/* fac mai intai o iteratie de alfa beta, apoi pe restul pentru a putea salva miscarea ideala */
		List<Move> list = field.getAvailableMoves();
		
		if(list.size() > 80) {
			Random generator = new Random();
			return new Move(Math.abs(generator.nextInt() % 9), Math.abs(generator.nextInt() % 9));
		}

		int score = Integer.MIN_VALUE;
		Move idealMove = new Move(0,0);
		Field initialField = new Field(deepCopy(field.mBoard), deepCopy(field.mMacroboard));
		
		for(Move move : list) {
			Field updatedField = field.updateField(move, BotParser.mBotId);
			if(field.gameOver() == BotParser.mBotId)
				return move;
			
			int tempScore = alphaBeta(updatedField, MAX_DEPTH - 1, Integer.MIN_VALUE, Integer.MAX_VALUE, 3 - BotParser.mBotId);
			
			if(tempScore > score) {
				idealMove = move;
				score = tempScore;
			}

			field = new Field(deepCopy(initialField.mBoard), deepCopy(initialField.mMacroboard));
		}

		return idealMove;
	}

	public static int evaluate(Field field) {
		
		/* valori pentru castig total */
		if(field.gameOver() == BotParser.mBotId) {
			return 10000; // Am castigat
		} else {
			if(field.gameOver() == 3 - BotParser.mBotId)
				return -10000; // A castigat adversarul
		}

		int S = 0;
		int count_1eu = 0; /* eu - -  */
		int count_1el = 0; /* el - -  */
		int count_2eu = 0; /* eu eu - */
		int count_2el = 0; /* el el - */

		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				int k = field.microBoardFinished(i, j);

				if(k == 0) {

					// daca nimeni n-a castigat un microboard, se calculeaza scorul:
					int[][] linii = field.listeazaLinii(i, j);
					for(int p = 0; p < 8; p++) {
						int count_eu = 0;
						int count_el = 0;
						for(int q = 0; q < 3; q++) {
							if(linii[p][q] == BotParser.mBotId)
								count_eu++;
							if(linii[p][q] == 3 - BotParser.mBotId)
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
		}

		S += 5 * (count_1eu - count_1el) + 50 * (count_2eu - count_2el);

		int[][] linii = listeazaLinii(field.mMacroboard);
		for(int i = 0; i < 8; i++) {
			int count_eu = 0;
			int count_el = 0;
			for(int j = 0; j < 3; j++) {
				if(linii[i][j] == BotParser.mBotId) {
					count_eu++;
				} else {
					if(linii[i][j] == 3 - BotParser.mBotId) {
						count_el++;
					}
				}
			}

			if(count_eu == 1 && count_el == 0)
				S += 1000;

			if(count_eu == 2 && count_el == 0)
				S += 5000;

			if(count_el == 1 && count_eu == 0)
				S -= 1000;

			if(count_el == 2 && count_eu == 0)
				S -= 5000;
		}

		return S;
	}

	/* Alpha-Beta Pruning */
	public static int alphaBeta(Field field, int depth, int alpha, int beta, int player) {
		if(depth == 0 || field.gameOver() != 0)
			return evaluate(field);

		if(player == BotParser.mBotId) {
			int v = Integer.MIN_VALUE;
			List<Move> moves = field.getAvailableMoves();
			Field newField = new Field(deepCopy(field.mBoard), deepCopy(field.mMacroboard));
			for (Move m : moves) {
				Field updatedField = newField.updateField(m, BotParser.mBotId);
				v = Math.max(v, alphaBeta(updatedField, depth - 1, alpha, beta, 3 - player));
				alpha = Math.max(alpha, v);
				newField = new Field(deepCopy(field.mBoard), deepCopy(field.mMacroboard));
				if (beta <= alpha)
					break;
			}
			return v;

		} else {
			int v = Integer.MAX_VALUE;
			List<Move> moves = field.getAvailableMoves();
			Field newField = new Field(deepCopy(field.mBoard), deepCopy(field.mMacroboard));
			for (Move m : moves) {
				Field updatedField = newField.updateField(m, 3 - BotParser.mBotId);
				v = Math.min(v, alphaBeta(updatedField, depth - 1, alpha, beta, 3 - player));
				alpha = Math.min(beta, v);
				newField = new Field(deepCopy(field.mBoard), deepCopy(field.mMacroboard));
				if (beta <= alpha)
					break;
			}
			return v;
		}
	}

	/* Primeste ca parametru coordonata unui microBoard din macroBoard si ii returneaza toate liniile
		   0 <= x, y <= 2. */
	public static int[][] listeazaLinii(int[][] macro) {
		

		int[][] microBoard = new int[3][3];
		for(int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				microBoard[i][j] = macro[i][j];
			}
		}
		int[][] linii = new int[8][3];
		for(int i = 0; i < 3; i++) {

			// pentru linii
			linii[0][i] = microBoard[0][i];
			linii[1][i] = microBoard[1][i];
			linii[2][i] = microBoard[2][i];
		}

		for(int i = 0; i < 3; i++) {
			// pentru coloane
			linii[3][i] = microBoard[i][0];
			linii[4][i] = microBoard[i][1];
			linii[5][i] = microBoard[i][2];
		}

		// pentru diagonala principala
		linii[6][0] = microBoard[0][0];
		linii[6][1] = microBoard[1][1];
		linii[6][2] = microBoard[2][2];

		// pentru diagonala secundara
		linii[7][0] = microBoard[0][2];
		linii[7][1] = microBoard[1][1];
		linii[7][2] = microBoard[2][0];

		return linii;
	}

	public static int[][] deepCopy(int[][] original) {
		if (original == null) {
			return null;
		}

		final int[][] result = new int[original.length][];
		for (int i = 0; i < original.length; i++) {
			result[i] = Arrays.copyOf(original[i], original[i].length);
		}
		return result;
	}


	public static void main(String[] args) {
		BotParser parser = new BotParser(new BotStarter());
		parser.run();
	}
}