# Board namespace

General pattern
```
<project>.<category><id>.<descriptor>.v<rev>
```
Description

| Part       | Meaning              | Example                    |
|------------|----------------------|----------------------------|
| project    | project              | `opendeep`                 |
| category   | hardware class       | `mod`, `pba`, `pcb`, `asm` |
| id         | sequential family ID | `001`                      |
| descriptor | role / MCU / subtype | `eval`, `rp2040`           |
| v<rev>     | revision             | `v01`, `v02`               |

