#include <windows.h>
#include <vector>
#include <random>

const int INSTRUCTION_WINDOW_WIDTH = 600;
const int INSTRUCTION_WINDOW_HEIGHT = 400;
const int GAME_WINDOW_WIDTH = 1000;
const int GAME_WINDOW_HEIGHT = 1000;
const int CELL_SIZE = 5;

using namespace std;

vector<vector<bool>> grid(GAME_WINDOW_WIDTH, vector<bool>(GAME_WINDOW_HEIGHT));


void InitializeGrid(vector<vector<bool>>& grid)
{
    // Seed the random number generator
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> dist(0, 1);

    // Initialize the grid with random values
    for (int i = 0; i < GAME_WINDOW_WIDTH; ++i)
    {
        for (int j = 0; j < GAME_WINDOW_WIDTH; ++j)
        {
            grid[i][j] = dist(rng) == 1;
        }
    }
}

void UpdateGameState(HWND hwnd)
{
    // Updating the grid based on the rules of Conway's Game of Life
    std::vector<std::vector<bool>> newGrid = grid;
    for (int i = 0; i < GAME_WINDOW_WIDTH; ++i)
    {
        for (int j = 0; j < GAME_WINDOW_WIDTH; ++j)
        {
            int liveNeighbors = 0;
            for (int di = -1; di <= 1; ++di)
            {
                for (int dj = -1; dj <= 1; ++dj)
                {
                    if (!(di == 0 && dj == 0) && i + di >= 0 && i + di < GAME_WINDOW_WIDTH && j + dj >= 0 && j + dj < GAME_WINDOW_WIDTH)
                    {
                        if (grid[i + di][j + dj])
                        {
                            liveNeighbors++;
                        }
                    }
                }
            }
            if (grid[i][j])
            {
                if (liveNeighbors < 2 || liveNeighbors > 3)
                {
                    newGrid[i][j] = false; // Cell dies due to underpopulation or overpopulation
                }
            }
            else
            {
                if (liveNeighbors == 3)
                {
                    newGrid[i][j] = true; // Cell is born due to reproduction
                }
            }
        }
    }
    grid = newGrid;
    InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
}

void DrawGrid(HDC hdc, HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 3));

    // Draw the grid
    for (int i = 0; i < GAME_WINDOW_WIDTH; ++i)
    {
        for (int j = 0; j < GAME_WINDOW_WIDTH; ++j)
        {
            if (grid[i][j])
            {
                RECT cellRect = { j * CELL_SIZE, i * CELL_SIZE, (j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE };
                FillRect(hdc, &cellRect, (HBRUSH)(COLOR_WINDOWFRAME + 1));
            }
        }
    }
}

LRESULT CALLBACK InstructionWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            // Creating the label for the game name
            CreateWindow(
                    "STATIC", "Conway's Game of Life",
                    WS_VISIBLE | WS_CHILD | SS_LEFT,
                    20, 20, INSTRUCTION_WINDOW_WIDTH - 60, 30,
                    hwnd, NULL, NULL, NULL
            );

            // Creating the label for the game rules
            CreateWindow(
                    "STATIC", "Instructions:\n- Any live cell with fewer than two live neighbours dies, as if caused by underpopulation."
                              "\n- Any live cell with two or three live neighbours lives on to the next generation."
                              "\n- Any live cell with more than three live neighbours dies, as if by overpopulation."
                              "\n- There will be 100 miliseconds timing between the generations "
                              "\n- To start the game close this window",
                    WS_VISIBLE | WS_CHILD | SS_LEFT,
                    20, 60, INSTRUCTION_WINDOW_WIDTH - 60, INSTRUCTION_WINDOW_HEIGHT - 150,
                    hwnd, NULL, NULL, NULL
            );

            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case 1:
                    // Closing the instruction window and open the game window
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        case WM_DESTROY:
            // Exit the application when the instruction window is closed
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK GameWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            SetTimer(hwnd, 1, 100, NULL); // Start the timer to update the grid every 100 milliseconds
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Clear the window
            DrawGrid(hdc, hwnd);

            EndPaint(hwnd, &ps);
            break;
        }

        case WM_TIMER:
        {
            UpdateGameState(hwnd);
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char* instructionClassName = "InstructionWindowClass";
    const char* gameClassName = "GameWindowClass";

    // Registering the window classes
    //instruction window
    WNDCLASS instructionWc = { 0 };
    instructionWc.lpfnWndProc = InstructionWindowProc;
    instructionWc.hInstance = hInstance;
    instructionWc.lpszClassName = instructionClassName;
    instructionWc.hbrBackground= (HBRUSH)(COLOR_WINDOW + 3);


    RegisterClass(&instructionWc);
    //game window
    WNDCLASS gameWc = { 0 };
    gameWc.lpfnWndProc = GameWindowProc;
    gameWc.hInstance = hInstance;
    gameWc.lpszClassName = gameClassName;
    gameWc.hbrBackground= (HBRUSH)(COLOR_WINDOW + 3);

    RegisterClass(&gameWc);

    //Creating instruction window
    HWND instructionHwnd = CreateWindow(
            instructionClassName,
            "Game Instructions",
            WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            INSTRUCTION_WINDOW_WIDTH,
            INSTRUCTION_WINDOW_HEIGHT,
            NULL,
            NULL,
            hInstance,
            NULL
    );

    if (instructionHwnd == NULL)
    {
        MessageBox(NULL, "Instruction Window creation failed!", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Show and update the instruction window
    ShowWindow(instructionHwnd, nCmdShow);
    UpdateWindow(instructionHwnd);

    //Main message loop for the instruction window to operating system
    MSG instructionMsg;
    while (GetMessage(&instructionMsg, NULL, 0, 0))
    {
        TranslateMessage(&instructionMsg);
        DispatchMessage(&instructionMsg);
    }

    // Create the game window if the instruction window was closed by clicking the button
    if (instructionMsg.message == WM_QUIT)
    {
        HWND gameHwnd = CreateWindow(
                gameClassName,
                "Conway's Game of Life",
                WS_OVERLAPPEDWINDOW ,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                GAME_WINDOW_WIDTH,
                GAME_WINDOW_HEIGHT,
                NULL,
                NULL,
                hInstance,
                NULL
        );

        if (gameHwnd == NULL)
        {
            MessageBox(NULL, "Game Window creation failed!", "Error", MB_OK | MB_ICONERROR);
            return 0;
        }

        InitializeGrid(grid);

        ShowWindow(gameHwnd, nCmdShow);
        UpdateWindow(gameHwnd);

        // Main message loop for the game window to the operating system
        MSG gameMsg;
        while (GetMessage(&gameMsg, NULL, 0, 0))
        {
            TranslateMessage(&gameMsg);
            DispatchMessage(&gameMsg);
        }
    }

    return static_cast<int>(instructionMsg.wParam);
}
