#include <stdio.h>
#include <string.h>

/*
 * isBigEndian function - determine endianness of the given file. If the second byte has the value -1 (FF) it means
 * that the file boom is big endian. Either it is little endian.
 */
int isFileBigEndian(const char* ch)
{
    if (ch[1] == -1) return 1; // Big endian
    return 0; // Little endian
}

/*
 * swapBytes function - simple swap between two bytes.
 */
void swapBytes(char* byte1, char* byte2)
{
    char temp = *byte1;
    *byte1 = *byte2;
    *byte2 = temp;
}

/*
 * noFlags function - the function role is to create a file that is identical in its content - mean that the OS source
 * and destination OS is equal. The function will receive a flag, that will sign if we need to swap each two bytes
 */
void noFlags(char* srcFileName, char* destFileName, int swap)
{
    // open the source file in "rb" mode - read and prevent the standard library from translating a few characters
    FILE* srcP = fopen(srcFileName, "rb");
    if (!srcP) return; // the file is invalid
    if (srcP)
    {
        char buffer[2]; // read 2 bytes (UTF-16)
        size_t counts = 1;
        // open the source file in "wb" mode - write and prevent the standard library from translating a few characters
        FILE* destP = fopen(destFileName, "wb");
        while ((counts = fread(buffer, sizeof(buffer), counts, srcP)) > 0)
        {
            if (swap) swapBytes(&buffer[0], &buffer[1]);
            fwrite(buffer, sizeof(buffer), counts, destP);
        }
        fclose(destP); 
    }
    fclose(srcP);
}


/*
 * newLine function - return the newLine char (refer to OS).
 */
char newLine(char* os)
{
    if (strcmp(os, "-mac") == 0) return '\r';
    else if (strcmp(os, "-unix") == 0) return '\n';
    return 0;
}


/*
 * windowsDest function - function role is to take care when the destination OS is windows, and the source is mac or
 * unix. It will check the given OS file (which is windows), and will create new file of the destination OS.
 * Moreover, it will take care of the case when we need to swap each two bytes (URF-16) - by the flag swap.
 */
void windowsDest(char* srcFileName, char* destFileName, char* srcFlag, char* destFlag, int swap)
{
    FILE* srcP = fopen(srcFileName, "rb");
    if(!srcP) return; 
    FILE* destP = fopen(destFileName, "wb");
    char temp[2]; // temporary buffer
    char buffer[2]; // read 2 bytes (UTF-16)
    size_t counts = 1;

    char srcOS = newLine(srcFlag); 

    fread(buffer, sizeof(buffer), counts, srcP); // read the first 2 bytes to determined big or little endian
    int bigEndian = isFileBigEndian(buffer);
    if (swap) swapBytes(&buffer[0], &buffer[1]); 
    fwrite(buffer, sizeof(buffer), counts, destP); 

    while ((counts = fread(buffer, sizeof(buffer), counts, srcP)) > 0)
    {
        if (bigEndian) // big endian file
        {
            if ((buffer[0] == 0x00) && (buffer[1] == srcOS)) // we got a match
            {
                buffer[0] = 0x00;
                buffer[1] = 0x0d;
                if (swap) swapBytes(&buffer[0], &buffer[1]);
                fwrite(buffer, sizeof(buffer), counts, destP); 
                temp[0] = 0x00;
                temp[1] = 0x0a;
                if (swap) swapBytes(&temp[0], &temp[1]);
                fwrite(temp, sizeof(temp), counts, destP); 
            }
            else
            {
                if (swap) swapBytes(&buffer[0], &buffer[1]);
                fwrite(buffer, sizeof(buffer), counts, destP);
            }
        }
        else // little endian file
        {
            if ((buffer[1] == 0x00) && (buffer[0] == srcOS)) // we got a match
            {
                buffer[1] = 0x00;
                buffer[0] = 0x0d;
                if (swap) swapBytes(&buffer[0], &buffer[1]);
                fwrite(buffer, sizeof(buffer), counts, destP); 
                temp[1] = 0x00;
                temp[0] = 0x0a;
                if (swap) swapBytes(&temp[0], &temp[1]);
                fwrite(temp, sizeof(temp), counts, destP); 
            }
            else
            {
                if (swap) swapBytes(&buffer[0], &buffer[1]);
                fwrite(buffer, sizeof(buffer), counts, destP); 
            }
        }
    }
    fclose(srcP);
    fclose(destP);
}

/*
 * windowsSrc function - function role is to take care when the source OS is windows, and the destination OS is mac or
 * unix. It will check the given OS (which is windows), and will create new file of the destination OS.
 * Moreover, it will take care of the case when we need to swap each two bytes (URF-16) - by the flag swap.
 */
void windowsSrc(char* srcFileName, char* destFileName, char* srcFlag, char* destFlag, int swap)
{
    FILE* srcP = fopen(srcFileName, "rb");
    if(!srcP) return;
    FILE* destP = fopen(destFileName, "wb");
    char temp[2]; // temporary buffer
    char buffer[2]; // read 2 bytes (UTF-16)
    size_t counts = 1;

    char destOS = newLine(destFlag); 

    fread(buffer, sizeof(buffer), counts, srcP); // read the first 2 bytes to determined big or little endian
    int bigEndian = isFileBigEndian(buffer);
    if (swap) swapBytes(&buffer[0], &buffer[1]); 
    fwrite(buffer, sizeof(buffer), counts, destP);

    while ((counts = fread(buffer, sizeof(buffer), counts, srcP)) > 0)
    {
        if (bigEndian) // big endian file
        {
            if ((buffer[0] == 0x00) && (buffer[1] == '\r')) // we got '\r'
            {
                strcpy(temp, buffer); // save the buffer with '\r'
                fread(buffer, sizeof(buffer), counts, srcP);
                if ((buffer[0] == 0x00) && (buffer[1] == '\n')) // we got '\n'
                {
                    buffer[1] = destOS; // change the new line char
                    if (swap) swapBytes(&buffer[0], &buffer[1]);
                    fwrite(buffer, sizeof(buffer), counts, destP); 
                }
                else // we dont found '\n'
                {
                    if (swap)
                    {
                        swapBytes(&temp[0], &temp[1]);
                        swapBytes(&buffer[0], &buffer[1]);
                    }
                    fwrite(temp, sizeof(buffer), counts, destP); 
                    fwrite(buffer, sizeof(buffer), counts, destP); 
                }
            }
            else // we dont found '\r'
            {
                if (swap) swapBytes(&buffer[0], &buffer[1]);
                fwrite(buffer, sizeof(buffer), counts, destP); 
            }
        }
        else
        {
            if ((buffer[1] == 0x00) && (buffer[0] == '\r')) // we got '\r'
            {
                strcpy(temp, buffer); // save the buffer with '\r'
                fread(buffer, sizeof(buffer), counts, srcP);
                if ((buffer[1] == 0x00) && (buffer[0] == '\n')) // we got '\n'
                {
                    buffer[0] = destOS; // change the new line char
                    if (swap) swapBytes(&buffer[0], &buffer[1]);
                    fwrite(buffer, sizeof(buffer), counts, destP); 
                }
                else // we dont found '\n'
                {
                    if (swap)
                    {
                        swapBytes(&temp[0], &temp[1]);
                        swapBytes(&buffer[0], &buffer[1]);
                    }
                    fwrite(temp, sizeof(buffer), counts, destP); 
                    fwrite(buffer, sizeof(buffer), counts, destP); 
                }
            }
            else // we dont found '\r'
            {
                if (swap) swapBytes(&buffer[0], &buffer[1]);
                fwrite(buffer, sizeof(buffer), counts, destP);
            }
        }
    }
    fclose(srcP);
    fclose(destP);
}

/*
 * noWindows function - function role is to take care when teh source and the destination OS are not windows.
 * It will check the given OS, and will modify the file to the destination OS. Moreover, it will take care of the
 * case when we need to swap each two bytes (URF-16) - by the flag swap.
 */
void noWindows(char* srcFileName, char* destFileName, char* srcFlag, char* destFlag, int swap)
{
    FILE* srcP = fopen(srcFileName, "rb");
    if(!srcP) return;
    FILE* destP = fopen(destFileName, "wb");
    char buffer[2]; // read 2 bytes (UTF-16)
    size_t counts = 1;

    char srcOS = newLine(srcFlag); // create the new line char for the source
    char destOS = newLine(destFlag); // create the new line char for the destination

    fread(buffer, sizeof(buffer), counts, srcP); // read the first 2 bytes to determined big or little endian
    int bigEndian = isFileBigEndian(buffer);

    if (swap) swapBytes(&buffer[0], &buffer[1]); // swap bytes if we need to
    fwrite(buffer, sizeof(buffer), counts, destP); // write the buffer to the file

    while ((counts = fread(buffer, sizeof(buffer), counts, srcP)) > 0)
    {
        if (bigEndian) // big endian file
        {
            if ((buffer[0] == 0x00) && (buffer[1] == srcOS)) buffer[1] = destOS; // change the new line char
            if (swap) swapBytes(&buffer[0], &buffer[1]);
            fwrite(buffer, sizeof(buffer), counts, destP);
        }
        else // little endian file
        {
            if ((buffer[1] == 0x00) && (buffer[0] == srcOS)) buffer[0] = destOS; // change the new line char
            if (swap) swapBytes(&buffer[0], &buffer[1]);
            fwrite(buffer, sizeof(buffer), counts, destP);
        }
    }
    fclose(srcP);
    fclose(destP);
}


int main(int argc, char* argv[])
{
    int doSwap = 0;
    if ((argc != 3) && (argc != 5) && (argc!= 6)) return 0; // invalid num of arguments
    if (strcmp(argv[1], argv[2]) == 0) return 0; // the source and he destination file have the same name
	switch (argc)
	{
	case 3: // src.txt dst.txt
		noFlags(argv[1], argv[2], doSwap);
		break;
    case 5: // src.txt dst.txt flag1 flag2
        if (strcmp(argv[3], argv[4]) == 0) noFlags(argv[1], argv[2], doSwap); // base case
        else if (strcmp(argv[3], "-win") == 0) windowsSrc(argv[1], argv[2], argv[3], argv[4], doSwap); // windows source case
        else if (strcmp(argv[4], "-win") == 0) windowsDest(argv[1], argv[2], argv[3], argv[4], doSwap); // windows destination case
        else noWindows(argv[1], argv[2], argv[3], argv[4], doSwap); // without windows case
            break;
    case 6: // src.txt dst.txt flag1 flag2 flag3
        if (strcmp(argv[5], "-swap") == 0) doSwap = 1; // update swap flag if required
        if (strcmp(argv[3], argv[4]) == 0) noFlags(argv[1], argv[2], doSwap); // base case
        else if (strcmp(argv[3], "-win") == 0) windowsSrc(argv[1], argv[2], argv[3], argv[4], doSwap); // windows source case
        else if (strcmp(argv[4], "-win") == 0) windowsDest(argv[1], argv[2], argv[3], argv[4], doSwap); // windows destination case
        else noWindows(argv[1], argv[2], argv[3], argv[4], doSwap); // without windows case
            break;
	default:
		break;
	}
	return 0;
}
