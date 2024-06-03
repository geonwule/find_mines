#include <iostream>
#include <ctime>   // time
#include <cstdlib> // rand

// #define WIDTH 9
// #define HEIGHT 9
// #define MINE_NUM 10

ssize_t WIDTH, HEIGHT, MINE_NUM;

enum e_map_element
{
    EMPTY = 0,
    MINE,
    MARK,
};

struct Maps
{
    e_map_element type;
    bool is_clicked;
    int surrounded_mine_num;
};

// int maps[HEIGHT][WIDTH] = {
//     EMPTY,
// };
// int marks[HEIGHT][WIDTH] = {
//     EMPTY,
// };
// Maps info[HEIGHT][WIDTH];

#include <vector>
std::vector<std::vector<int>> maps;
std::vector<std::vector<int>> marks;
std::vector<std::vector<Maps>> info;

void print_maps()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            std::cout << maps[y][x] << " ";
        }
        std::cout << std::endl;
    }
}

void print_game_maps(bool end_game = false)
{
    std::cout << "y\\x";
    for (int x = 0; x < WIDTH; x++)
        std::cout << x << " ";
    std::cout << "\n";

    for (int y = 0; y < HEIGHT; y++)
    {
        std::cout << y << ": ";
        for (int x = 0; x < WIDTH; x++)
        {
            if (end_game && info[y][x].type == MINE)
            {
                std::cout << "✹ ";
                continue;
            }

            if (marks[y][x] == MARK)
                std::cout << "✔︎ ";
            else if (info[y][x].is_clicked == false)
                std::cout << "◻️ ";
            else
            {
                if (info[y][x].type == MINE)
                    std::cout << "✹ ";
                else
                {
                    if (info[y][x].surrounded_mine_num)
                        std::cout << info[y][x].surrounded_mine_num << " ";
                    else
                        std::cout << ". ";
                }
            }
        }
        std::cout << std::endl;
    }
}

void generate_mine()
{
    {
        int mine_cnt = 0;
        while (mine_cnt < MINE_NUM)
        {
            for (int y = 0; y < HEIGHT; y++)
            {
                for (int x = 0; x < WIDTH; x++)
                {
                    if (maps[y][x] == EMPTY && mine_cnt < MINE_NUM && rand() % 10 == 0)
                    {
                        maps[y][x] = MINE;
                        mine_cnt++;
                    }
                    info[y][x].type = (e_map_element)maps[y][x];
                }
            }
        }
    }
}

#include <sstream>
int input(int &x, int &y)
{
    std::cout << "input (x, y) :";
    std::string str;
    std::getline(std::cin, str);
    std::stringstream ss(str);
    char sep;
    ss >> x >> sep >> y;
    if (ss.fail() || x >= WIDTH || y >= HEIGHT)
    {
        std::cerr << "input error\n";
        return -1;
    }
    if (sep == 'm')
    {
        if (info[y][x].is_clicked)
            return 1;
        if (marks[y][x] == EMPTY)
            marks[y][x] = MARK;
        else
            marks[y][x] = EMPTY;
        return 1;
    }
    std::cout << "input " << "(" << x << "," << y << ")\n";
    return 0;
}

int click_maps(const int x, const int y);

void calculate_surrounded_mine(const int x, const int y)
{
    Maps &cur = info[y][x];

    static int dx[] = {1, -1, 0, 0, -1, -1, 1, 1};
    static int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};

    int mine_cnt = 0;
    for (int i = 0; i < 8; i++)
    {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (ny < 0 || nx < 0 || nx >= WIDTH || ny >= HEIGHT || maps[ny][nx] == EMPTY)
            continue;
        mine_cnt++;
    }

    cur.surrounded_mine_num = mine_cnt;

    if (!mine_cnt)
    {
        for (int i = 0; i < 8; i++)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (ny < 0 || nx < 0 || nx >= WIDTH || ny >= HEIGHT || maps[ny][nx] == MINE)
                continue;
            
            click_maps(nx, ny);
        }
    }
}

int click_num = 0;

int click_maps(const int x, const int y)
{
    if (info[y][x].is_clicked)
        return 0;

    info[y][x].is_clicked = true;
    click_num++;
    e_map_element type = info[y][x].type;

    switch (type)
    {
    case EMPTY:
        calculate_surrounded_mine(x, y);
        return 0;

    case MINE:
        return -1;

    default:
        return 0;
    }
}

int main(int ac, char **av)
{
    if (ac != 4)
    {
        std::cerr << "input <WIDTH> <HEIGHT> <MINE_COUNT>" << std::endl;
        return 1;
    }

    WIDTH = atoi(av[1]);
    HEIGHT = atoi(av[2]);
    MINE_NUM = atoi(av[3]);

    if (WIDTH < 1 || HEIGHT < 1 || MINE_NUM < 1 || MINE_NUM > (WIDTH * HEIGHT / 2) )
    {
        std::cerr << "have to input : WIDTH > 1, HEIGHT > 1, 1 < MINE_COUNT <= WIDTH * HEIGHT / 2" << std::endl;
        return 1;
    }

    // std::vector<std::vector<int>> maps;
    // std::vector<std::vector<int>> marks;
    // std::vector<std::vector<Maps>> info;
    maps.resize(HEIGHT);
    marks.resize(HEIGHT);
    info.resize(HEIGHT);

    for (int i = 0; i < HEIGHT; i++)
    {
        maps[i].resize(WIDTH, EMPTY);
        marks[i].resize(WIDTH, EMPTY);
        info[i].resize(WIDTH);
    }

    // srand
    srand(time(NULL));

    generate_mine();
    std::cout << "-----answer----\n";
    print_maps();
    std::cout << "\n\n";

    // game loop
    bool is_success = false;
    while (true)
    {
        print_game_maps();
        int x, y;
        if (input(x, y))
            continue;
        if (click_maps(x, y))
            break;
        if (WIDTH * HEIGHT - click_num == MINE_NUM)
        {
            is_success = true;
            break;
        }
    }
    print_game_maps(true);

    if (is_success)
        std::cout << "Success!!\n";
    else
        std::cout << "Fail\n";
    return 0;
}