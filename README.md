## Виды параметров
* **Switch-опция**: ```-h``` или ```--help``` или ```-las``` (три свича)
* **Flag-опция**: ```-C2```, ```-C 2```, ```--Col=2``` или ```--Col 2``` (параметров может быть несколько, т. е. это мини-позиционные аргументы)
* **Позиционный аргумент**: аргумент на конкретной позиции

### Пример выражения из символов и потока параметров
```
./name -las -h --header -C2 -D 5 --dendy=56 posarg1 posarg2

SwitchOption(l) SwitchOption(a) SwitchOption(s) SwitchOption(h) SwitchOption(header) FlagOption({C:2}) FlagOption({D:5}) FlagOption({dendy:56}) PosArg(posarg1) PosArg(posarg2)

```

## Интерфейс
### Базовый тип ```Parameter```
Тип, представляющий все возможные команды. Объект этого типа должен быть всегда в консистентном состоянии. Параметр может быть Switch ИЛИ Flag ИЛИ PosArg. Для такого есть std::variant.