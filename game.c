#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define CLEAR "cls"
#else
#include <unistd.h>
#define CLEAR "clear"
#endif

#define PI 3.14159265358979323846
#define G 9.8
#define MAX_TRY 3
#define HIT_RANGE 2.0

#define WIDTH 70
#define HEIGHT 20

double degree_to_radian(double degree)
{
    return degree * PI / 180.0;
}

void delay_ms(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void init_map(char map[HEIGHT][WIDTH + 1], int target_x)
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            map[i][j] = ' ';
        }
        map[i][WIDTH] = '\0';
    }

    for (int j = 0; j < WIDTH; j++)
    {
        map[HEIGHT - 1][j] = '_';
    }

    for (int h = 0; h < 3; h++)
    {
        int row = HEIGHT - 2 - h;
        if (row >= 0 && target_x >= 0 && target_x < WIDTH)
        {
            map[row][target_x] = 'T';
        }
    }
}

void print_map(char map[HEIGHT][WIDTH + 1])
{
    for (int i = 0; i < HEIGHT; i++)
    {
        printf("%s\n", map[i]);
    }
}

double simulate_shot(double angle_deg, double speed, double target)
{
    double angle_rad = degree_to_radian(angle_deg);
    double vx = speed * cos(angle_rad);
    double vy = speed * sin(angle_rad);

    double t = 0.0;
    double dt = 0.12;
    double x, y;
    double last_x = 0.0;

    int target_screen_x = (int)(target / 1.5);
    char map[HEIGHT][WIDTH + 1];

    init_map(map, target_screen_x);

    while (1)
    {
        x = vx * t;
        y = vy * t - 0.5 * G * t * t;

        if (y < 0)
            break;

        int screen_x = (int)(x / 1.5);
        int screen_y = HEIGHT - 2 - (int)(y / 1.5);

        if (screen_x >= 0 && screen_x < WIDTH && screen_y >= 0 && screen_y < HEIGHT - 1)
        {
            if (map[screen_y][screen_x] == ' ')
            {
                map[screen_y][screen_x] = '.';
            }
        }

        system(CLEAR);

        printf("==============================================\n");
        printf("           Projectile Shooting Game\n");
        printf("==============================================\n");
        printf("Angle: %.1f deg | Speed: %.1f m/s | Target: %.1f m\n", angle_deg, speed, target);
        printf("Current Position -> x: %6.2f m, y: %6.2f m\n", x, y);
        printf("'.' = trail, '*' = projectile, 'T' = target\n");
        printf("==============================================\n\n");

        char frame[HEIGHT][WIDTH + 1];
        for (int i = 0; i < HEIGHT; i++)
        {
            strcpy(frame[i], map[i]);
        }

        if (screen_x >= 0 && screen_x < WIDTH && screen_y >= 0 && screen_y < HEIGHT - 1)
        {
            frame[screen_y][screen_x] = '*';
        }

        print_map(frame);

        delay_ms(150);

        last_x = x;
        t += dt;

        if (screen_x >= WIDTH)
            break;
    }

    int landing_x = (int)(last_x / 1.5);
    if (landing_x >= 0 && landing_x < WIDTH)
    {
        map[HEIGHT - 2][landing_x] = 'X';
    }

    system(CLEAR);
    printf("==============================================\n");
    printf("               Final Trajectory\n");
    printf("==============================================\n");
    printf("Target: %.1f m | Landing: %.2f m\n", target, last_x);
    printf("'.' = trajectory, 'X' = landing point, 'T' = target\n");
    printf("==============================================\n\n");
    print_map(map);

    return last_x;
}

int main()
{
    int round = 1;
    int score = 0;

    srand((unsigned int)time(NULL));

    printf("==============================================\n");
    printf("         Projectile Shooting Game (ASCII)\n");
    printf("==============================================\n");
    printf("Rules:\n");
    printf("1. Choose an angle and speed.\n");
    printf("2. The projectile follows a parabolic path.\n");
    printf("3. Hit the target within %.1f meters.\n", HIT_RANGE);
    printf("4. You have %d attempts.\n", MAX_TRY);
    printf("==============================================\n\n");

    while (round <= MAX_TRY)
    {
        double target = 30 + rand() % 61;  // 30 ~ 90
        double angle, speed;
        double landing, diff;

        printf("[ Attempt %d / %d ]\n", round, MAX_TRY);
        printf("Target Distance: %.1f m\n", target);

        printf("Enter angle (0 < angle < 90): ");
        scanf("%lf", &angle);

        printf("Enter speed (> 0): ");
        scanf("%lf", &speed);

        if (angle <= 0 || angle >= 90 || speed <= 0)
        {
            printf("Invalid input. Try again.\n\n");
            continue;
        }

        landing = simulate_shot(angle, speed, target);
        diff = fabs(landing - target);

        printf("\n==============================================\n");
        printf("Landing Position: %.2f m\n", landing);

        if (diff <= HIT_RANGE)
        {
            printf(">>> HIT!\n");
            printf("Difference: %.2f m\n", diff);
            score += 10;
        }
        else
        {
            printf(">>> MISS!\n");
            if (landing < target)
                printf("You fell short by %.2f m\n", target - landing);
            else
                printf("You overshot by %.2f m\n", landing - target);
        }

        printf("Current Score: %d\n", score);
        printf("==============================================\n");

        round++;

        if (round <= MAX_TRY)
        {
            printf("Press Enter to continue...");
            getchar();
            getchar();
            system(CLEAR);
        }
    }

    printf("\n==============================================\n");
    printf("Game Over!\n");
    printf("Final Score: %d\n", score);
    printf("==============================================\n");

    return 0;
}