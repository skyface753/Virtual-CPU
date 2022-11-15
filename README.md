# Virtual CPU

## Entrypoint

Folder: `test-programs`
File: `init`

## Instruction Set

| Command | Arguments | Description                     |
| ------- | --------- | ------------------------------- |
| L       | n         | Acc = n                         |
| A       |  n        | Acc = Acc + n                   |
| S       | n         | Acc = Acc - n                   |
| P       |           | Block running Process (4 Ticks) |
| X       | Filename  | Execute Filename                |
