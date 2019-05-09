using namespace std;

#include "ConsoleEngine.h"

class Console_Maze : public ConsoleTemplateEngine {
public:
	Console_Maze() {
		m_sAppName = L"Maze";
	}
private:
	int m_nMazeWidth;
	int m_nMazeHeight;
	int *m_maze;

protected:
	virtual bool OnUserCreate() {
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		return true;
	}
};

int main() {
	// Use ConsoleEngine derived app
	Console_Maze game;
	game.ConstructConsole(80, 50, 8, 8);
	game.Start();
	
	
	return 0;
}