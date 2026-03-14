#include <stdio.h>
int main() {
    float price, hour1,hour2, min1, min2, total_hours, total_price;
    int conv_hours, conv_min;
    FILE *file = fopen("svodka.txt","a");

    if (file == NULL) {
        printf("not file in directory dude");
        return 1;
    }

    printf("Укажите стоимость часа.\n");
    scanf("%f", &price);
    printf("Укажите начало рабочего дня и конец.\n");
    scanf("%f:%f %f:%f", &hour1, &min1, &hour2, &min2);
    hour1 = hour1 + min1 / 60.0;
    hour2 = hour2 + min2 / 60.0;
    total_hours = hour2 - hour1;
    total_price = total_hours * price;
    conv_hours = (int) total_hours;
    conv_min = (int)((total_hours - conv_hours) * 60);
    printf("Отработано:%d ч %d мин, Заработано:%.2f\n", conv_hours, conv_min, total_price);
    fprintf(file, "Часы %d Мин %d, Зп: %.2f\r\n", conv_hours, conv_min, total_price);
    fclose(file);

    return 0;
}