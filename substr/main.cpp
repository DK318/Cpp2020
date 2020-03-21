#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

const size_t BUFFER_SIZE = 8192;

size_t *prefix_function;

size_t calculate_prefix_function(size_t last_prefix, char cur_char, const char *str) {
    size_t k = last_prefix;
    while (k > 0 && cur_char != str[k]) {
        k = prefix_function[k - 1];
    }
    if (cur_char == str[k]) {
        k++;
    }
    return k;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " <substring> <filename>\n";
        return EXIT_FAILURE;
    }
    int fd = open(argv[2], O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        perror("open failed");
        return EXIT_FAILURE;
    }
    size_t n = strlen(argv[1]);
    prefix_function = new size_t[n];
    prefix_function[0] = 0;
    for (size_t i = 1; i < n; i++) {
        prefix_function[i] = calculate_prefix_function(prefix_function[i - 1],
                argv[1][i], argv[1]);
    }
    size_t last_prefix = 0;
    while (true) {
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read = read(fd, buffer, std::size(buffer));
        if (bytes_read < 0) {
            perror("read failed");
            close(fd);
            return EXIT_FAILURE;
        }
        if (bytes_read == 0) {
            break;
        }
        for (size_t i = 0; i < bytes_read; i++) {
            char cur_char = buffer[i];
            last_prefix = calculate_prefix_function(last_prefix, cur_char, argv[1]);
            if (last_prefix == n) {
                std::cout << "true\n";
                close(fd);
                return EXIT_SUCCESS;
            }
        }
    }
    close(fd);
    std::cout << "false\n";
    return EXIT_SUCCESS;
}