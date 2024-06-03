#include "score.h"

char playerName[MAX_NAME_LENGTH + 1] = "";
int currentScore = 0;

int getHighestScore()
{
    int highScore;
    int count;
    HighScore highScores[MAX_HIGHSCORE];
    readHighScores(highScores, &count);

    if (count == 0)
    {
        highScore = 0;
    }
    else
    {
        highScore = highScores[0].score;
    }

    if (currentScore > highScore)
    {
        highScore = currentScore;
    }

    return highScore;
}
int compareHighScores(const void *a, const void *b)
{
    HighScore *scoreA = (HighScore *)a;
    HighScore *scoreB = (HighScore *)b;
    return scoreB->score - scoreA->score;
}

void sortHighScores(HighScore highScores[], int count)
{
    qsort(highScores, count, sizeof(HighScore), compareHighScores);
}

void writeHighScores(HighScore highScores[], int count)
{
    FILE *file = fopen("highscores.txt", "w");
    if (file == NULL)
    {
        return;
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%s %d\n", highScores[i].name, highScores[i].score);
    }

    fclose(file);
}

void readHighScores(HighScore highScores[], int *count)
{
    FILE *file = fopen("highscores.txt", "r");
    if (file == NULL)
    {
        *count = 0;
        return;
    }

    *count = 0;
    while (fscanf(file, "%s %d", highScores[*count].name, &highScores[*count].score) != EOF)
    {
        (*count)++;
        if (*count >= MAX_HIGHSCORE)
        {
            break;
        }
    }

    fclose(file);
}

void saveHighScore(const char *playerName, int score)
{
    HighScore highScores[MAX_HIGHSCORE + 1];
    int count;
    readHighScores(highScores, &count);

    strcpy(highScores[count].name, playerName);
    highScores[count].score = score;
    count++;

    sortHighScores(highScores, count);

    if (count > MAX_HIGHSCORE)
    {
        count = MAX_HIGHSCORE;
    }

    writeHighScores(highScores, count);
}