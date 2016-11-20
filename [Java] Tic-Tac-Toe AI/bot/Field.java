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

import java.util.ArrayList;

/**
 * Field class
 * 
 * Handles everything that has to do with the field, such 
 * as storing the current state and performing calculations
 * on the field.
 * 
 * @author Jim van Eeden <jim@starapple.nl>, Joost de Meij <joost@starapple.nl>
 */

public class Field {
    private int mRoundNr;
    private int mMoveNr;
	public int[][] mBoard;
	public int[][] mMacroboard;


	private final int COLS = 9, ROWS = 9;

	
	private String mLastError = "";
	
	public Field() {
		mBoard = new int[COLS][ROWS];
		mMacroboard = new int[COLS / 3][ROWS / 3];
		clearBoard();
	}

	public Field(int[][] mBoard, int[][] mMacroboard) {
		this.mBoard = mBoard;
		this.mMacroboard = mMacroboard;
	}

	public Field updateField(Move move, int player) {
		int x = move.getX();
		int y = move.getY();
		if(mBoard[x][y] != 0) {
			System.out.println("EROARE LA UPDATE FIELD");
		} else {
			mBoard[x][y] = player; /* update board */
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					if(mMacroboard[i][j] == -1)
						mMacroboard[i][j] = 0; // reinitializarea pentru cazul in care aveai mai multe micro in care puteai pune
				}
			}
			int aux = microBoardFinished(x/3, y/3); // adica pun playerul respectiv acolo
			if(aux == 1)
				mMacroboard[x/3][y/3] = 1;
			if(aux == 2)
				mMacroboard[x/3][y/3] = 2;
		}

		int futureMicro = microBoardFinished(x % 3, y % 3);
		if(futureMicro != 0) {
			// adica daca miscarea te trimite intr-un micro deja terminat, faci toate microboardurile neterminate available
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					if(mMacroboard[i][j] == 0)
						mMacroboard[i][j] = -1;
				}
			}
		} else {
			mMacroboard[x % 3][y % 3] = -1; // daca te trimite intr-unul neterminat, el va fi singurul available
		}
		return this;
	}

	public int[][] getMicroboard(int x, int y) {
		int[][] micro = new int[3][3];
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				micro[i][j] = mBoard[x * 3 + i][y * 3 + j];
			}
		}
		return micro;
	}

	public int gameOver() {
		// 0 if not over
		// 1 if 1 won
		// 2 if 2 won
		// 3 if draw
		int[][] microBoard = new int[3][3];
		for(int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				microBoard[i][j] = mMacroboard[i][j];
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

		for(int i = 0; i <= 7; i++) {
			int count1 = 0;
			int count2 = 0;
			for(int j = 0; j < 3; j++) {
				if(linii[i][j] == 1)
					count1++;
				if(linii[i][j] == 2)
					count2++;
			}
			if(count1 == 3)
				return 1;
			if(count2 == 3)
				return 2;
		}

		if(getAvailableMoves() == null)
			return 3;

		return 0;
	}

	public int microBoardFinished(int x, int y) {
		// 0 if not over
		// 1 if 1 won
		// 2 if 2 won
		// 3 if draw
		int player;
		int[][] linii = listeazaLinii(x, y);
		boolean finished = true;

		// Se verifica daca s-a castigat pe vreo linie.
		int count0 = 0;
		for(int i = 0; i < 8; i++) {
			int count1 = 0;
			int count2 = 0;
			for(int j = 0; j < 3; j++) {
				if(linii[i][j] == 1)
					count1++;
				if(linii[i][j] == 2)
					count2++;
				if(linii[i][j] == 0)
					count0++;
			}
			if(count1 == 3)
				return 1;
			if(count2 == 3)
				return 2;
		}

		if(count0 > 0)	// se mai poate completa adica
			return 0;

		return 3;
	}


	public int[][] listeazaLinii(int x, int y) {
		/* primeste ca parametru coordonata unui microBoard din macroBoard si ii returneaza toate liniile
		   0 <= x, y <= 2*/
		int[][] microBoard = new int[3][3];
		for(int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				microBoard[i][j] = mBoard[x * 3 + j][y * 3 + i];
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

		// pentru diagonala mare
		linii[6][0] = microBoard[0][0];
		linii[6][1] = microBoard[1][1];
		linii[6][2] = microBoard[2][2];

		// pentru diagonala mica
		linii[7][0] = microBoard[0][2];
		linii[7][1] = microBoard[1][1];
		linii[7][2] = microBoard[2][0];

		return linii;
	}

	public int[][] listeazaLiniiMacro() {
		int[][] linii = new int[8][3];
		for(int i = 0; i < 3; i++) {
			
			// pentru linii
			linii[0][i] = mMacroboard[0][i];
			linii[1][i] = mMacroboard[1][i];
			linii[2][i] = mMacroboard[2][i];
		}

		for(int i = 0; i < 3; i++) {
			// pentru coloane
			linii[3][i] = mMacroboard[i][0];
			linii[4][i] = mMacroboard[i][1];
			linii[5][i] = mMacroboard[i][2];
		}

		// pentru diagonala principala
		linii[6][0] = mMacroboard[0][0];
		linii[6][1] = mMacroboard[1][1];
		linii[6][2] = mMacroboard[2][2];

		// pentru diagonala secundara
		linii[7][0] = mMacroboard[0][2];
		linii[7][1] = mMacroboard[1][1];
		linii[7][2] = mMacroboard[2][0];

		return linii;
	}



	public boolean isEmpty(int x, int y) {
		boolean empty = true;
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++)
				if(mBoard[x * 3 + i][y * 3 + j] != 0)
					empty = false;
		}
		return empty;
	}


	/**
	 * Parse data about the game given by the engine
	 * @param key : type of data given
	 * @param value : value
	 */
	public void parseGameData(String key, String value) {
	    if (key.equals("round")) {
	        mRoundNr = Integer.parseInt(value);
	    } else if (key.equals("move")) {
	        mMoveNr = Integer.parseInt(value);
	    } else if (key.equals("field")) {
            parseFromString(value); /* Parse Field with data */
        } else if (key.equals("macroboard")) {
            parseMacroboardFromString(value); /* Parse macroboard with data */
        }
	}

	public void parseFromString(String s) {
	    System.err.println("Move " + mMoveNr);
		s = s.replace(";", ",");
		String[] r = s.split(",");
		int counter = 0;
		for (int y = 0; y < ROWS; y++) {
			for (int x = 0; x < COLS; x++) {
				mBoard[x][y] = Integer.parseInt(r[counter]); 
				counter++;
			}
		}
	}
	

	public void parseMacroboardFromString(String s) {
		String[] r = s.split(",");
		int counter = 0;
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {
				mMacroboard[x][y] = Integer.parseInt(r[counter]);
				counter++;
			}
		}
	}
	
	public void clearBoard() {
		for (int x = 0; x < COLS; x++) {
			for (int y = 0; y < ROWS; y++) {
				mBoard[x][y] = 0;
			}
		}
	}

	public ArrayList<Move> getAvailableMoves() {
	    ArrayList<Move> moves = new ArrayList<Move>();
		
		for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < COLS; x++) {
                if (isInActiveMicroboard(x, y) && mBoard[x][y] == 0) {
                    moves.add(new Move(x, y));
                }
            }
        }

		return moves;
	}
	
	public Boolean isInActiveMicroboard(int x, int y) {
	    return mMacroboard[(int) x/3][(int) y/3] == -1;
	}
	


	public String getLastError() {
		return mLastError;
	}

	public String toString() {
		String r = "";
		int counter = 0;
		for (int y = 0; y < ROWS; y++) {
			for (int x = 0; x < COLS; x++) {
				if (counter > 0) {
					r += ",";
				}
				r += mBoard[x][y];
				counter++;
			}
		}
		return r;
	}
	

	public boolean isFull() {
		for (int x = 0; x < COLS; x++)
		  for (int y = 0; y < ROWS; y++)
		    if (mBoard[x][y] == 0)
		      return false; // At least one cell is not filled
		// All cells are filled
		return true;
	}
	
	public int getNrColumns() {
		return COLS;
	}
	
	public int getNrRows() {
		return ROWS;
	}

	public boolean isEmpty() {
		for (int x = 0; x < COLS; x++) {
			  for (int y = 0; y < ROWS; y++) {
				  if (mBoard[x][y] > 0) {
					  return false;
				  }
			  }
		}
		return true;
	}
	

	public int getPlayerId(int column, int row) {
		return mBoard[column][row];
	}

	public void printBoard() {
		for (int i = 0; i < 9; i++) {
			if( i == 3 || i == 6)
				System.out.println();
			for (int j = 0; j < 9; j++) {

				if( j == 3 || j == 6)
					System.out.print( "| " + mBoard[j][i]);
				else System.out.print(mBoard[j][i]);
				System.out.print(" ");
			}
			System.out.println();
		}
		System.out.println();
	}

	public void printMacro() {
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				System.out.print(mMacroboard[i][j] + " ");
			}
			System.out.println();
		}

	}
}