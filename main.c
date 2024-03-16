#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define PHILOSOPHER_COUNT 5
#define MAX_MEALS_REQUIRED_TO_FINISH 10 // Максимальна кількість спроб для завершення обіду

pthread_mutex_t forks[PHILOSOPHER_COUNT];
pthread_t philosophers[PHILOSOPHER_COUNT];

void think_or_eat(int philosopher_number, const char* action) {
    printf("Філософ %d %s.\n", philosopher_number, action);
    sleep(1 + rand() % 2); // Чекає випадковий час, імітуючи діяльність
}

void* philosopher(void* num) {
    int philosopher_number = *(int*)num;
    int meals_eaten = 0;

    while (meals_eaten < MAX_MEALS_REQUIRED_TO_FINISH) {
        // Філософ думає
        think_or_eat(philosopher_number, "думає");

        // Випадково вибираємо, яку виделку спробувати взяти спочатку
        int first_fork = philosopher_number;
        int second_fork = (philosopher_number + 1) % PHILOSOPHER_COUNT;

        // Блокування першої виделки
        pthread_mutex_lock(&forks[first_fork]);
        printf("Філософ %d взяв виделку %d.\n", philosopher_number, first_fork);

        // Спроба взяти другу виделку
        if (pthread_mutex_trylock(&forks[second_fork]) == 0) {
            printf("Філософ %d взяв виделку %d і починає їсти.\n", philosopher_number, second_fork);
            think_or_eat(philosopher_number, "їсть");
            meals_eaten++;
            pthread_mutex_unlock(&forks[second_fork]); // Відпускаємо другу виделку
            printf("Філософ %d поклав виделку %d.\n", philosopher_number, second_fork);
        } else {
            // Якщо не вдається взяти другу виделку, відпускаємо першу
            printf("Філософ %d не зміг взяти виделку %d і продовжує думати.\n", philosopher_number, second_fork);
        }
        pthread_mutex_unlock(&forks[first_fork]); // Відпускаємо першу виделку
        printf("Філософ %d поклав виделку %d.\n", philosopher_number, first_fork);

        if (meals_eaten >= MAX_MEALS_REQUIRED_TO_FINISH) {
            printf("Філософ %d завершив обід і відходить від столу.\n", philosopher_number);
            break;
        }
    }

    return NULL;
}


int main() {
    int i, numbers[PHILOSOPHER_COUNT];

    srand(time(NULL)); // Ініціалізація генератора випадкових чисел

    // Ініціалізація м'ютексів
    for (i = 0; i < PHILOSOPHER_COUNT; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    // Створення потоків-філософів
    for (i = 0; i < PHILOSOPHER_COUNT; i++) {
        numbers[i] = i;
        if (pthread_create(&philosophers[i], NULL, philosopher, &numbers[i])) {
            fprintf(stderr, "Помилка при створенні потока\n");
            return 1;
        }
    }

    // Очікування завершення потоків
    for (i = 0; i < PHILOSOPHER_COUNT; i++) {
        pthread_join(philosophers[i], NULL);
    }

    return 0;
}
