#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

///////////////////////////////////////////////////////
void AddEdge(vector<vector<int>>& vect,const int m,const int n) {
	vect[m].push_back(n);
	vect[n].push_back(m);
}
vector<vector<int>> CreateAdjencyMatrix(const int n, const int m, const int* arr) {
	vector<vector<int>> adjMtr(n * m);
	
	constexpr int neutral = -1;
	
	for (int i = 0; i < n*m; i++)
	{
		if (arr[i] == neutral)
		{
			continue;
		}
		AddEdge(adjMtr, i, arr[i]);
	}
	return adjMtr;
}

///////////////////////////////////////////////////////
//Drawing Labyrinth
void DrawingLines(const int columns, const bool haveEntrance) {
	//Drawing Top and Bottom wall
	for (int j = 0; j < columns; ++j) {
		if (j == 0 && haveEntrance)
		{
			cout << "+ ";
			continue;
		}
		cout << "+-";
	}
	cout << "+" << endl;
}
void DrawingInnerLines(const int columns,const char* firstParsing,const char* secondParsing, const int currentRow, const vector<vector<int>> adjMatrix,const int ruleOfSearching, const bool needsIndex) {
	for (int i = 0; i < columns; i++)
	{
		const int indexOfBox = (columns * currentRow) + i;
		const int searched = indexOfBox + ruleOfSearching;
		auto haveNeigbor = find(adjMatrix[indexOfBox].begin(), adjMatrix[indexOfBox].end(), searched);
		/*if (needsIndex)
		{
			//If created with indexes you need to see the biggest number to make the spacing right
			char indexOfBox = indexOfBox;
		}*/
		if (haveNeigbor != adjMatrix[indexOfBox].end())
		{
			if (i == (columns - 1))
			{
				cout << firstParsing[0];
			}
			else
			{
				cout << firstParsing;
			}
		}
		else
		{
			if (i == (columns - 1))
			{
				cout << secondParsing[0];
			}
			else
			{
				cout << secondParsing;
			}
		}
	}
}
void DrawLabyrinth(const vector<vector<int>> adjMatrix, const int rows, const int colums) {
    
	const char firstParsing[] = " |";
	const char secondParsing[] = "  ";
	const char thirdParsing[] = "-+";
	const char forthParsing[] = " +";
	const bool needsIndex = true;

	DrawingLines(colums, true);
	// Draw the side and lower walls
	for (int i = 0; i < rows; ++i) {
	
		//Draw the side walls
		cout << "|";
		DrawingInnerLines(colums, secondParsing, firstParsing, i,adjMatrix, 1, needsIndex);
		
		//For the down right corner
		if (i == (rows - 1))
		{
			cout << " " << endl;
			continue;
		}
		cout << "|" << endl;

		//Draws lower walls 
		cout << "|";
		DrawingInnerLines(colums, forthParsing, thirdParsing, i, adjMatrix, colums, !needsIndex);
		cout << "|" << endl;

	}
	//bottom line
	DrawingLines(colums, false);
}
///////////////////////////////////////////////////////
void PathFindingNaiveWay(const int current,const int goal, vector<bool>& used, vector<int>& indexesOfPath, const vector<vector<int>> adjMatrix)
{
	used[current] = true;
	indexesOfPath.push_back(current);

	//Goal found or last element of the path is the goal
	if (current == goal)
	{
		return;
	}
	
	for (size_t i = 0; i < adjMatrix[current].size(); i++)
	{
		int nextIndex = adjMatrix[current][i];
		if (!used[nextIndex])
		{

			PathFindingNaiveWay(nextIndex, goal, used, indexesOfPath, adjMatrix);
			if (indexesOfPath.back() == goal) // If goal found in the path, stop exploring
				return;
		}
	}

	//If all of his neighbors are already used
	indexesOfPath.pop_back();
	return;
}
void PrintPath(const vector<int> path) {
	cout << "The path in the labyrinth is: ";
	for (size_t i = 0; i < path.size(); i++)
	{
		cout << path[i] << " ";
	}
	cout << endl;
}
///////////////////////////////////////////////////////
int CharToInt(const char symbol) {
	if (symbol < '0' && symbol > '9')
	{
		return -1;
	}
	return symbol - '0';
}
int* ReadingIntArrayFromFile(const string arr, const int size) {
	int* predecessors = new int[size];
	unsigned int indexOfPredecessorArr = 1;
	//Hardcoded with zero being always the first element
	predecessors[0] = -1;
	//Needs function for skipping spaces if needed
	//i is from 3 believing that the file's elements are spaced with only one space
	for (size_t i = 3; i < arr.size(); i += 2)
	{

		predecessors[indexOfPredecessorArr] = CharToInt(arr[i]);
		
		//If number is bigger than 9
		int multiply = 1;
		while (arr[i+1] != ' ')
		{
			predecessors[indexOfPredecessorArr] = predecessors[indexOfPredecessorArr] * pow(10, multiply) + CharToInt(arr[i + 1]);
			i++;
		}

		indexOfPredecessorArr++;
	}
	return predecessors;
}
///////////////////////////////////////////////////////
int main() {

	vector<string> lineOfTheFile;
	unsigned int indexFileLine = 0;
	
	/*string fileDirectory;
	cout << "Type the file directory: ";
	cin >> fileDirectory;*/

	//path is hardcoded needs function for adding double "\\"
	ifstream inputFile("./File.txt");

	if (inputFile.is_open())
	{
		//Also hardcoded for specific task
		int linesInFile = 3;
		lineOfTheFile.resize(linesInFile);
		while (getline(inputFile, lineOfTheFile[indexFileLine]))
		{
			indexFileLine++;
			//cout << lineOfTheFile[indexFileLine] << endl;
		}
		inputFile.close();
	}
	else
	{
		cerr << "There was problem with opening the file" << endl;
        return -1;
    }
	//This is preventing the ReadingIntArrayFromFile function to crash
	lineOfTheFile[indexFileLine - 2] += " ";

	////////////////////////////////////
    //first row in the file are the size of the maze
	int rows = CharToInt(lineOfTheFile[0][2]);
	int columns = CharToInt(lineOfTheFile[0][0]);
    //third row in the file are start and finish
    int start = CharToInt(lineOfTheFile[2][0]);
	int goal = CharToInt(lineOfTheFile[2][2]);
	
    //second row of the file is the predecessor array 
    int* predecessorArray = ReadingIntArrayFromFile(lineOfTheFile[1], rows * columns);

	vector<vector<int>> adjMatrix = CreateAdjencyMatrix(rows, columns, predecessorArray);
	
	////////////////////////////////////
	cout << "Printing the adjency matrix: " << endl;
	for (size_t i = 0; i < rows * columns; i++)
	{
		cout << "Adjency vertex of " << i << " are: ";
		for (int v : adjMatrix[i])
		{
			cout << v << " ";
		}
		cout << endl;
	}

    ////////////////////////////////////
    DrawLabyrinth(adjMatrix, rows, columns);
	
	////////////////////////////////////
	vector<int> path;
	vector<bool> used;
	used.resize(rows * columns);
	fill(used.begin(), used.end(), false);
	//
	PathFindingNaiveWay(start, goal, used, path, adjMatrix);
	
	PrintPath(path);
	////////////////////////////////////
	
	delete[]predecessorArray;
	return 0;
	
}

