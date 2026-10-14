#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int showEnds = 0;
    int numberLines = 0;
    int squeeze = 0;

    int lineNumber = 1;
    int atLineStart = 1;
    int lineHasContent = 0;
    int fileCount = 0;

    char ch;

    if (argc < 2) {
        printf("Usage: mycat [-ens] file1 [file2 ...]\n");
        return 1;
    }

    // Process command-line arguments
    for (int i = 1; i < argc; i++) {

        if (argv[i][0] == '-') {

            for (int j = 1; argv[i][j] != '\0'; j++) {

                if (argv[i][j] == 'e')
                    showEnds = 1;

                else if (argv[i][j] == 'n')
                    numberLines = 1;

                else if (argv[i][j] == 's')
                    squeeze = 1;

                else {
                    printf("Unknown option: %c\n", argv[i][j]);
                    return 1;
                }
            }
        }
    }

    // Process each file
    for (int i = 1; i < argc; i++) {

        if (argv[i][0] == '-')
            continue;

        fileCount++;

        int fd = open(argv[i], O_RDONLY);

        if (fd == -1) {
            perror(argv[i]);
            continue;
        }

        while (read(fd, &ch, 1) > 0) {

            // Skip blank lines when -s is enabled
            if (squeeze && atLineStart && ch == '\n')
                continue;

            // Number the beginning of each line
            if (atLineStart && numberLines) {
                printf("%6d  ", lineNumber++);
                fflush(stdout);
            }

            if (ch == '\n') {

                // Add $ at the end of each line
                if (showEnds)
                    write(STDOUT_FILENO, "$", 1);

                write(STDOUT_FILENO, "\n", 1);

                atLineStart = 1;
                lineHasContent = 0;
            }
            else {
                write(STDOUT_FILENO, &ch, 1);

                atLineStart = 0;
                lineHasContent = 1;
            }
        }

        close(fd);
    }

    if (fileCount == 0) {
        printf("Error: No input files provided.\n");
        return 1;
    }

    return 0;
}
