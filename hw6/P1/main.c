#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

// Function to convert a string to uppercase
void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    int pipe1[2]; // Pipe for parent to child communication
    int pipe2[2]; // Pipe for child to parent communication

    // Create both pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe failed");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) { // Parent process
        char input[BUFFER_SIZE];
        char output[BUFFER_SIZE];
        close(pipe1[0]); // Close the reading end of pipe1 (parent writes to this pipe)
        close(pipe2[1]); // Close the writing end of pipe2 (parent reads from this pipe)

        while (1) {
            // Read text from standard input
            printf("Enter some text: ");
            fgets(input, BUFFER_SIZE, stdin);

            // Remove newline character from input
            input[strcspn(input, "\n")] = 0;

            // Write the input text to the child process
            write(pipe1[1], input, strlen(input) + 1);

            // Read the modified text from the child process
            read(pipe2[0], output, BUFFER_SIZE);

            // Print the output received from the child
            printf("Uppercase: %s\n", output);
        }

        // Close pipes
        close(pipe1[1]);
        close(pipe2[0]);

    } else { // Child process
        char buffer[BUFFER_SIZE];
        close(pipe1[1]); // Close the writing end of pipe1 (child reads from this pipe)
        close(pipe2[0]); // Close the reading end of pipe2 (child writes to this pipe)

        while (1) {
            // Read the input text from the parent process
            read(pipe1[0], buffer, BUFFER_SIZE);

            // Convert the text to uppercase
            to_uppercase(buffer);

            // Send the uppercase text back to the parent process
            write(pipe2[1], buffer, strlen(buffer) + 1);
        }

        // Close pipes
        close(pipe1[0]);
        close(pipe2[1]);
    }

    return 0;
}

