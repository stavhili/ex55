/***********
 ID:211894092
 NAME:Stav Hili Shitrit
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Episode {
    char *name;
    char *length;
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

TVShow ***database = NULL;
int dbSize = 0;

char *getString();
int getInt();

int validLength(char *lengthString);
int countShows();

void shrinkDB();
void expandDB();

void freeEpisode(Episode *episodeToFree);
void freeSeason(Season *seasonToFree);
void freeShow(TVShow *showToFree);
void freeAll();

TVShow *findShow(char *showName);
Season *findSeason(TVShow *show, char *seasonName);
Episode *findEpisode(Season *season, char *episodeName);

void addShow();
void addSeason();
void addEpisode();

void deleteShow();
void deleteSeason();
void deleteEpisode();

void printEpisode();
void printShow();
void printArray();

void addMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: addShow(); break;
        case 2: addSeason(); break;
        case 3: addEpisode(); break;
    }
}

void deleteMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: deleteShow(); break;
        case 2: deleteSeason(); break;
        case 3: deleteEpisode(); break;
    }
}

void printMenuSub() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: printShow(); break;
        case 2: printEpisode(); break;
        case 3: printArray(); break;
    }
}

void mainMenu() {
    printf("Choose an option:\n");
    printf("1. Add\n");
    printf("2. Delete\n");
    printf("3. Print\n");
    printf("4. Exit\n");
}

int main() {
    int choice;
    do {
        mainMenu();
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1: addMenu(); break;
            case 2: deleteMenu(); break;
            case 3: printMenuSub(); break;
            case 4: freeAll(); break;
        }
    } while (choice != 4);
    return 0;
}

char *getString() {
    int capacity = 16;
    int length = 0;
    char *inputString = (char *)malloc(capacity);
    if (!inputString) exit(1);

    int currentChar;
    while ((currentChar = getchar()) != '\n') {
        if (length + 1 >= capacity) {
            capacity *= 2;
            char *resizedString = (char *)realloc(inputString, capacity);
            if (!resizedString) {
                free(inputString);
                exit(1);
            }
            inputString = resizedString;
        }
        inputString[length++] = (char)currentChar;
    }

    inputString[length] = '\0';
    return inputString;

}
int getInt(){
    int number;
    scanf("%d", &number);
    getchar();
    return number;
}

int validLength(char *lengthString){
    // Check length is exactly 8 characters
    if (strlen(lengthString) != 8) {
        return 0;
    }
    // Check colons at positions 2 and 5
    if (lengthString[2] != ':' || lengthString[5] != ':') {
        return 0;
    }
    // Check all other characters are digits
    for (int i = 0; i < 8; i++) {
        if (i == 2 || i == 5) continue;
        if (lengthString[i] < '0' || lengthString[i] > '9') {
            return 0;
        }
    }
    // Convert and validate ranges
    int hours = (lengthString[0] - '0') * 10 + (lengthString[1] - '0');
    int minutes = (lengthString[3] - '0') * 10 + (lengthString[4] - '0');
    int seconds = (lengthString[6] - '0') * 10 + (lengthString[7] - '0');
    // Validate: hours 0-99, minutes and seconds 0-59
    if (hours > 99 || minutes > 59 || seconds > 59) {
        return 0;
    }
    return 1;
}
int countShows(){
    int totalShows = 0;

    // Iterate through 2D array
    for (int row = 0; row < dbSize; row++) {
        for (int col = 0; col < dbSize; col++) {
            // Count non-NULL cells
            if (database[row][col] != NULL) {
                totalShows++;
            }
        }
    }

    return totalShows;
}

void shrinkDB() {
    // Don't shrink if 1x1 or empty
    if (dbSize <= 1) {
        // Free everything if 1x1 and empty
        if (dbSize == 1 && database[0][0] == NULL) {
            free(database[0]);
            free(database);
            database = NULL;
            dbSize = 0;
        }
        return;
    }
    int newSize = dbSize - 1;
    // Collect remaining shows in temporary list
    int totalShows = countShows();
    if (totalShows == 0) {
        for (int row = 0; row < dbSize; row++) {
            free(database[row]);
        }
        free(database);
        database = NULL;
        dbSize = 0;
        return;
    }
    TVShow **tempShowsList = (TVShow **)malloc(totalShows * sizeof(TVShow *));
    if (tempShowsList == NULL) exit(1);
    int showIndex = 0;
    for (int row = 0; row < dbSize; row++) {
        for (int col = 0; col < dbSize; col++) {
            if (database[row][col] != NULL) {
                tempShowsList[showIndex++] = database[row][col];
            }
        }
    }
    // Free old array
    for (int row = 0; row < dbSize; row++) {
        free(database[row]);
    }
    free(database);
    // Allocate new smaller array
    database = (TVShow ***)malloc(newSize * sizeof(TVShow **));
    if (database == NULL) exit(1);
    for (int row = 0; row < newSize; row++) {
        database[row] = (TVShow **)malloc(newSize * sizeof(TVShow *));
        if (database[row] == NULL) exit(1);

        // Initialize with NULL
        for (int col = 0; col < newSize; col++) {
            database[row][col] = NULL;
        }
    }
    // Put shows back in sequence
    showIndex = 0;
    for (int row = 0; row < newSize && showIndex < totalShows; row++) {
        for (int col = 0; col < newSize && showIndex < totalShows; col++) {
            database[row][col] = tempShowsList[showIndex++];
        }
    }
    // Free temporary list
    free(tempShowsList);
    // Update size
    dbSize = newSize;
}

void expandDB(){
    int oldSize = dbSize;
    int newSize = dbSize + 1;

    int totalShows = countShows();
    TVShow **tempShowsList = NULL;

    if (totalShows > 0) {
        tempShowsList = (TVShow **)malloc(totalShows * sizeof(TVShow *));
        if (tempShowsList == NULL) exit(1);

        int idx = 0;
        for (int row = 0; row < oldSize; row++) {
            for (int col = 0; col < oldSize; col++) {
                if (database[row][col] != NULL) {
                    tempShowsList[idx++] = database[row][col];
                }
            }
        }
    }

    // free old grid (rows only, not shows)
    if (database != NULL) {
        for (int row = 0; row < oldSize; row++) {
            free(database[row]);
        }
        free(database);
    }

    // allocate new grid
    database = (TVShow ***)malloc(newSize * sizeof(TVShow **));
    if (database == NULL) exit(1);

    for (int row = 0; row < newSize; row++) {
        database[row] = (TVShow **)malloc(newSize * sizeof(TVShow *));
        if (database[row] == NULL) exit(1);
        for (int col = 0; col < newSize; col++) {
            database[row][col] = NULL;
        }
    }

    // refill packed row-major
    int showIndex = 0;
    for (int row = 0; row < newSize && showIndex < totalShows; row++) {
        for (int col = 0; col < newSize && showIndex < totalShows; col++) {
            database[row][col] = tempShowsList[showIndex++];
        }
    }

    free(tempShowsList);
    dbSize = newSize;
}

void freeEpisode(Episode *episodeToFree) {
    if (episodeToFree == NULL) return;
    free(episodeToFree->name);
    free(episodeToFree->length);
    free(episodeToFree);
}
void freeSeason(Season *seasonToFree){
    if (seasonToFree == NULL) {
        return;
    }

    // Free all episodes in linked list
    Episode *currentEpisode = seasonToFree->episodes;
    while (currentEpisode != NULL) {
        Episode *episodeToDelete = currentEpisode;
        currentEpisode = currentEpisode->next;
        freeEpisode(episodeToDelete);
    }

    // Free season name
    free(seasonToFree->name);

    // Free structure itself
    free(seasonToFree);
}

void freeShow(TVShow *showToFree){
    if (showToFree == NULL) {
        return;
    }

    // Free all seasons in linked list
    Season *currentSeason = showToFree->seasons;
    while (currentSeason != NULL) {
        Season *seasonToDelete = currentSeason;
        currentSeason = currentSeason->next;
        freeSeason(seasonToDelete);
    }

    // Free show name
    free(showToFree->name);

    // Free structure itself
    free(showToFree);
}

void freeAll(){
    if (database == NULL) {
        return;
    }

    // Free all shows in array
    for (int row = 0; row < dbSize; row++) {
        for (int col = 0; col < dbSize; col++) {
            if (database[row][col] != NULL) {
                freeShow(database[row][col]);
            }
        }
    }

    // Free each row
    for (int row = 0; row < dbSize; row++) {
        free(database[row]);
    }

    // Free main array
    free(database);

    // Reset globals
    database = NULL;
    dbSize = 0;
}

TVShow *findShow(char *showName){
    // Iterate through 2D array
    for (int row = 0; row < dbSize; row++) {
        for (int col = 0; col < dbSize; col++) {
            // Check cell not empty and name matches
            if (database[row][col] != NULL &&
                strcmp(database[row][col]->name, showName) == 0) {
                return database[row][col];
                }
        }
    }
    // Not found
    return NULL;
}
Season *findSeason(TVShow *show, char *seasonName){
    if (show == NULL) {
        return NULL;
    }
    // Iterate through seasons linked list
    Season *currentSeason = show->seasons;
    while (currentSeason != NULL) {
        if (strcmp(currentSeason->name, seasonName) == 0) {
            return currentSeason;
        }
        currentSeason = currentSeason->next;
    }
    // Not found
    return NULL;
}
Episode *findEpisode(Season *season, char *episodeName){
    if (season == NULL) {
        return NULL;
    }
    // Iterate through episodes linked list
    Episode *currentEpisode = season->episodes;
    while (currentEpisode != NULL) {
        if (strcmp(currentEpisode->name, episodeName) == 0) {
            return currentEpisode;
        }
        currentEpisode = currentEpisode->next;
    }
    // Not found
    return NULL;
}

void addShow(){
    printf("Enter the name of the show:\n");
    char *showName = getString();
    // Check if show already exists
    if (findShow(showName) != NULL) {
        printf("Show already exists.\n");
        free(showName);
        return;
    }
    // Create new show
    TVShow *newShow = (TVShow *)malloc(sizeof(TVShow));
    if (newShow == NULL) exit(1);
    newShow->name = showName;
    newShow->seasons = NULL;
    // Check if array is full
    int currentShowCount = countShows();
    if (currentShowCount == dbSize * dbSize) {
        expandDB();
    }
    // Find correct position for insertion (alphabetical order)
    int insertRow = -1, insertCol = -1;
    int foundPosition = 0;
    for (int row = 0; row < dbSize && !foundPosition; row++) {
        for (int col = 0; col < dbSize && !foundPosition; col++) {
            if (database[row][col] == NULL) {
                // Found empty cell
                insertRow = row;
                insertCol = col;
                foundPosition = 1;
            } else if (strcmp(showName, database[row][col]->name) < 0) {
                // New show should be before current show
                insertRow = row;
                insertCol = col;
                foundPosition = 1;
            }
        }
    }
    // Shift all shows right from found position (safe shift, no duplicates)
    int insertIndex = insertRow * dbSize + insertCol;
    int lastIndex = dbSize * dbSize - 1;

    for (int k = lastIndex; k > insertIndex; k--) {
        int srcRow = (k - 1) / dbSize;
        int srcCol = (k - 1) % dbSize;

        int dstRow = k / dbSize;
        int dstCol = k % dbSize;

        database[dstRow][dstCol] = database[srcRow][srcCol];
        database[srcRow][srcCol] = NULL;
    }
    database[insertRow][insertCol] = newShow;
}
void addSeason(){
    printf("Enter the name of the show:\n");
    char *showName = getString();

    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = getString();

    if (findSeason(show, seasonName) != NULL) {
        printf("Season already exists.\n");
        free(showName);
        free(seasonName);
        return;
    }

    printf("Enter the position:\n");
    int position = getInt();

    Season *newSeason = (Season *)malloc(sizeof(Season));
    if (newSeason == NULL) exit(1);
    newSeason->name = seasonName;
    newSeason->episodes = NULL;
    newSeason->next = NULL;

    free(showName);

    if (show->seasons == NULL || position == 0) {
        newSeason->next = show->seasons;
        show->seasons = newSeason;
        return;
    }

    Season *current = show->seasons;
    int currentPosition = 0;
    while (current->next != NULL && currentPosition < position - 1) {
        current = current->next;
        currentPosition++;
    }

    newSeason->next = current->next;
    current->next = newSeason;
}
void addEpisode(){
    printf("Enter the name of the show:\n");
    char *showName = getString();

    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = getString();

    Season *season = findSeason(show, seasonName);
    if (season == NULL) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }

    printf("Enter the name of the episode:\n");
    char *episodeName = getString();

    if (findEpisode(season, episodeName) != NULL) {
        printf("Episode already exists.\n");
        free(showName);
        free(seasonName);
        free(episodeName);
        return;
    }

    printf("Enter the length (xx:xx:xx):\n");
    char *episodeLength = getString();
    while (!validLength(episodeLength)) {
        printf("Invalid length, enter again:\n");
        free(episodeLength);
        episodeLength = getString();
    }

    printf("Enter the position:\n");
    int position = getInt();

    Episode *newEpisode = (Episode *)malloc(sizeof(Episode));
    if (newEpisode == NULL) exit(1);
    newEpisode->name = episodeName;
    newEpisode->length = episodeLength;
    newEpisode->next = NULL;

    free(showName);
    free(seasonName);

    if (season->episodes == NULL || position == 0) {
        newEpisode->next = season->episodes;
        season->episodes = newEpisode;
        return;
    }

    Episode *current = season->episodes;
    int currentPosition = 0;
    while (current->next != NULL && currentPosition < position - 1) {
        current = current->next;
        currentPosition++;
    }

    newEpisode->next = current->next;
    current->next = newEpisode;
}

void deleteEpisode(){
    printf("Enter the name of the show:\n");
    char *showName = getString();

    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = getString();

    Season *season = findSeason(show, seasonName);
    if (season == NULL) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }

    printf("Enter the name of the episode:\n");
    char *episodeName = getString();

    Episode *episodeToDelete = findEpisode(season, episodeName);
    if (episodeToDelete == NULL) {
        printf("Episode not found.\n");
        free(showName);
        free(seasonName);
        free(episodeName);
        return;
    }

    if (season->episodes == episodeToDelete) {
        season->episodes = episodeToDelete->next;
    } else {
        Episode *previous = season->episodes;
        while (previous != NULL && previous->next != episodeToDelete) {
            previous = previous->next;
        }
        if (previous != NULL) previous->next = episodeToDelete->next;
    }

    freeEpisode(episodeToDelete);
    free(showName);
    free(seasonName);
    free(episodeName);
}
void deleteSeason(){
    printf("Enter the name of the show:\n");
    char *showName = getString();

    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = getString();

    Season *seasonToDelete = findSeason(show, seasonName);
    if (seasonToDelete == NULL) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }

    if (show->seasons == seasonToDelete) {
        show->seasons = seasonToDelete->next;
    } else {
        Season *previous = show->seasons;
        while (previous != NULL && previous->next != seasonToDelete) {
            previous = previous->next;
        }
        if (previous != NULL) previous->next = seasonToDelete->next;
    }

    freeSeason(seasonToDelete);
    free(showName);
    free(seasonName);
}
void deleteShow(){
    printf("Enter the name of the show:\n");
    char *showName = getString();
    // Find show
    TVShow *showToDelete = findShow(showName);
    if (showToDelete == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    // Find show position in array
    int deleteRow = -1, deleteCol = -1;
    for (int row = 0; row < dbSize; row++) {
        for (int col = 0; col < dbSize; col++) {
            if (database[row][col] == showToDelete) {
                deleteRow = row;
                deleteCol = col;
                row = dbSize;
                break;
            }
        }
    }
    // Free show (all seasons and episodes)
    freeShow(showToDelete);
    database[deleteRow][deleteCol] = NULL;
    // Shift all shows left (safe flat shift, closes the gap cleanly)
    int deleteIndex = deleteRow * dbSize + deleteCol;
    int lastIndex = dbSize * dbSize - 1;

    for (int k = deleteIndex; k < lastIndex; k++) {
        int srcRow = (k + 1) / dbSize;
        int srcCol = (k + 1) % dbSize;

        int dstRow = k / dbSize;
        int dstCol = k % dbSize;

        database[dstRow][dstCol] = database[srcRow][srcCol];
    }

    // Clear the last cell after shifting
    database[lastIndex / dbSize][lastIndex % dbSize] = NULL;
    // Check if array can be shrunk
    int remainingShows = countShows();
    int smallerSize = dbSize - 1;

    // If all remaining shows fit in smaller array
    if (smallerSize > 0 && remainingShows <= smallerSize * smallerSize) {
        shrinkDB();
    }
    free(showName);
}

void printShow(){
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Name: %s\n", show->name);
    printf("Seasons:\n");
    Season *currentSeason = show->seasons;
    int seasonIndex = 0;
    // Print ALL seasons with their episodes
    while (currentSeason != NULL) {
        printf("\tSeason %d: %s\n", seasonIndex, currentSeason->name);
        // Print episodes of THIS season
        Episode *currentEpisode = currentSeason->episodes;
        int episodeIndex = 0;
        while (currentEpisode != NULL) {
            printf("\t\tEpisode %d: %s (%s)\n",
                   episodeIndex,
                   currentEpisode->name,
                   currentEpisode->length);
            currentEpisode = currentEpisode->next;
            episodeIndex++;
        }
        currentSeason = currentSeason->next;
        seasonIndex++;
    }
    free(showName);
}
void printEpisode(){
    printf("Enter the name of the show:\n");
    char *showName = getString();

    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = getString();

    Season *season = findSeason(show, seasonName);
    if (season == NULL) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }

    printf("Enter the name of the episode:\n");
    char *episodeName = getString();

    Episode *episode = findEpisode(season, episodeName);
    if (episode == NULL) {
        printf("Episode not found.\n");
        free(showName);
        free(seasonName);
        free(episodeName);
        return;
    }

    printf("Name: %s\n", episode->name);
    printf("Length: %s\n", episode->length);

    free(showName);
    free(seasonName);
    free(episodeName);
}
void printArray(){
    // If array is completely empty - print nothing
    if (database == NULL || dbSize == 0) {
        return;
    }

    // Iterate through array
    for (int row = 0; row < dbSize; row++) {
        for (int col = 0; col < dbSize; col++) {
            if (database[row][col] == NULL) {
                printf("[NULL]");
            } else {
                printf("[%s]", database[row][col]->name);
            }

            // Space between cells, but not after last cell in row
            if (col < dbSize - 1) {
                printf(" ");
            }
        }
        // Newline after each row
        printf("\n");
    }
}