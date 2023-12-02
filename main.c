#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// types
struct Command
{
    char *name;
    int (*cmd)(char *, char *);
};

// public variables
char **words;
char *word;
char *solution;
char *missed;
struct Command *cPalette;
struct Command *gameCommands;
struct Command *rootCommands;
int gameSize;
int rootSize;
int cPaletteSize;
int diff = 8;
int wordsSize = 0;
int chance;

// tools

void str_toLower(char *str)
{
    for (int i = 0; str[i]; i++)
        str[i] = tolower(str[i]);
}

void saveWords()
{
    FILE *fptr;

    fptr = fopen("words.txt", "w");

    if (fptr != NULL)
    {
        for (int i = 0; i < wordsSize; i++)
            fprintf(fptr, "%s\n", words[i]);
        printf("Hangman << Words are saved!\n");
    }
    else
    {
        printf("Hangman << Unable to write file!\n");
    }

    fclose(fptr);
}

void loadWords()
{
    char **buff = (char **)malloc(sizeof(char *) * (30 + 1));
    FILE *fptr;

    fptr = fopen("words.txt", "r");

    if (fptr == NULL)
    {
        printf("Hangman << Not able to open the words file.\n");
    }
    else
    {
        char line[30];
        while (fgets(line, 30, fptr))
        {
            for (int i = 0; i < 30; i++)
            {
                if (line[i] == '\n')
                {
                    line[i] = '\0';
                    break;
                }

                if (line[i] == '\0')
                    break;
            }
            buff[wordsSize] = (char *)malloc(sizeof(char) * (30 + 1));
            strcpy(buff[wordsSize], line);
            wordsSize++;
        }
    }

    words = buff;

    fclose(fptr);
}

// game commands
int gameExit()
{
    cPalette = rootCommands;
    cPaletteSize = rootSize;
    free(word);
    free(solution);
    free(missed);
    printf("Hangman << Exiting...\n");
    return 1;
}

bool check_end()
{
    bool eq = true;
    int i = 0;
    char c;
    while ((c = word[i++]) != '\0')
    {
        if (c != solution[i])
        {
            eq = false;
            break;
        }
    }

    return eq;
}

int gameEnd()
{
    system("clear");

    printf("\t--------------------------%s--------------------------\n", check_end() ? "Anyone can win in this game, so don't be so proud! :)" : "Sorry, but you're a loser :)");
    return gameExit();
}

void printGame()
{
    printf("\t--------------------------State--------------------------\n");
    printf("\tWord: ");
    int i = 0;
    char c;
    while ((c = word[i++]) != '\0')
        printf("%c", c == solution[i] ? c : '_');

    printf("\n\tMissed: ");
    i = 0;
    while ((c = missed[i++]) != '\0')
        printf("%c ", c);

    printf("\n\tChance: %d\n", chance);
    printf("\t---------------------------------------------------------\n");
}

int gameTip(char *par0, char *par1)
{
    if (par0[0] == '\0' || par1[0] != '\0')
    {
        printf("Hangman << Invalid input!\n");
        return 1;
    }

    if (strcmp(par0, word) == 0)
    {
        int i = 0;
        char c;
        while ((c = word[i++]) != '\0')
        {
            solution[i] = c;
        }

        return gameEnd();
    }
    else
    {
        system("clear");
        chance--;
        printGame();
    }

    return 1;
}

int gameTry(char *par0, char *par1)
{
    if (par0[0] == '\0' || par0[1] != '\0' || par1[0] != '\0')
    {
        printf("Hangman << Invalid input!\n");
        return 1;
    }

    system("clear");
    bool contains = false;
    char c;
    int i = 0;
    while ((c = word[i++]) != '\0')
    {
        if (c == par0[0])
        {
            if (solution[i] != c)
            {
                solution[i] = c;
            }
            else
            {
                printf("Hangman << This character already found!\n");
                break;
            }

            contains = true;
        }
    }

    if (!contains)
    {
        bool inMissed = false;
        i = 0;
        while ((c = missed[i++]) != '\0')
        {
            if (c == par0[0])
            {
                inMissed = true;
            }
        }

        if (!inMissed)
        {
            missed[diff - chance] = par0[0];
        }
        chance--;
    }

    if (!check_end() && chance > 0)
    {
        printGame();
        return 1;
    }

    return gameEnd();
}

int gameHelp()
{
    system("clear");
    printf("\ttry (y) -> try the (y) character in the word\n");
    printf("\ttip (word) -> try to figure out the full word\n");
    printf("\tend -> prints the solution of the game\n");
    printf("\texit -> exit from game immediately\n\n");
    printGame();
    return 1;
}

// root commands
int rootDiff(char *par0, char *par1)
{
    if (par1[0] != '\0' && par0[0] == '\0')
    {
        printf("Hangman << Something is wrong with the parameters!\n");
    }
    else
    {
        if (sscanf(par0, "%d", &diff) == 1)
        {
            if (diff > 30)
                diff = 30;

            if (diff <= 0)
                diff = 1;
        }
        else
        {
            printf("Hangman << You're du... not so smart, your num is not a num!\n");
        }
    }

    return 1;
}

int rootHelp()
{
    printf("\tdiff (num) -> oppertunity to miss a character (default 8)\n");
    printf("\tlist [add|remove (word)] -> manage words.txt file\n");
    printf("\tplay (word) -> start a game with a random word or a specific one\n");
    printf("\texit -> exit from app\n");
    return 1;
}

int rootPlay(char *par0, char *par1)
{
    if (par1[0] != '\0')
    {
        printf("Hangman << Invalid input!\n");
        return 1;
    }

    system("clear");
    printf("Hangman << Game is started... Good luck!\n");

    solution = (char *)malloc(sizeof(char) * 30);
    word = (char *)malloc(sizeof(char) * 30);
    missed = (char *)malloc(sizeof(char) * diff);

    int index = 0;
    if (par0[0] == '\0')
    {
        index = (rand() % (wordsSize - 1 + 1)) + 1;
        strcpy(word, words[index - 1]);
    }
    else
    {
        strcpy(word, par0);
    }
    chance = diff;
    cPalette = gameCommands;
    cPaletteSize = gameSize;

    printGame();

    return 1;
}

int rootExit()
{
    return 0;
}

int rootList(char *par0, char *par1)
{
    if (strcmp(par0, "add") == 0)
    {
        bool contains = false;
        for (int i = 0; i < wordsSize; i++)
            if (strcmp(par1, words[i]) == 0)
                contains = true;

        if (contains)
        {
            printf("Hangman << The word is already in the file\n");
        }
        else
        {
            if (wordsSize < 30)
            {
                words[wordsSize] = (char *)malloc(sizeof(char) * 30);
                strcpy(words[wordsSize], par1);
                wordsSize++;
                saveWords();
            }
        }
    }
    else if (strcmp(par0, "remove") == 0)
    {
        int index = -1;
        for (int i = 0; i < wordsSize; i++)
            if (strcmp(par1, words[i]) == 0){
                index = i;
                break;
            }

        if (index != -1)
        {
            for (int i = index; i < wordsSize - 1; i++)
                strcpy(words[i], words[i + 1]);

            wordsSize--;
            saveWords();
        }
        else
        {
            printf("Hangman << There is no word like \"%s\" in the file!\n", par1);
        }
    }
    else
    {
        printf("Hangman << words that stored in file (%d): \n\t", wordsSize);
        for (int i = 0; i < wordsSize; i++)
            printf("%s ", words[i]);
        printf("\n");
    }

    return 1;
};

// layers
int process(char *input)
{

    char cmd[15];
    char par0[10];
    char par1[25];

    int readed = sscanf(input, "%s %s %s", cmd, par0, par1);

    if (readed > 0)
    {
        for (int i = 0; i < cPaletteSize; i++)
        {
            if (strcmp(cmd, cPalette[i].name) == 0)
                return cPalette[i].cmd(par0, par1);
        }
    }

    printf("Hangman << Missing or misspelled command!\n");

    return 1;
}

void initCommands()
{
    static struct Command gameCommandsDef[] = {
        {"tip", &gameTip},
        {"try", &gameTry},
        {"exit", &gameExit},
        {"end", &gameEnd},
        {"help", &gameHelp},
        {"?", &gameHelp}};
    gameSize = sizeof(gameCommandsDef) / sizeof(struct Command);
    gameCommands = gameCommandsDef;

    static struct Command rootCommandsDef[] = {
        {"?", &rootHelp},
        {"help", &rootHelp},
        {"exit", &rootExit},
        {"list", &rootList},
        {"play", &rootPlay},
        {"diff", &rootDiff}};
    rootSize = sizeof(rootCommandsDef) / sizeof(struct Command);
    rootCommands = rootCommandsDef;

    cPalette = rootCommands;
    cPaletteSize = rootSize;
}

// main
int main(int argc, char const *argv[])
{
    int exit = 1;
    char input[50];

    printf("Initialize hangman...\n");
    initCommands();
    loadWords();
    srand(time(0));

    while (exit)
    {
        printf("Hangman >> ");
        fgets(input, 50, stdin);
        str_toLower(input);
        exit = process(input);
    }

    printf("Hope you enjoy the game");

    return exit;
}
