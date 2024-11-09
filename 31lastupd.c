#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Функция для преобразования буквенных прав в восьмеричный формат
int letter_to_octal(const char *permissions) {
    int octal = 0;
    if (permissions[0] == 'r') octal += 4;
    if (permissions[1] == 'w') octal += 2;
    if (permissions[2] == 'x') octal += 1;
    return octal;
}

// Функция для отображения битового представления прав доступа
void display_bit_representation(int octal) {
    printf("Bit representation: ");
    for (int i = 8; i >= 0; i--) 
    {
        printf("%d", (octal >> i) & 1);
        if (i % 3 == 0) printf(" "); // Для удобства разделяем группы
    }
    printf("\n");
}
 

// Функция для отображения прав доступа файла
void display_file_permissions(const char *filename) {
    struct stat fileStat;

    if (stat(filename, &fileStat) < 0) 
    {
        perror("stat error\n");
        return;
    }

    printf("File: %sn", filename);
    
    int owner = ((fileStat.st_mode >> 6) & 7);
    int group = ((fileStat.st_mode >> 3) & 7);
    int others = (fileStat.st_mode & 7);

    // Отображение прав доступа в буквенном формате
    printf("Permissions in letter format: ");
    printf((owner & 4) ? "r" : "-");
    printf((owner & 2) ? "w" : "-");
    printf((owner & 1) ? "x" : "-");
    printf((group & 4) ? "r" : "-");
    printf((group & 2) ? "w" : "-");
    printf((group & 1) ? "x" : "-");
    printf((others & 4) ? "r" : "-");
    printf((others & 2) ? "w" : "-");
    printf((others & 1) ? "x" : "-");
    printf("\n");

    // Отображение прав доступа в восьмеричном формате
    printf("Octal representation: %d%d%dn", owner, group, others);

    // Отображение битового представления
    display_bit_representation((owner << 6) | (group << 3) | others);
}

// Функция отображения восьмеричных прав доступа 
void display_octal_permissions(const char *input) {
    int owner = letter_to_octal(input);
    int group = letter_to_octal(input + 3);
    int others = letter_to_octal(input + 6);
    
    printf("Octal representation: %d%d%d\n", owner, group, others);
    
    // Отображение битового представления
    display_bit_representation((owner << 6) | (group << 3) | others);
}

mode_t parse_permissions(const char *permissions) {
    mode_t mode = 0;

    // Проверяем права для владельца
    if (permissions[0] == 'r') mode |= S_IRUSR; // чтение владельцем
    if (permissions[1] == 'w') mode |= S_IWUSR; // запись владельцем
    if (permissions[2] == 'x') mode |= S_IXUSR; // выполнение владельцем

    // Проверяем права для группы
    if (permissions[3] == 'r') mode |= S_IRGRP; // чтение группой
    if (permissions[4] == 'w') mode |= S_IWGRP; // запись группой
    if (permissions[5] == 'x') mode |= S_IXGRP; // выполнение группой

    // Проверяем права для остальных
    if (permissions[6] == 'r') mode |= S_IROTH; // чтение остальными
    if (permissions[7] == 'w') mode |= S_IWOTH; // запись остальными
    if (permissions[8] == 'x') mode |= S_IXOTH; // выполнение остальными

    
    return mode;
}

void change_file_permissions(const char *filename, const char *permissions) 
{

    mode_t mode = parse_permissions(permissions);
    
    if (chmod(filename, mode) == -1) {
        perror("Error changing access rights\n");
        exit(EXIT_FAILURE);
    }

    printf("File access rights '%s' have been changed to '%s'\n", filename, permissions);
}

// Основная функция программы
int main() {
    int choice;

    while (1) {
        printf("Choose an option:\n");
        printf("1. Enter permissions in letter format\n");
        printf("2. Enter permissions in octal format\n");
        printf("3. Enter a filename to display permissions\n");
        printf("4. Simulate changing permissions\n");
        printf("5. Exit\n");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char permissions[10];
                printf("Enter permissions (e.g. rwxr-xr--): ");
                scanf("%s", permissions);

                if (strlen(permissions) == 9) 
                {
                    display_octal_permissions(permissions);
                } 
                else 
                printf("Invalid input format.\n");
                break;
            }
            case 2: {
                int permissions[10];
                printf("Enter permissions (755): ");
                scanf("%o", permissions);
                display_bit_representation(*permissions);
                //else 
                //printf("Invalid input format.\n");
                break;
            }
            case 3: {
                char filename[256];
                printf("Enter filename: ");
                scanf("%s", filename);
                display_file_permissions(filename);
                break;
            }
            case 4: {
                /*char permissions[10];
                printf("Enter permissions (e.g. rwxr-xr--): ");
                scanf("%s", permissions);
                change_permissions_simulation(permissions);
                break;*/
                char filename[256];
                char permissions[10];

                printf("Enter filename: ");
                scanf("%255s", filename); // Ограничиваем длину ввода

                printf("Enter permissions (rwxrwxrwx): ");
                scanf("%9s", permissions); // Ограничиваем длину ввода
                display_file_permissions(filename);
                change_file_permissions(filename, permissions);
            }
            case 5:
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }

        printf("\n");
    }

    return 0;
}
