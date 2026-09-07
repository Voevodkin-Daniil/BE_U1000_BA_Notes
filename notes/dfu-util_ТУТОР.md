# dfu-util — полное руководство по USB DFU

## О документе

`dfu-util` — консольная утилита, реализующая host-side часть стандарта USB Device Firmware Upgrade. Она предназначена не для одного конкретного микроконтроллера: её можно использовать с любым устройством, которое предоставляет совместимый USB DFU-интерфейс.

Документ сначала описывает сам USB DFU и возможности `dfu-util`: поиск устройств, выбор по VID/PID, path, serial, interface и altsetting, переход из runtime в DFU mode, upload/download, reset, wait, verbose, DfuSe, автоматизацию и диагностику. В конце отдельно приведены два раздела для BE-U1000.

Официальный проект указывает поддержку USB DFU 1.0 и 1.1. Текущий опубликованный релиз на официальной странице — `0.11`. В некоторых vendor SDK встречаются более старые версии, поэтому перед использованием полезно выполнить:

```text
dfu-util -V
dfu-util -h
```

---

# Содержание

1. Что такое dfu-util
2. Что такое USB DFU
3. Runtime mode и DFU mode
4. Что dfu-util умеет
5. Установка
6. Windows и USB-драйверы
7. Linux
8. macOS
9. Проверка версии
10. Встроенная справка
11. `--list`
12. Как читать `Found DFU`
13. VID/PID
14. USB path
15. Configuration
16. Interface
17. Alternate Setting
18. Serial Number
19. Как dfu-util выбирает устройство
20. Download
21. Upload
22. Detach
23. Detach Delay
24. Wait
25. Reset
26. Transfer Size
27. Upload Size
28. Verbose
29. DFU state machine
30. DFU suffix и prefix
31. DfuSe
32. Скрипты и автоматизация
33. Несколько устройств
34. Безопасность прошивки
35. Типичные ошибки
36. BE-U1000: драйвер и alternate settings
37. BE-U1000: практические команды
38. Источники

---

# 1. Что такое dfu-util

`dfu-util` — host-side программа для USB DFU.

Схема:

```text
firmware.bin
     ↓
  dfu-util
     ↓
 USB stack
     ↓
USB DFU device
     ↓
bootloader / firmware
     ↓
Flash / RAM / другой target
```

Сама утилита не знает архитектуру вашего микроконтроллера.

Она знает:

```text
USB DFU protocol
USB descriptors
выбранный target
передаваемые байты
```

За смысл target отвечает устройство.

---

# 2. Что такое USB DFU

DFU:

```text
Device Firmware Upgrade
```

Стандарт описывает способ обновления firmware через USB.

Устройство предоставляет DFU functional descriptor и DFU interface.

DFU может применяться для:

- микроконтроллеров;
- отладочных плат;
- мобильных устройств;
- программаторов;
- USB-периферии;
- загрузчиков;
- промышленного оборудования.

Официальный сайт dfu-util указывает поддержку DFU 1.0 и DFU 1.1.

---

# 3. Runtime mode и DFU mode

DFU-устройство может существовать в двух состояниях.

## Runtime mode

Устройство выполняет обычную функцию:

```text
keyboard
sensor
network device
application
```

но содержит DFU runtime interface.

## DFU mode

Устройство находится в режиме обновления.

Схема:

```text
Application
→ DFU DETACH
→ USB re-enumeration
→ DFU mode
→ download/upload
```

VID/PID в runtime и DFU mode могут различаться.

Именно поэтому `--device` умеет отдельно задавать runtime и DFU VID/PID.

---

# 4. Что dfu-util умеет

Основные возможности:

```text
найти DFU-устройства
отфильтровать устройство
выбрать interface
выбрать alternate setting
загрузить firmware
прочитать firmware
попросить переход Runtime → DFU
подождать re-enumeration
выполнить USB reset
работать с DfuSe
вывести подробную диагностику
```

---

# 5. Установка

Официальный проект публикует binaries для Windows и поддерживает сборку на других системах.

Linux-дистрибутивы обычно имеют готовый пакет.

macOS:

```text
Homebrew
MacPorts
```

Windows:

```text
dfu-util.exe
libusb-compatible driver
```

---

# 6. Windows и USB-драйверы

Официальная страница dfu-util указывает, что программа использует `libusb 1.0`.

На Windows DFU-interface должен быть зарегистрирован с совместимым драйвером.

Официально перечислены:

```text
WinUSB
libusb-win32
libusbK
```

На практике часто используют:

```text
Zadig
```

для привязки конкретного USB interface к WinUSB.

## Важный принцип

Composite USB device может иметь несколько interfaces:

```text
Interface 0 → CDC
Interface 1 → HID
Interface 2 → DFU
```

Замена драйвера должна выполняться для нужного interface.

Нельзя бездумно ставить WinUSB на весь composite device, если другие interfaces должны работать через свои классовые драйверы.

---

# 7. Linux

Большинство Linux-дистрибутивов поставляют `dfu-util` как пакет.

После установки:

```bash
dfu-util -V
dfu-util -l
```

Если устройство видно только под root, проблема обычно связана с permissions/udev rules.

Типовой рабочий подход:

```text
udev rule
→ доступ пользователя
→ dfu-util без sudo
```

Не рекомендуется решать постоянную проблему прав запуском всей embedded toolchain от root.

---

# 8. macOS

Официальная страница проекта указывает:

```bash
brew install dfu-util
```

или установку через MacPorts.

Проверка:

```bash
dfu-util -V
```

---

# 9. Проверка версии

```bash
dfu-util -V
```

или:

```bash
dfu-util --version
```

Пример:

```text
dfu-util 0.11
```

Vendor SDK может включать другую версию.

При написании методички или скрипта полезно фиксировать:

```text
версию dfu-util
ОС
драйвер
VID/PID
```

---

# 10. Встроенная справка

```bash
dfu-util -h
```

или:

```bash
dfu-util --help
```

Это источник истины для реально установленной версии.

Например, новая версия может иметь опции, отсутствующие в старой vendor-сборке.

---

# 11. `--list`

Главная диагностическая команда:

```bash
dfu-util -l
```

Она показывает подключённые DFU-capable devices.

Пример абстрактного вывода:

```text
Found DFU: [1234:5678] ver=0110, devnum=5,
cfg=1, intf=0, path="1-2.3",
alt=0, name="Application",
serial="ABC123"
```

Перед любой автоматизацией полезно сначала научиться интерпретировать этот вывод.

---

# 12. Как читать `Found DFU`

Пример:

```text
[1234:5678]
```

Это:

```text
VID = 0x1234
PID = 0x5678
```

Дальше:

```text
ver=0110
```

версия DFU descriptor/protocol.

```text
devnum=5
```

текущий USB device number ОС.

```text
cfg=1
```

USB configuration.

```text
intf=0
```

номер interface.

```text
path="1-2.3"
```

физико-логический USB path.

```text
alt=0
```

alternate setting.

```text
name="Application"
```

строковое имя target.

```text
serial="ABC123"
```

серийный номер.

---

# 13. VID/PID и `--device`

Опция:

```text
-d
--device
```

Формат может учитывать отдельно Runtime и DFU mode.

Простейший пример:

```bash
dfu-util --device 1234:5678 -l
```

Официальная man page поддерживает формат:

```text
RunTimeVID:RunTimePID,DFUVID:DFUPID
```

Например:

```bash
dfu-util --device 1457:51ab,0951:0026 -l
```

Это удобно, если устройство меняет VID/PID при переходе в DFU.

Вариант:

```text
--device ,0951:0026
```

означает искать конкретный VID/PID только в DFU mode.

---

# 14. USB path

Опция:

```text
-p
--path
```

Пример:

```bash
dfu-util --path 1-2.3 -l
```

Path особенно полезен, если подключено несколько одинаковых устройств.

Важно:

```text
devnum != path
```

`devnum` может изменяться при re-enumeration.

`path` связан с расположением устройства в USB topology и часто стабильнее при подключении в тот же физический порт.

---

# 15. Configuration

Опция:

```text
-c
--cfg
```

Пример:

```bash
dfu-util --cfg 1 -l
```

Man page подчёркивает важную деталь:

```text
configuration используется для matching,
но dfu-util не переключает USB configuration этой опцией
```

То есть это фильтр, а не команда `SET_CONFIGURATION`.

---

# 16. Interface

Опция:

```text
-i
--intf
```

Пример:

```bash
dfu-util --intf 2 -l
```

Полезно для composite devices.

Схема:

```text
USB device
├── intf 0 → CDC
├── intf 1 → HID
└── intf 2 → DFU
```

---

# 17. Alternate Setting

Опция:

```text
-a
--alt
```

Можно выбирать по номеру или имени, если реализация/версия поддерживает имя.

Пример:

```bash
dfu-util --alt 0 --download app.bin
```

или:

```bash
dfu-util --alt Application --download app.bin
```

Смысл alt определяет устройство.

Пример:

```text
alt 0 → application
alt 1 → bootloader
alt 2 → config
```

У другого устройства те же номера могут означать совершенно другое.

---

# 18. Serial Number

Опция:

```text
-S
--serial
```

Пример:

```bash
dfu-util --serial ABC123 -l
```

Это один из лучших способов выбрать конкретную плату, если serial действительно уникален.

Man page позволяет отдельно задавать serial для runtime и DFU mode.

---

# 19. Как dfu-util выбирает устройство

Фильтры можно комбинировать:

```text
VID/PID
path
configuration
interface
alt
serial
```

Например:

```bash
dfu-util \
  --device 1234:5678 \
  --path 1-2.3 \
  --intf 0 \
  --alt 1 \
  --serial ABC123 \
  --download firmware.bin
```

Это полезно в производстве, где ошибка выбора устройства недопустима.

---

# 20. Download

Основная команда записи:

```text
-D FILE
--download FILE
```

Пример:

```bash
dfu-util --alt 0 --download firmware.bin
```

Короткая форма:

```bash
dfu-util -a 0 -D firmware.bin
```

Официальная man page также допускает:

```text
FILE = -
```

для чтения firmware из stdin.

Пример концептуально:

```bash
some_generator | dfu-util -a 0 -D -
```

Применять stdin следует только в хорошо контролируемых скриптах.

---

# 21. Upload

Опция:

```text
-U FILE
--upload FILE
```

Пример:

```bash
dfu-util --alt 0 --upload backup.bin
```

Это чтение данных из device target в файл.

Важно:

```text
поддержка upload зависит от самого устройства
```

Наличие download не гарантирует возможность чтения обратно.

Для некоторых targets firmware intentionally запрещает upload.

---

# 22. Detach

Опция:

```text
-e
--detach
```

Она просит DFU-capable runtime device выйти из обычного режима и re-enumerate в DFU mode.

Концептуально:

```text
runtime application
→ DFU_DETACH
→ disconnect/re-enumeration
→ DFU bootloader
```

Команда `--detach` после запроса завершается.

Устройство должно само поддерживать этот механизм.

---

# 23. Detach Delay

Опция:

```text
-E
--detach-delay SECONDS
```

При upload/download dfu-util после detach ждёт re-enumeration.

Man page указывает default:

```text
5 seconds
```

Пример:

```bash
dfu-util --detach-delay 10 --download firmware.bin
```

Это полезно для устройств, которые медленно перезапускаются в DFU mode.

---

# 24. Wait

Опция:

```text
-w
--wait
```

Заставляет dfu-util ждать появления подходящего устройства.

Пример:

```bash
dfu-util \
  --wait \
  --device 1234:5678 \
  --alt 0 \
  --download firmware.bin
```

Применение:

```text
запустить скрипт
→ оператор подключает устройство
→ dfu-util автоматически начинает работу
```

Удобно в производственных процедурах.

---

# 25. Reset

Опция:

```text
-R
--reset
```

После upload/download утилита отправляет USB reset signalling.

Пример:

```bash
dfu-util -a 0 -R -D firmware.bin
```

Это не всегда означает аппаратный reset MCU.

Фактическая реакция зависит от USB stack и DFU firmware устройства.

---

# 26. Transfer Size

Опция:

```text
-t
--transfer-size
```

Задаёт размер одного USB transfer.

Man page указывает, что оптимальное значение обычно определяется автоматически и эта опция требуется редко.

Пример:

```bash
dfu-util --transfer-size 1024 -D firmware.bin
```

Не следует менять transfer size без причины.

---

# 27. Upload Size

Опция:

```text
-Z
--upload-size
```

Задаёт ожидаемый размер upload для progress bar.

Важно:

```text
это не ограничение фактического размера чтения
```

Man page указывает, что реальный размер определяет устройство.

---

# 28. Verbose

```text
-v
--verbose
```

Уровни:

```bash
dfu-util -v -l
dfu-util -vv -l
dfu-util -vvv -l
```

Современная man page описывает:

```text
-v   → больше информации
-vv  → ещё больше деталей
-vvv → libusb USB request debug
```

Verbose нужен для диагностики:

- matching;
- descriptors;
- interface;
- DFU state;
- transfer;
- USB errors.

---

# 29. DFU state machine

USB DFU использует состояния, например:

```text
appIDLE
appDETACH
dfuIDLE
dfuDNLOAD-SYNC
dfuDNLOAD-IDLE
dfuMANIFEST
dfuUPLOAD-IDLE
dfuERROR
```

Пользователь часто видит только часть:

```text
dfuIDLE
dfuMANIFEST
```

Если утилита сообщает `dfuERROR`, важно смотреть status code и verbose-лог.

Состояния определяют допустимые следующие USB DFU requests.

---

# 30. DFU suffix и prefix

Официальный проект поставляет дополнительные инструменты.

## dfu-suffix

Используется для:

```text
add
check
remove
```

DFU firmware suffix.

Suffix помогает сопоставить файл и устройство.

## dfu-prefix

Используется для некоторых vendor-specific firmware formats, например TI LMDFU/Stellaris и NXP LPC.

Не следует автоматически добавлять prefix/suffix, если vendor bootloader ожидает raw binary.

---

# 31. DfuSe

DfuSe — расширение DFU, исторически связанное с ST.

Опция:

```text
-s
--dfuse-address
```

Формат:

```text
ADDRESS[:LENGTH][:MODIFIERS]
```

Примеры из официальной man page:

```bash
dfu-util -a 0 -s 0x08000000:1024 -U newfile.bin
```

Чтение 1 KiB.

```bash
dfu-util -a 0 -s 0x08004000:leave -D image.bin
```

Запись raw binary по адресу и запрос выхода из DFU.

Поддерживаются modifiers наподобие:

```text
leave
unprotect
mass-erase
force
will-reset
```

## Важно

`--dfuse-address` относится к DfuSe.

Нельзя применять STM32/DfuSe-рецепты к обычному USB DFU target, если его firmware не поддерживает DfuSe.

---

# 32. Скрипты и автоматизация

## Bash

```bash
#!/usr/bin/env bash
set -e

FW="build/app.bin"

test -f "$FW"

dfu-util -l
dfu-util -a 0 -D "$FW"
```

## PowerShell

```powershell
$Firmware = ".\build\app.bin"

if (-not (Test-Path $Firmware)) {
    throw "Firmware not found: $Firmware"
}

dfu-util -l

dfu-util `
    --alt 0 `
    --download $Firmware

if ($LASTEXITCODE -ne 0) {
    throw "DFU failed"
}
```

## CMD

```bat
@echo off
set FW=build\app.bin

if not exist "%FW%" (
    echo Firmware not found
    exit /b 1
)

dfu-util -a 0 -D "%FW%"

if errorlevel 1 exit /b 1
```

---

# 33. Несколько устройств

Если подключено несколько одинаковых boards:

```text
path="1-2"
serial="A001"

path="1-3"
serial="A002"
```

лучше задавать:

```bash
dfu-util --serial A001 ...
```

или:

```bash
dfu-util --path 1-2 ...
```

В производстве можно использовать несколько фильтров одновременно.

Не стоит полагаться только на `devnum`, потому что он может изменяться после re-enumeration.

---

# 34. Безопасность прошивки

Перед download важно понимать:

```text
куда устройство запишет данные
стирается ли Flash автоматически
какой размер target
какой формат ожидается
нужна ли подпись
что произойдёт после download
```

Успешное:

```text
Download done.
```

означает успешную передачу на уровне DFU, но не доказывает:

- правильность linker script;
- совместимость прошивки с hardware;
- валидность приложения;
- правильный target;
- успешный boot после reset.

Для критичных устройств желательно:

```text
1. проверить hash файла
2. проверить model/revision
3. проверить serial
4. выбрать target явно
5. логировать операцию
6. проверить версию после reboot
```

---

# 35. Типичные ошибки

## `dfu-util` не найден

```text
command not found
not recognized as an internal or external command
```

Решение:

```text
установить программу
добавить в PATH
или использовать полный путь
```

## `No DFU capable USB device available`

Проверить:

```text
устройство подключено?
оно в DFU mode?
драйвер доступен?
permissions?
VID/PID filter правильный?
```

## `Cannot open DFU device`

На Windows:

```text
неподходящий driver
не тот interface
доступ
```

На Linux:

```text
permissions / udev
```

## `No such file or directory`

Это локальная файловая ошибка:

```text
firmware file не найден
```

## Несколько устройств

dfu-util может отказаться работать без уточнения.

Добавить:

```text
--device
--path
--serial
```

## Upload не работает

Причина может быть в target firmware, который не разрешает чтение.

## После download код не запускается

Проверить:

```text
правильный target?
правильный адрес?
правильный формат?
правильный image?
нужен reset?
нужен leave?
```

## `Invalid DFU suffix signature`

Некоторые старые версии dfu-util предупреждают о raw-файле без suffix.

Проверить требования конкретного device/vendor.

---

# 36. BE-U1000: драйвер и alternate settings

Этот раздел относится только к BE-U1000.

## Windows driver

Для DFU interface BE-U1000 используется WinUSB.

Через Zadig:

```text
Options
→ List All Devices
→ TCM and run (Interface 2)
```

Проверить:

```text
USB ID = CAFE:2303
Interface = 02
```

Выбрать:

```text
WinUSB
```

и установить driver.

Не следует менять драйвер `CDC (Interface 0)`, если он нужен как виртуальный COM.

## Проверка

```powershell
dfu-util -l
```

Типичный BE-U1000:

```text
alt=0, name="TCM and run"
alt=1, name="Run"
alt=2, name="eFlash and run"
alt=3, name="eFlash"
alt=4, name="eFlash NVR id"
alt=5, name="eFlash NVR params"
alt=6, name="SPI"
alt=7, name="SPI part"
```

Назначение:

| Alt | Назначение |
|---:|---|
| 0 | TCM and run |
| 1 | управляющий Run target |
| 2 | eFlash Main and run |
| 3 | eFlash Main |
| 4 | NVR id |
| 5 | NVR params |
| 6 | QSPI/SPI target |
| 7 | QSPI/SPI partial/offset target |

> NVR и QSPI — энергонезависимые области. Их нельзя использовать как экспериментальные targets без понимания layout.

---

# 37. BE-U1000: практические команды

Этот раздел относится только к BE-U1000.

## Найти плату

```powershell
dfu-util -l
```

Пример:

```text
path="1-1"
```

Для `--path` используется:

```text
1-1
```

а не `devnum`.

## TCM

Сборка:

```powershell
make clean
make MEM_REG_ROM=TCMA BOARD=EVU_BA_2_5
```

Загрузка:

```powershell
dfu-util \
  --path 1-1 \
  --alt 0 \
  --download firmware.bin
```

В PowerShell одной строкой:

```powershell
dfu-util --path 1-1 --alt 0 --download .\output\debug\firmware.bin
```

После `Download done.` программа запускается из TCM.

Не нажимать RESET до проверки: после reset может загрузиться старая программа из eFlash.

## eFlash

Для постоянной записи использовать eFlash target и только image, собранный для eFlash.

Пример:

```powershell
dfu-util --path 1-1 --alt 2 --download firmware.bin
```

`alt 2`:

```text
eFlash and run
```

`alt 3`:

```text
eFlash
```

## Характерное предупреждение старого dfu-util

Версия `0.9` может вывести:

```text
Invalid DFU suffix signature
A valid DFU suffix will be required in a future dfu-util release!!!
```

если используется raw `.bin`.

Если далее есть:

```text
Download done.
```

передача данных состоялась.

## `unable to read DFU status after completion`

При `TCM and run` устройство может начать выполнять новый код и перестать отвечать как DFU interface до финального запроса старой версии dfu-util.

Проверять нужно фактический запуск:

```text
UART
LED
GPIO
другой ожидаемый эффект
```

---

# 38. Источники

- [dfu-util — официальный сайт](https://dfu-util.sourceforge.net/)
- [dfu-util — официальная man page](https://dfu-util.sourceforge.net/dfu-util.1.html)
- [dfu-util releases](https://dfu-util.sourceforge.net/releases/)
- [USB DFU specification](https://www.usb.org/document-library/device-firmware-upgrade-11-new-version-31-aug-2004)
- [BE-U1000 — руководство пользователя](https://mcu.baikalelectronics.ru/docs/mcu-docs/manual/)
- [BE-U1000 — Quick Start EVU-BA / EVU-LI](https://mcu.baikalelectronics.ru/docs/dev-boards/lite/EVU-BA_EVU-LI-start/)
