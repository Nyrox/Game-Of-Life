#include <vector>
#include <fstream>
#include <iostream>
#include <string>


const int BOARD_WIDTH = 20;
const int BOARD_HEIGHT = 30;

struct Cell {
	bool isAlive = false;
};

struct Generation {
	Cell cells[BOARD_WIDTH * BOARD_HEIGHT];

	// Print's the board to std out
	void printToCout() {
		for (int y = 0; y < BOARD_HEIGHT; y++) {
			for (int x = 0; x < BOARD_WIDTH; x++) {			
				std::cout << getCell(x, y).isAlive;
			}
			std::cout << "\n";
		}
	}

	// Get's the amount of neighbors that are alive
	int getAliveNeighborCount(int x, int y) {
		int count = 0;

		// Top row
		count += isCellAlive(x - 1, y - 1);
		count += isCellAlive(x, y - 1);
		count += isCellAlive(x + 1, y - 1);
	
		// Middle row, excluding itself
		count += isCellAlive(x - 1, y);
		count += isCellAlive(x + 1, y);
		
		// Bottom row
		count += isCellAlive(x - 1, y + 1);
		count += isCellAlive(x, y + 1);
		count += isCellAlive(x + 1, y + 1);

		return count;
	}

	// Get a cell by its x and y coords, does no bound checking
	Cell& getCell(int x, int y) {
		return cells[x + y * BOARD_WIDTH];
	}

private:
	// Checks if a given coordinate is out of bounds.
	bool isInBounds(int x, int y) {
		if (x < 0 || y < 0) return false;
		if (x >= BOARD_WIDTH || y >= BOARD_HEIGHT) return false;

		return true;
	}

	// Check if a cell is alive, includes bound checking
	bool isCellAlive(int x, int y) {
		if (!isInBounds(x, y)) return false;
		return getCell(x, y).isAlive;
	}



};

Generation loadGenerationFromFile(std::string filepath) {
	std::ifstream file(filepath);
	Generation generation;

	int x = 0; 
	int y = 0;
	while (true) {
		auto cell = file.get();
		// EOF
		if (cell == -1) break;
		// Ignore whitespace/tabs
		if (cell == '\n' || cell == '\r' || cell == '\t') continue;;
		
		bool cellAlive = (cell == '1') ? true : false;
		generation.getCell(x, y).isAlive = cellAlive;

		x++;
		if (x == BOARD_WIDTH) {
			x = 0;
			y++;
		}

		// We have enough characters to fill the board
		if (y >= BOARD_HEIGHT) break;
	}

	file.close();

	if (y != BOARD_HEIGHT) std::cout << "Not all cells were given! Setting rest to 0\n";

	return generation;
}

void saveGenerationToFile(std::string filepath, Generation generation) {
	std::ofstream file(filepath);
	
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			file.put(generation.getCell(x, y).isAlive ? '1' : '0');
		}
		file.put('\n');
	}

	file.close();
}

class GameOfLife {
public:
	void generateNextGeneration() {
		Generation next;
		Generation& last = generations.back();

		for (int y = 0; y < BOARD_HEIGHT; y++) {
			for (int x = 0; x < BOARD_WIDTH; x++) {
				/* Actual logic */
				int aliveNeighbors = last.getAliveNeighborCount(x, y);
				
				if (last.getCell(x, y).isAlive) {
					// Determine whether it survives
					if (aliveNeighbors < 2) {
						next.getCell(x, y).isAlive = false;
					}
					else if (aliveNeighbors < 4) {
						next.getCell(x, y).isAlive = true;
					}
					else {
						next.getCell(x, y).isAlive = false;
					}
				}
				else {
					if (aliveNeighbors == 3) {
						next.getCell(x, y).isAlive = true;
					}
				}
			}
		}

		generations.push_back(next);
	}

	std::vector<Generation> generations;
};

int main() {
	std::string filepath;
	std::cout << "Please enter the filename of a valid seed file [Default for autosave]: ";
	std::getline(std::cin, filepath);
	if (filepath.empty()) filepath = "autosave.txt";

	GameOfLife game;
	game.generations.push_back(loadGenerationFromFile(filepath));

	
	while (true) {
		game.generations.back().printToCout();
		std::cout << "[Enter=Continue, Save ~filepath=Save current seed to file, \"exit\"=Quit]\n";

		std::string input;
		std::getline(std::cin, input);

		if (input == "exit") break;
		if (input.find("save") != std::string::npos) {
			try {
				auto filename = input.substr(5);
				saveGenerationToFile(filename, game.generations.back());
			}
			catch (std::exception e) {
				std::cout << "Error: " << e.what() << "\n";
				std::cin.get();
			}
			continue;
		}
		if (!input.empty()) {
			std::cout << "I don't know that command...\n" << "\n";
			std::cin.get();
		}
		else {
			game.generateNextGeneration();
		}


		// Awful hack, but there isn't really an alternative other than printing a million newlines instead, so I opted for this.
		system("cls");

		
	}

	saveGenerationToFile("autosave.txt", game.generations.back());
}