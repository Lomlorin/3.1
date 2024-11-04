#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

//Функция вывода прав доступа, пермишн-разрешение 
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

//Функция преобразования буквенных прав в восьмеричный формат, 
int letter_to_octal(const char *permissions) {
    int octal = 0;
    if (permissions[0] == 'r') octal += 4;
    if (permissions[1] == 'w') octal += 2;
    if (permissions[2] == 'x') octal += 1;
    return octal;
}
// Функция преобразования восьмеричных прав в буквенные
void octal_to_letter(int octal, char *permissions) {
    for (int i = 0; i < 3; i++) {
        int perm = (octal >> (3 * (2 - i))) & 7; // Получаем права для каждой группы
        permissions[i * 3] = (perm & 4) ? 'r' : '-';
        permissions[i * 3 + 1] = (perm & 2) ? 'w' : '-';
        permissions[i * 3 + 2] = (perm & 1) ? 'x' : '-';
    }
    permissions[9] = '0'; // Завершаем строку
}
//Функция отображения восьмеричных прав доступа 
void display_octal_permissions(const char *input) {
    int owner = letter_to_octal(input);
    int group = letter_to_octal(input + 3);
    int others = letter_to_octal(input + 6);
    printf("Octal representation: %d%d%d\n", owner, group, others);
}
//Функция отображения прав доступа файла 
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
//Функция изменения прав доступа файла 
void change_permissions(const char *filename, const char *permissions) {
    int mode = 0;

    // Проверка на восьмеричный формат
    if (strlen(permissions) == 3) {
        mode = strtol(permissions, NULL, 8);
    } else if (strlen(permissions) == 9) {
        // Преобразуем буквы в восьмеричное представление
        for (int i = 0; i < 9; i++) {
            if (permissions[i] == 'r') {
                mode |= (4 << (8 - i));
            } else if (permissions[i] == 'w') {
                mode |= (2 << (8 - i));
            } else if (permissions[i] == 'x') {
                mode |= (1 << (8 - i));
            }
        }
    } else {
        printf("Invalid permissions format. Must be either 3 digits or 9 characters long.\n");
        return;
    }

    // Изменяем права доступа
    if (chmod(filename, mode) < 0) {
        perror("chmod error");
        return;
    }

    printf("Permissions for file '%s' changed to '%s'\n", filename, permissions);
}

//в мэйне реализован интерфейс программы, объявляется переменная choice. где хран выбор пользователя

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
                
                if (strlen(permissions) == 3 || strlen(permissions) == 9) {
                    display_octal_permissions(permissions);
                } else if (strlen(permissions) == 3) {
                    // Если введены только три цифры, выводим буквенное представление
                    char letter_permissions[10];
                    octal_to_letter(strtol(permissions, NULL, 8), letter_permissions);
                    printf("Permissions in letter format: %sn", letter_permissions);
                } else {
                    printf("Invalid input format.n");
                }
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
        
        printf("n");
    }
    
    return 0;
}
