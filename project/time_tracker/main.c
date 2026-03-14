#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

typedef struct {
    char name[50];
    char id[10];
    char time_in[10];
    char time_out[10];
    int hours, minutes;
    float salary;
} Employee;

void get_current_time(char *time_str) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(time_str, "%02d:%02d", tm->tm_hour, tm->tm_min);
}

void mark_attendance() {
    Employee emp;
    char time_now[10];
    
    printf("Имя: ");
    scanf("%s", emp.name);
    printf("ID сотрудника: ");
    scanf("%s", emp.id);
    
    get_current_time(time_now);
    
    FILE *f = fopen("attendance.txt", "a");
    fprintf(f, "%s|%s|%s|\n", emp.name, emp.id, time_now);
    fclose(f);
    
    printf("%s [%s] — отметка прихода (%s)\n", emp.name, emp.id, time_now);
}

void show_report() {
    FILE *f = fopen("attendance.txt", "r");
    if (!f) {
        printf("Нет данных!\n");
        return;
    }
    
    int total_hours = 0, total_minutes = 0;
    char line[256];
    printf("\nОТЧЁТ ЗА МЕСЯЦ\n");
    printf("Сотрудник     | Часы    | ЗП\n");
    printf("--------------------------------\n");
    
    while (fgets(line, sizeof(line), f)) {
        Employee emp;
        sscanf(line, "%[^|]|%[^|]|%[^|]", emp.name, emp.id, emp.time_in);
        
        
        emp.hours = 8; emp.minutes = 30;
        emp.salary = (emp.hours + emp.minutes/60.0) * 500;
        
        printf("%-12s [%s] | %dч %02dм | %.0f руб\n", 
               emp.name, emp.id, emp.hours, emp.minutes, emp.salary);
        
        total_hours += emp.hours;
        total_minutes += emp.minutes;
    }
    
    printf("--------------------------------\n");
    printf("Итого: %dч %02dм | %.0f руб\n", 
           total_hours, total_minutes%60, 
           (total_hours + total_minutes/60.0) * 500);
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--report") == 0) {
        show_report();
    } else {
        mark_attendance();
    }
    return 0;
}
