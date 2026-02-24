# CLI

![CI status](https://github.com/eyakm1/hse_spb_2026_swe_practice_1/actions/workflows/CI.yml/badge.svg?branch=master)

## Обзор

*CLI* – небольшой интерпретатор командной строки, поддерживающий базовые команды unix-подобных систем, окружение, пайплайны и расширяемость.

Интерпретатор поддерживает:

- `cat` — вывод содержимого файла.
- `echo` — печать аргументов.
- `wc` — подсчёт строк, слов и байт в файле.
- `pwd` — печать текущей директории.
- `grep` — поиск по регулярному выражению (ключи `-w`, `-i`, `-A N`; разбор аргументов через библиотеку CLI11, см. ниже).
- `exit` — завершение интерпретатора.
- Переменные окружения (`VAR=значение`, `$VAR`).
- Одинарные и двойные кавычки (полное и слабое экранирование).
- Вызов внешних программ (если команда не реализована явно).
- Пайплайны: `|` для передачи потока вывода между командами.

## Сборка и запуск

### Linux

Для сборки проекта необходимо установить компилятор `g++` хотя бы версии 13.3 и систему сборки CMake версии хотя бы 3.28

Далее сборки и запуска проекта:

```shell
cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target cli_app --config Release
./build/app/cli_app
```

### Windows

Для сборки проекта необходимо установить [MSVC тулчейн](https://visualstudio.microsoft.com/visual-cpp-build-tools/).

Далее для сборки и запуска проекта откройте "x64 Native Tools Command Prompt":

```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target cli_app --config Release
build\app\Release\cli_app.exe
```

## Архитектура

[Описание архитектуры приложения](docs/architecture.md)

## Контакты авторов

- eyakm1
- kristinadriam
- minmise

## Лицензия

MIT license

