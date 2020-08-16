/**
* IZP Proj1
* Autor: Iveta Strnadova, xstrna14
* Datum 4.11.2018
* Projekt obsahuje pøíkazy navíc a detekci zacyklení
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define TEXT_LENGHT 1001
#define COMMAND_LENGHT 201
#define MAX_COMMANDS 100
#define MAX_NONSAFE_CYCLES 100

/**
* Function load commands from file into two dimensional array
* @param fileName[] - name of file from which the commands should be extracted
* @param command[][COMMAND_LENGHT] - two dimensional array into which commands are loaded
* @return -1 if there is error with loading given file
* @return -2 if any of loading commands exceeds COMMAND_LENGHT
*/
int loadCommands(char fileName[], char command[][COMMAND_LENGHT])
{
    FILE* f;
    int i;
    if((f=fopen(fileName, "r"))==NULL) return -1;

    for(i=0; (fgets(command[i],COMMAND_LENGHT,f))!=NULL; i++)
        if(command[i][strlen(command[i])-1]!='\n') return -2;
    fclose(f);
    return i;
}

/**
* Function removes char '\n' at the end of text line, supposing there is any
* @param text[] - text line that is currently being edited
*/
void removeEOL(char text[])
{
    if(text[strlen(text)-1]=='\n') text[strlen(text)-1]='\0';
}

/**
* Function loads new line of text to edit, overwriting the current one
* @param text[] - text line that is currently being edited
* @return false if the line was succesfuly loaded
* @return true if reached end of text to edit and nothing was loaded
*/
bool getNewLine(char text[])
{
    if(fgets(text, TEXT_LENGHT, stdin)!=NULL) return false;
    else return true;
}

/**
* Function extracts number from given string
* @param command[] - current command from which the number needs to be extracted
* @return resultNumber - returns the desired number if the conversion was succesful
* @return -1 if there was a problem while extracting number
*/
int getNumberInCommand(char command[])
{
    char *pointerOnExpectedEnd;
    int resultNumber;
    resultNumber=strtol(command, &pointerOnExpectedEnd, 10);
    if((*pointerOnExpectedEnd)=='\n') return resultNumber;
    else return -1;
}

/**
* Function finds, whether string hay contains substring needle
* @param hay[] - larger string supposedely containting string needle
* @param needle[] - smaller string
* @return pointerOnFoundString-hay - returns position of needle in hay if needle was found
* @return -1 if needle wasn't found
*/
int findString(char hay[], char needle[])
{
    char *pointerOnFoundString;
    removeEOL(needle);
    pointerOnFoundString=strstr(hay, needle);
    if(pointerOnFoundString!=NULL) return (pointerOnFoundString-hay);
    else return -1;
}

/**
* Function extracts the number from command, extracts the condition and checks, if current text line contains that condition string
* @param text[] - text line that is currently being edited
* @param command[] - current command from which the number needs to be extracted
* @return resultNumber - returns number to which commandRow will jump if the text contains condition string
* @return 0 if there isn't condition string in text
* @return -1 if there was a problem in sytax
*/
int conditionedGoto(char text[], char command[])
{
    char *pointerOnSpace;
    int resultNumber, conditionLenght, foundString;
    resultNumber=strtol(command, &pointerOnSpace, 10);
    ///If there isn't a space after number, the syntax of this command is wrong and it is not executed
    if((*pointerOnSpace)==' ')
    {
        ///Saves lenght of the condition part of command (including \n char), creates an string of that lenght, copies condition part to this string, removes EOL, then passes condition with text to function findString
        conditionLenght=strlen(&pointerOnSpace[1]);
        char condition[conditionLenght];
        strcpy(condition, &pointerOnSpace[1]);
        removeEOL(condition);
        foundString=findString(text, condition);
        if(foundString>=0) return resultNumber;
        else return 0;
    }
    else return -1;
}

/**
* Function inserts content before text line
* @param text[] - text line that is currently being edited
* @param beforeContent[] - part of command containing content to be added (with '\n' char, which is removed in removeEOL)
* @return -1 if adding content would result in text overflow
* @return 0 if adding was succesful
*/
int insertBefore(char text[], char beforeContent[])
{
    removeEOL(beforeContent);
    if(strlen(text)+strlen(beforeContent)>(TEXT_LENGHT-1)) return -1;
    char help[TEXT_LENGHT];
    strcpy(help, beforeContent);
    strcpy(&help[strlen(beforeContent)], text);
    strcpy(text, help);
    return 0;
}

/**
* Function inserts content after text line
* @param text[] - text line that is currently being edited
* @param beforeContent[] - part of command containing content to be added
* @return -1 if adding content would result in text overflow
* @return 0 if adding was succesful
*/
int insertAfter(char text[], char afterContent[])
{
    removeEOL(text);
    if(strlen(text)+strlen(afterContent)>(TEXT_LENGHT-1)) return -1;
    char help[TEXT_LENGHT];
    strcpy(help, text);
    strcpy(&help[strlen(text)], afterContent);
    strcpy(text, help);
    return 0;
}

/**
* Function inserts EOL at the end of current text line
* @param text[] - text line that is currently being edited
* @return -1 if adding char '\n' would result in text overflow
* @return 0 if adding was succesful
*/
int insertEOL(char text[])
{
    if(strlen(text)==(TEXT_LENGHT-1)) return -1;
    text[strlen(text)+1]='\0';
    text[strlen(text)]='\n';
    return 0;
}

/**
* Function finds first pattern occurence in text and replaces it with replacement
* @param text[] - text line that is currently being edited
* @param pattern[] - string that would be, if found, replaced
* @param replacement[] - string to replace pattern
* @return -1 if the text would overflow after replacing
* @return 0 if no string pattern was found
* @return (positionOfPattern+strlen(replacement)) - if replacement was succesful, returns position of char right after last char of replaced string
*/
int replaceOnce(char text[], char pattern[], char replacement[])
{
    if(strlen(text)+strlen(replacement)-strlen(pattern)>(TEXT_LENGHT-1)) return -1;
    int positionOfPattern;
    char help[TEXT_LENGHT];

    ///Finds index of first char of pattern in text
    positionOfPattern=findString(text, pattern);
    if(positionOfPattern==-1) return 0;

    strcpy(help, text); ///Copies text to help
    strcpy(&help[positionOfPattern], replacement); ///copies string replacement on position where pattern starts
    strcpy(&help[positionOfPattern+strlen(replacement)], &text[positionOfPattern+strlen(pattern)]); ///copies rest of the text after pattern occurence to help after inserted replacement
    strcpy(text, help);
    return (positionOfPattern+strlen(replacement));
}

/**
* Function extracts pattern and replacement from command and executes replaceOnce once or multiple times based on whether it was s or S command
* @param text[] - text line that is currently being edited
* @param command[] - current command from which the number needs to be extracted
* @param repeatedly - whether the function was called for command s or S, determines if the replacement will be executed only once or for each pattern in text
* @return check - variable carrying return value of function replaceOnce (-1 if failed due to risk of overflow, 0 or bigger number if function was executed without error)
* @return -2 if there is number of dividers different than 2, thus wrong syntax
*/
int replacement(char text[], char command[], bool repeatedly)
{
    char divide = command[0];
    int patternLenght=0, check=1, move=0, replacementLenght, divideNumber=0;

    ///Checking, if the syntax is right
    for(int i=0;command[i]!='\0';i++) if(command[i]==divide) divideNumber++;
    if(divideNumber!=2) return -2;

    char help[strlen(command)];
    strcpy(help, &command[1]);
    removeEOL(help);

    ///Counts lenght of pattern between dividers
    for(int i=0;help[i]!=divide;i++)patternLenght++;
    ///Counts lenght of replacemen; +1 for '\0' -1 because of one divide char it will skip
    replacementLenght=strlen(help)-patternLenght;
    char pattern[patternLenght+1], replacement[replacementLenght];

    ///Coopies end of help after divider to replacement
    strcpy(replacement, &help[1+patternLenght]);
    ///Cuts help right after last char of pattern, then copies this string to pattern
    help[patternLenght]='\0';
    strcpy(pattern, help);

    if(repeatedly==false)
        check=replaceOnce(text, pattern, replacement);
    else
        while(check>0)
        {
            ///If replaceOnce found pattern and replaced it, check is index of char after the replacement (to avoid infinite cycle in case replacement contains pattern), which is added to move
            check=replaceOnce(&text[move], pattern, replacement);
            move=move+check;
        }
    return check;
}

/**
* Function examines commands between goto and line it jumps to. To avoid infinite loop, there should be command loading next line (n, d or f). However, with use of conditioned goto, even loops without n, d or f can be finite, but they are difficult to predict
* @param command[][201] - array of commands used to edit text
* @param lineWithGoto - index of command line from which goto (or conditioned goto) is called
* @param lineToJumpTo - index of command to which the program through goto jumps
* @return 1 if cycle is potentialy infinite, to be counted and terminated if needed
* @return 0 the cycle is alright
* @return -1 if the cycle is infinite
*/
int cycleCheck(char command[][COMMAND_LENGHT], int lineWithGoto, int lineToJumpTo)
{
    ///With possible use of conditioned goto, cycle can be finite even without use of n, d or f
    int countC=0, countG=0, countNDF=0;
    for(int i=lineToJumpTo; i<lineWithGoto; i++)
    {
        ///If there is g or c before n/d/f command, there is chance they are skipped, so they can't be used anymore as a safe way to recognise finite loop
        if((command[i][0]=='n'||command[i][0]=='d'||command[i][0]=='f'))
        {
            if((countC==0&&countG==0)) return 0;
            else countNDF++;
        }
        if(command[i][0]=='c') countC++;
        if(command[i][0]=='g') countG++;
    }
    if((countC!=0||countNDF!=0)) return 1;
    return -1;
}

/**
* Function prints current text line (if bool isN==true) and loads new one (for both n and d). If command n/d is followed by number, it is exucuted multiple times
* @param text[] - text line that is currently being edited
* @param command[] - current command from which the number needs to be extracted
* @return endOfText - returns bool endOfText, which in main checks, if there are any more lines of input to edit
*/
bool executeND(bool isN, char command[], char text[])
{
    int n;
    bool endOfText=false;
    if(command[1]=='\n')
        {
            if(isN==true) fprintf(stdout, "%s", text);
            endOfText=getNewLine(text);
        }
    else
        {
            n=getNumberInCommand(&command[1]);
            for(int i=0;(i<n)&&(endOfText==false);i++)
            {
                if(isN==true)fprintf(stdout, "%s", text);
                endOfText=getNewLine(text);
            }
            if(n==-1) fprintf(stderr, "Failed to extract number from \"%s\".\n", command);
        }
    return endOfText;
}

int main(int argc, char* argv[])
{
    char text[TEXT_LENGHT];
    char command[MAX_COMMANDS][COMMAND_LENGHT];
    int totalCommands, commandRow, n, errorCheck, foundString;
    bool endOfText;

    ///Checking for required number of arguments
    if(argc!=2)
    {
        fprintf(stdout, "Unvalid number of argument. Exiting program...\n");
        return 1;
    }

    totalCommands=loadCommands(argv[1], command);
    if(totalCommands<=0)
    {
    if(totalCommands==0) fprintf(stderr, "No commands to load. Exiting program...\n");
    if(totalCommands==-1) fprintf(stderr, "Error while loading file %s. Exiting program...\n", argv[1]);
    if(totalCommands==-2) fprintf(stderr, "Error while loading commands. Maximal lenght exceeded. Exiting program...");
    return 1;
    }

    ///Finite cycle check counting is troubling, because conditioned goto is harder to predict. There will be series of commands, where there won't be indicators whether it is finite cycle or not, and only way will be to count iterations of goto. As there can be more cycles like this, each of them needs to have their own cycle check, hence array of infiniteCycleCheck with size of number of commands
    int finiteCycleCheck[totalCommands];
    for (int j=0;j<totalCommands;j++) finiteCycleCheck[j]=1;

    ///Loading first line from text to edit. In the rest of program, next line will be loaded only after commands n and d or at the end
    endOfText=getNewLine(text);
    if(endOfText)
    {
        fprintf(stderr, "File with text for editing is empty.\n");
        return 1;
    }

    ///Main cycle executing command after command, while checking whether there are any commands left to execute and whether there is still text to edit
    for(commandRow=0;(commandRow<totalCommands)&&(endOfText==false);commandRow++)
    {
        ///Switch based on first char of each command which indicates type of operation to do
        switch(command[commandRow][0])
        {
        case 'i':
            ///All commands passed from main to any function are given without the first letter indicating the type of command
            fprintf(stdout, "%s", &command[commandRow][1]);
            break;
        case 'b':
            errorCheck=insertBefore(text, &command[commandRow][1]);
            if(errorCheck==-1) fprintf(stderr, "Command \"%s\" was not executed due to overflow risk.\n", command[commandRow]);
            break;
        case 'a':
            errorCheck=insertAfter(text, &command[commandRow][1]);
            if(errorCheck==-1) fprintf(stderr, "Command \"%s\" was not executed due to overflow risk.\n", command[commandRow]);
            break;
        case 'd':
            endOfText=executeND(false, command[commandRow], text);
            break;
        case 'r':
            removeEOL(text);
            break;
        case 's':
            errorCheck=replacement(text, &command[commandRow][1], false);
            if(errorCheck==-2) fprintf(stderr, "Failed to execute command \"%s\" due to wrong syntax - invalid number of dividers.\n", command[commandRow]);
            if(errorCheck==-1) fprintf(stderr, "Command \"%s\" was not executed due to overflow risk.\n", command[commandRow]);
            break;
        case 'S':
            errorCheck=replacement(text, &command[commandRow][1], true);
            if(errorCheck==-2) fprintf(stderr, "Failed to execute command \"%s\" due to wrong syntax - invalid number of dividers.\n", command[commandRow]);
            if(errorCheck==-1) fprintf(stderr, "Command \"%s\" was not executed due to overflow risk.\n", command[commandRow]);
            break;
        case 'n':
            endOfText=executeND(true, command[commandRow], text);
            break;
        case 'q':
            ///If the command is q, endOfText is set true, the next interation isn't executed and no left lines are printed
            endOfText=true;
            break;
        case 'g':
            ///If it succeeds to extract number after g, commandRow is set on this number (n-2, counting in the increment of next iteration and difference between actual index and number of row)
            n=getNumberInCommand(&command[commandRow][1]);
            if(n==(commandRow+1))
            {
                fprintf(stderr, "Goto command calling its line leading to infinite cycle detected. Exiting program...");
                return 1;
            }
            if(n>0)
            {
                    if(finiteCycleCheck[commandRow]==1) finiteCycleCheck[commandRow]=cycleCheck(command, commandRow, n-1);
                    if(finiteCycleCheck[commandRow]>=1) finiteCycleCheck[commandRow]++;
                    if(finiteCycleCheck[commandRow]==-1||finiteCycleCheck[commandRow]>MAX_NONSAFE_CYCLES)
                    {
                        fprintf(stderr, "Infinite loop detected between lines %d and %d. Exiting program...", n, commandRow+1);
                        return 1;
                    }
                commandRow=n-2;
            }
            if(n==-1) fprintf(stderr, "Failed to extract number from \"%s\".\n", command[commandRow]);
            break;
        case 'f':
            ///Loads next lines until it finds one with desired pattern. If it doesn't find such, endOfText is set true and program ends
            foundString=-1;
            while((foundString==-1)&&(endOfText==false))
            {
                fprintf(stdout, "%s", text);
                endOfText=getNewLine(text);
                foundString=findString(text, &command[commandRow][1]);
            }
            break;
        case 'c':
            n=conditionedGoto(text, &command[commandRow][1]);
            if(n==(commandRow+1))
            {
                fprintf(stderr, "Goto command calling its line leading to infinite cycle detected. Exiting program...");
                return 1;
            }
            if(n>0)
            {
                if(finiteCycleCheck[commandRow]==1&&n<=commandRow) finiteCycleCheck[commandRow]=cycleCheck(command, commandRow, n-1);
                if(finiteCycleCheck[commandRow]>=1) finiteCycleCheck[commandRow]++;
                if(finiteCycleCheck[commandRow]==-1||finiteCycleCheck[commandRow]>MAX_NONSAFE_CYCLES)
                {
                    fprintf(stderr, "Infinite loop detected between lines %d and %d. Exiting program...", n, commandRow+1);
                    return 1;
                }
                else commandRow=n-2;
            }
            if(n==-1)fprintf(stderr, "Failed to extract number from \"%s\".\n", command[commandRow]);
            break;
        case 'e':
            errorCheck=insertEOL(text);
            if(errorCheck==-1) fprintf(stderr, "Failed to append EOL due to maximum string lenght.\n");
            break;
        default:
            fprintf(stderr, "Failed to recognise command \"%s\" due to wrong syntax.\n", command[commandRow]);
            break;
        }
    }
    ///In case the command cycle ended due to the lack of commands with some text not printed, this part will print the rest of text
    while(endOfText==false)
        {
            fprintf(stdout, "%s", text);
            endOfText=getNewLine(text);
        }
    return 0;
}
