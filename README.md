# Test signals generator STM32F4xx

## Описание проекта
Генератор набора тестовых сигналов под управлением микроконтроллера STM32F407 (Arm® Cortex®-M4, арзитектура Armv7E-M). Генерация сигналов на базе таймеров микроконтроллера и микросхем синтезатора AD9850.
Проект Keil MDK-ARM. Компилятор ARM v.6.

## Состав проекта и используемые модули
- Реализовано несколько драйверов: микросхема AD9850, дисплеи 128x64 MT12864 и ST7920, таймеры совместно с DMA, работа с кнопками и валкодером.
- Для пользовательского интерфейса реализована библиотека "UniGraphic": UnniRamGraphic для отображения графических примитивов в 2D массив, UnniRamGraphicUIObjects для создания компонентов пользовательского интерфейса.
- Компонент bsc_tree_menu отвечает за создание структуры меню в виде элементов, организованных древовидно в модификации двухсвязного списка, каждому узлу (странице) меню назначается своя функция отрисовки и обработчик событий.
- Отдельные драйверы и компоненты этого проекта автономны и применяются также в других проектах.

## Демонстрация пользовательского интерфейса и набора функций
- YouTube MCU STM32F4xx Signals generator user interface
[![Видео YouTube](https://img.youtube.com/vi/q_aCrORlHpU/maxresdefault.jpg)](https://youtu.be/q_aCrORlHpU)
