#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Функция вывода прав доступа
void print_permissions(mode_t mode) {
    printf("Permissions: ");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
    printf("n");
}

// Функция преобразования буквенных прав в восьмеричный формат
int letter_to_octal(const char *permissions) {
    int octal = 0;
    for (int i = 0; i < 3; i++) {
        int perm = 0;
        perm += (permissions[i * 3] == 'r') ? 4 : 0; // Чтение для владельца/группы/других
        perm += (permissions[i * 3 + 1] == 'w') ? 2 : 0; // Запись
        perm += (permissions[i * 3 + 2] == 'x') ? 1 : 0; // Исполнение
        octal = (octal << 3) | perm; // Сдвигаем и добавляем права 
    }
    return octal;
}

// Функция отображения битового представления прав доступа
void display_bit_representation(int octal) {
    printf("Bit representation: ");
    for (int i = 8; i >= 0; i--) {
        printf("%d", (octal >> i) & 1);
        if (i % 3 == 0 && i != 0) {
            printf(" "); // Добавляем пробелы между группами
        }
    }
    printf("n");
}

// Функция отображения восьмеричных прав доступа
void display_octal_permissions(const char *input) {
    int octal = 0;
    
    // Проверяем, является ли ввод буквенным форматом прав
    if (strlen(input) == 9 && strspn(input, "rwx-") == strlen(input)) {
        octal = letter_to_octal(input);
        printf("Octal representation: %o\n", octal);
        display_bit_representation(octal);
        printf("Permissions in letter format: %s\n", input);
    } else {
        // Если ввод не в буквенном формате, пробуем интерпретировать как восьмеричное число
        octal = strtol(input, NULL, 8);
        printf("Octal representation: %o\n", octal);
        display_bit_representation(octal);

        // Выводим буквенное представление прав
        char letter_permissions[10];
        for (int i = 0; i < 3; i++) {
            int perm = (octal >> (3 * (2 - i))) & 7; // Получаем права для каждой группы
            letter_permissions[i * 3] = (perm & 4) ? 'r' : '-';
            letter_permissions[i * 3 + 1] = (perm & 2) ? 'w' : '-';
            letter_permissions[i * 3 + 2] = (perm & 1) ? 'x' : '-';
        }
        letter_permissions[9] = '0'; // Завершаем строку
        printf("Permissions in letter format: %s\n", letter_permissions);
    }
}

// Функция отображения прав доступа файла
void display_file_permissions(const char *filename) {
    struct stat fileStat;

    if (stat(filename, &fileStat) < 0) {
        perror("stat error");
        return;
    }

    printf("File: %s\n", filename);
    print_permissions(fileStat.st_mode);

    int owner = ((fileStat.st_mode >> 6) & 7);
    int group = ((fileStat.st_mode >> 3) & 7);
    int others = (fileStat.st_mode & 7);

    printf("Octal representation: %d%d%d\n", owner, group, others);
}

// Функция изменения прав доступа файла
void change_permissions(const char *filename, const char *permissions) {
    int mode = strtol(permissions, NULL, 8); // Преобразуем строку в восьмеричное число

    // Изменяем права доступа
    if (chmod(filename, mode) < 0) {
        perror("chmod error");
        return;
    }

    printf("Permissions for file '%s' changed to '%s'\n", filename, permissions);
}

// Основная функция
int main() {
    int choice;

    while (1) {
        printf("Choose an option:\n");
        printf("1. Enter permissions in letter or octal format\n");
        printf("2. Enter a filename to display permissions\n");
        printf("3. Change permissions of a file\n");
        printf("4. Exit\n");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char permissions[10];
                printf("Enter permissions (e.g. rwxr-xr-- or 755): ");
                scanf("%s", permissions);

                display_octal_permissions(permissions);
                break;
            }
            case 2: {
                char filename[256];
                printf("Enter filename: ");
                scanf("%s", filename);
                display_file_permissions(filename);
                break;
            }
            case 3: {
                char filename[256];
                char permissions[10];
                printf("Enter filename to change permissions: ");
                scanf("%s", filename);
                printf("Enter new permissions (e.g. rwxr-xr-- or 755): ");
                scanf("%s", permissions);

                change_permissions(filename, permissions);
                break;
            }
            case 4:
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }

        printf("\n");
    }

    return 0;
}
