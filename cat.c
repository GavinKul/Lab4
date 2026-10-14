#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int e = 0, n = 0, s = 0;
    int lineNum = 1;
    int files = 0;
    int error = 0;

    char line[4096];
    int len = 0;

    // Check for arguments
    if (argc < 2) {
        printf("Usage: mycat [-ens] file1 [file2 ...]\n");
        return 1;
    }

    // Check for switches
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                if (argv[i][j] == 'e')
                    e = 1;
                else if (argv[i][j] == 'n')
                    n = 1;
                else if (argv[i][j] == 's')
                    s = 1;
                else {
                    printf("Invalid option: %c\n", argv[i][j]);
                    return 1;
                }
            }
        }
    }

    // Read each file
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-')
            continue;

        files++;
        int fd = open(argv[i], O_RDONLY);

        if (fd == -1) {
            perror(argv[i]);
            error = 1;
            continue;
        }

        char ch;
        int count;

        while ((count = read(fd, &ch, 1)) > 0) {

            if (ch != '\n') {
                if (len >= sizeof(line)) {
                    printf("Error: Line too long\n");
                    close(fd);
                    return 1;
                }
                line[len++] = ch;
                continue;
            }

            // Check if line contains only whitespace
            int blank = 1;
            for (int j = 0; j < len; j++) {
                if (line[j] != ' ' && line[j] != '\t' &&
                    line[j] != '\r' && line[j] != '\v' &&
                    line[j] != '\f') {
                    blank = 0;
                    break;
                }
            }

            if (!s || !blank) {
                if (n)
                    printf("%6d  ", lineNum++);

                fflush(stdout);
                write(1, line, len);

                if (e)
                    write(1, "$", 1);

                write(1, "\n", 1);
            }

            len = 0;
        }

        // Print final line without newline
        if (len > 0) {
            int blank = 1;

            for (int j = 0; j < len; j++) {
                if (line[j] != ' ' && line[j] != '\t' &&
                    line[j] != '\r' && line[j] != '\v' &&
                    line[j] != '\f') {
                    blank = 0;
                    break;
                }
            }

            if (!s || !blank) {
                if (n)
                    printf("%6d  ", lineNum++);

                fflush(stdout);
                write(1, line, len);

                if (e)
                    write(1, "$", 1);
            }
        }

        len = 0;

        if (count < 0) {
            perror(argv[i]);
            error = 1;
        }

        close(fd);
    }

    if (files == 0) {
        printf("Error: No input files\n");
        return 1;
    }

    return error;
}

