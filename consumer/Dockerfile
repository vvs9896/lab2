# Используем официальный Debian образ в качестве базового
FROM debian:12

# Устанавливаем необходимые зависимости и компилируем программу в одном слое
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    libboost-all-dev

# Копируем исходные файлы программы в рабочую директорию
WORKDIR /app
COPY consumer.cpp .
COPY crow_all.h .
COPY json.hpp .

# Компилируем программу с поддержкой OpenMP и оптимизацией
RUN g++ -lpthread consumer.cpp -o consumer

# Открываем порт для приложения
EXPOSE 8080

# Запускаем программу
CMD ["./consumer"]