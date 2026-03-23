# MyApp / MyServer

Клиент-серверное C++17 приложение на TCP (POSIX sockets) с JSON обменом.

## Что делает приложение

- `MyServer` слушает TCP-порт, принимает JSON с массивом 4D-векторов (по факту одномерные), модифицирует каждый запрос случайным числом (Mersenne Twister), отправляет JSON-ответ.
- `MyApp` работает как интерактивный клиент: пользователь задает тип и значения векторов, складывает их в очередь, отправляет на сервер, получает ответ и продолжает работать в той же сессии.
- Клиентское соединение постоянное: поднимается на старте `MyApp`, закрывается при `exit/quit`.
- Сервер обрабатывает подключения пулом из 4 потоков.

## Поддерживаемое окружение

- OS: Linux (Ubuntu 24.04+)
- Компилятор: `g++` с поддержкой C++17
- CMake: 3.16+

## Зависимости

- `build-essential`
- `cmake`
- `git`

GoogleTest подтягивается автоматически через `FetchContent` при конфигурации CMake.

## Установка зависимостей (Ubuntu)

```bash
sudo apt update
sudo apt install -y build-essential cmake git
```

## Сборка

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j$(nproc)
```

## Запуск

### Сервер

```bash
./task1/MyServer -p 8080
```

Справка:

```bash
./task1/MyServer --help
```

### Клиент (MyApp)

```bash
./task1/MyApp -i 127.0.0.1 -p 8080
```

Справка:

```bash
./task1/MyApp --help
```

## Сценарий пользователя

1. Запустить сервер: `./task1/MyServer -p 8080`
2. Запустить клиент: `./task1/MyApp -i 127.0.0.1 -p 8080`
3. В клиенте:
   - `type double`
   - `vector 1 2 3 4`
   - `queue`
   - `vector 5 6 7 8`
   - `queue`
   - `send`
4. Клиент выведет измененные сервером векторы.
5. `exit` или `quit` завершает сессию и закрывает TCP соединение.

## Команды MyApp

- `help`
- `type <int|double|float>`
- `vector [n1 n2 n3 n4]`
- `print`
- `queue`
- `send`
- `push`
- `address`
- `alias <name>`
- `exit` / `quit`

## Логи

- `MyApp` пишет в `app.log`
- `MyServer` пишет в `server.log`

Логи содержат `INFO/WARNING/ERROR`, включая события подключения, отправки, приема и обработки данных.

## Тесты

```bash
cd build
ctest --output-on-failure
```

## Санитайзеры и статический анализ

Если нужен ASan/UBSan:

```bash
cmake -S . -B build-asan -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build-asan -j$(nproc)
```

Для `cppcheck` (если установлен):

```bash
cppcheck --enable=warning,performance,portability --std=c++17 --inline-suppr include src
```
