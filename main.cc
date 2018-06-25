#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#define STANDARD_LENGTH 1000
#define MAX_LENGTH 10000
#define MAX_COUNT 100

char composers[MAX_COUNT][STANDARD_LENGTH], arrangers[MAX_COUNT][STANDARD_LENGTH], lyricists[MAX_COUNT][STANDARD_LENGTH];
int composers_count, arrangers_count, lyricists_count;

char ties[MAX_COUNT][STANDARD_LENGTH];
int ties_count;

class Entry {
private:
    int disc, track;
    char prefix[STANDARD_LENGTH], suffix[STANDARD_LENGTH];
    char tie[STANDARD_LENGTH];
    char composer[MAX_COUNT][STANDARD_LENGTH], arranger[MAX_COUNT][STANDARD_LENGTH], lyricist[MAX_COUNT][STANDARD_LENGTH];
    int composer_count, arranger_count, lyricist_count;

    static char *Combine(char source[][STANDARD_LENGTH], int count) {
        static char ret[STANDARD_LENGTH];
        memset(ret, '\0', STANDARD_LENGTH);
        strcpy(ret, source[0]);
        for (int i = 1; i < count; ++i) {
            strcat(ret, ", ");
            strcat(ret, source[i]);
        }
        return ret;
    }

    static void Split(int *count, char *source, char dist[][STANDARD_LENGTH]) {
        *count = 0;
        for (int i = 0 ; i < strlen(source); ++i)
            if (source[i] == ',')
                source[i] = '\0';
        char *now = source;
        while (strlen(now)) {
            strcpy(dist[(*count)++], now);
            now += strlen(now) + 2;
        }
    }

    static void ClearTrailings(char *s) {
        int length = strlen(s);
        while (s[length - 1] == ' ' || s[length - 1] == '\n' || s[length - 1] == 't') {
            s[length - 1] = '\0';
            --length;
        }
    }

    static void AddCredits(char source[][STANDARD_LENGTH], int source_count, char dest[][STANDARD_LENGTH], int *dest_count) {
        for (int i = 0; i < source_count; ++i) {
            bool included = false;
            for (int j = 0; j < *dest_count; ++j) {
                if (!strcmp(source[i], dest[j])) {
                    included = true;
                    break;
                }
            }
            if (!included)
                strcpy(dest[(*dest_count)++], source[i]);
        }
    }

    static void AddTie(char *source, char dest[][STANDARD_LENGTH], int *dest_count) {
        bool included = false;
        for (int i = 0; i < *dest_count; ++i)
            if (!strcmp(source, dest[i])) {
                included = true;
                break;
            }
        if (!included)
            strcpy(dest[(*dest_count)++], source);
    }

public:

    Entry() {
        disc = 0;
        track = 0;
        memset(prefix, '\0', STANDARD_LENGTH);
        memset(suffix, '\0', STANDARD_LENGTH);
        memset(tie, '\0', STANDARD_LENGTH);
        memset(composer, '\0', STANDARD_LENGTH * MAX_COUNT);
        memset(arranger, '\0', STANDARD_LENGTH * MAX_COUNT);
        memset(lyricist, '\0', STANDARD_LENGTH * MAX_COUNT);
        composer_count = 0;
        arranger_count = 0;
        lyricist_count = 0;
    }

    Entry(int _disc, int _track, char *_prefix, char *_suffix, char *_tie,
          char *_composer, char *_arranger, char *_lyricist) {
        disc = _disc;
        track = _track;
        ClearTrailings(_prefix);
        strcpy(prefix, _prefix);
        ClearTrailings(_suffix);
        strcpy(suffix, _suffix);
        ClearTrailings(_tie);
        strcpy(tie, _tie);
        ClearTrailings(_composer);
        Split(&composer_count, _composer, composer);
        ClearTrailings(_arranger);
        Split(&arranger_count, _arranger, arranger);
        ClearTrailings(_lyricist);
        Split(&lyricist_count, _lyricist, lyricist);
    }

    char *Output(bool display_disc = true) {
        static char ret[MAX_LENGTH];
        memset(ret, '\0', MAX_LENGTH);

        if (display_disc)
            sprintf(ret, "M-%d.%02d - %s \"%s\" %s\nComposer: %s\nArranger: %s\nLyricist: %s\n",
                    disc, track, prefix, tie, suffix,
                    Combine(composer, composer_count),
                    Combine(arranger, arranger_count),
                    Combine(lyricist, lyricist_count));
        else
            sprintf(ret, "M-%02d - %s \"%s\" %s\nComposer: %s\nArranger: %s\nLyricist: %s\n",
                    track, prefix, tie, suffix,
                    Combine(composer, composer_count),
                    Combine(arranger, arranger_count),
                    Combine(lyricist, lyricist_count));

        return ret;
    }

    static char *FinalOutput(Entry *entries, int count) {
        static char ret[MAX_LENGTH];
        memset(ret, '\0', MAX_LENGTH);

        bool multiple_disc = entries[count - 1].disc > 1;

        for (int i = 0; i < count; ++i) {
            strcat(ret, entries[i].Output(multiple_disc));
            strcat(ret, "\n");
            AddCredits(entries[i].composer, entries[i].composer_count,
                       composers, &composers_count);
            AddCredits(entries[i].arranger, entries[i].arranger_count,
                       arrangers, &arrangers_count);
            AddCredits(entries[i].lyricist, entries[i].lyricist_count,
                       lyricists, &lyricists_count);

            AddTie(entries[i].tie, ties, &ties_count);
        }
        printf("%d\n", composers_count);

        strcat(ret, "\nComposers:\n");
        strcat(ret, Combine(composers, composers_count));
        strcat(ret, "\nArrangers:\n");
        strcat(ret, Combine(arrangers, arrangers_count));
        strcat(ret, "\nLyricists:\n");
        strcat(ret, Combine(lyricists, lyricists_count));

        strcat(ret, "\n\nTie ups:\n");
        strcat(ret, Combine(ties, ties_count));

        return ret;
    }
};

Entry all_tracks[STANDARD_LENGTH];
int count;

bool CheckCmd(char *s)
{
    return (!strcmp(s, "y") || !strcmp(s, "Y"));
}

int main()
{
    char cmd[STANDARD_LENGTH];
    char storage[MAX_COUNT][MAX_LENGTH];
    char *prefix, *suffix, *tie;
    char *composer, *arranger, *lyricist;

    int pos = 0;
    int disc = 1, track = 0;

    size_t n = 0;

    // puts("VGMdb Editing Helper");
    while (printf("New disc? "), scanf("%s", cmd) != EOF) {

        int old_disc = disc;
        int old_track = track;

        if (CheckCmd(cmd))
            ++disc, track = 0;
        ++track;

        getchar();

        printf("Prefix: ");
        prefix = storage[++pos];
        getline(&prefix, &n, stdin);
        printf("Tie up: ");
        tie = storage[++pos];
        getline(&tie, &n, stdin);
        printf("Suffix: ");
        suffix = storage[++pos];
        getline(&suffix, &n, stdin);

        printf("Composer: ");
        composer = storage[++pos];
        getline(&composer, &n, stdin);
        printf("Arranger: ");
        arranger = storage[++pos];
        getline(&arranger, &n, stdin);
        printf("Lyricist: ");
        lyricist = storage[++pos];
        getline(&lyricist, &n, stdin);

        all_tracks[count] = Entry(disc, track, prefix, suffix, tie, composer, arranger, lyricist);

        puts("");
        puts(all_tracks[count].Output());

        printf("Correct? ");
        scanf("%s", cmd);
        getchar();

        if (CheckCmd(cmd))
            ++count;
        else
            disc = old_disc, track = old_track;
    }

    puts("\nFinished");
    puts(Entry :: FinalOutput(all_tracks, count));

    return EXIT_SUCCESS;
}
































