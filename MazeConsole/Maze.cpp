using namespace std;

#include "ConsoleEngine.h"
#include <stack>

class Console_Maze : public ConsoleTemplateEngine {
public:
	Console_Maze() {
		m_sAppName = L"Maze";
	}
private:
	int m_nMazeWidth;
	int m_nMazeHeight;
	// Array that stores values of all the cells, telling what each neighbor is
	int *m_maze;

	enum {
		CELL_PATH_N = 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
	};

	// Algorithm Variables
	int m_nVisitedCells = 0;
	stack<pair<int, int>> m_stack;
	// Scaling size of maze
	int m_nPathWidth;

protected:
	virtual bool OnUserCreate() {
		// Maze parameters
		m_nMazeWidth = 40;
		m_nMazeHeight = 25;

		m_maze = new int[m_nMazeWidth * m_nMazeHeight];
		// Set all maze coordinates to 0
		memset(m_maze, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));

		// Random starting cell
		int x = rand() % m_nMazeWidth;
		int y = rand() % m_nMazeHeight;
		m_stack.push(make_pair(x, y));
		m_maze[y * m_nMazeWidth + x] = CELL_VISITED;
		m_nVisitedCells = 1;
		
		m_nPathWidth = 3;

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		// Add Slow down for visual
		//this_thread::sleep_for(10ms);

		// lambda function to calculate the offset without having to type it constantly
		// Language forces code to where its needed using lambda over macro
		auto offset = [&](int x, int y) {
			return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
		};

		// ======================= Maze Algorithm ===================
		if (m_nVisitedCells < m_nMazeWidth * m_nMazeHeight) {
			// Step 1: Create a set of unvisited neightbours
			vector<int> neighbours;
			
			// North Neigh
			if (m_stack.top().second > 0 && (m_maze[offset(0, -1)] & CELL_VISITED) == 0)
				neighbours.push_back(0);
			// East Neigh
			if (m_stack.top().first < m_nMazeWidth - 1 && (m_maze[offset(1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(1);
			// South Neigh
			if (m_stack.top().second < m_nMazeHeight - 1 && (m_maze[offset(0, 1)] & CELL_VISITED) == 0)
				neighbours.push_back(2);
			// West Neigh
			if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(3);

			// Are there neightbours available, if no availabe, then pop stack
			if (!neighbours.empty()) {
				// if there are neighbours, choose at random
				int next_cell_dir = neighbours[rand() % neighbours.size()];

				//Create a path between neighbour and current cell
				switch (next_cell_dir) {
				case 0: // North
					// Save the CELL_PATH_N bit
					m_maze[offset(0, 0)] |= CELL_PATH_N;
					// Tell Northern neighbour there is a path South
					m_maze[offset(0, -1)] |= CELL_PATH_S;
					// Push top of stack, North neighbour coord
					m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
					break;
				case 1: // East
					m_maze[offset(0, 0)] |= CELL_PATH_E;
					m_maze[offset(1, 0)] |= CELL_PATH_W;
					m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
					break;
				case 2: // South
					m_maze[offset(0, 0)] |= CELL_PATH_S;
					m_maze[offset(0, 1)] |= CELL_PATH_N;
					m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
					break;
				case 3: //West
					m_maze[offset(0, 0)] |= CELL_PATH_W;
					m_maze[offset(-1, 0)] |= CELL_PATH_E;
					m_stack.push(make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
					break;
				}

				// New Cell
				m_maze[offset(0, 0)] |= CELL_VISITED;
				m_nVisitedCells++;
			}

			else {
				m_stack.pop(); // BackTrack
			}
		}
		
		// ========== DRAW ================

		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
		
		// Draw Maze
		for (int x = 0; x < m_nMazeWidth; x++) {
			for (int y = 0; y < m_nMazeHeight; y++) {
				// Inflate each cell
				for (int py = 0; py < m_nPathWidth; py++)
					for (int px = 0; px < m_nPathWidth; px++) {

						if (m_maze[y * m_nMazeWidth + x] & CELL_VISITED)
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_WHITE);
						else
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_BLUE);
					}

				// Draw passageways between cells
				for (int p = 0; p < m_nPathWidth; p++) {
					if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_S)
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth);

					if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_E)
						Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p);
				}
			}
		}

		// Draw Current Position
		for (int cy = 0; cy < m_nPathWidth; cy++)
			for (int cx = 0; cx < m_nPathWidth; cx++)
				Draw(m_stack.top().first * (m_nPathWidth + 1) + cx, m_stack.top().second * (m_nPathWidth + 1) + cy, PIXEL_SOLID, FG_GREEN);

		return true;
	}
};

int main() {
	srand(clock());

	// Use ConsoleEngine derived app
	Console_Maze game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();
	
	
	return 0;
}