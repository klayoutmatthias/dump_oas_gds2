# dump_oas - a binary dump utility for OASIS (chip layout data) files 

This tool is a tool for debugging OASIS issues. It prints a formatted binary dump of the OASIS file.
It supports CBLOCK unfolding.

It is derived from KLayout's sources (www.klayout.org).

## Building

A Makefile is provided for building the binary. The only requirement is gcc (to use clang edit the Makefile).

In the source directory simply type

    make

The output binary will be "dump_oas".

On Windows, it is possible to build the tool using a Linux emulation shell (like MSYS2).

## Usage

The usage is simply

    dump_oas <path-to-oasis-file>


## Sample Output

```
000000000   25 53 45 4d 49 2d 4f 41  magic bytes
000000008 + 53 49 53 0d 0a 
000000013   01                       START
000000014   03 31 2e 30              version ("1.0")
000000018   00 e8 07                 resolution (1000)
000000021   01                       table flag (at end)
000000022   07 0d 53 5f 43 45 4c 4c  PROPNAME ("S_CELL_OFFSET")
000000030 + 5f 4f 46 46 53 45 54 
000000037   07 1a 53 5f 4d 41 58 5f  PROPNAME ("S_MAX_SIGNED_INTEGER_WIDTH")
000000045 + 53 49 47 4e 45 44 5f 49 
000000053 + 4e 54 45 47 45 52 5f 57 
000000061 + 49 44 54 48 
000000065   07 1c 53 5f 4d 41 58 5f  PROPNAME ("S_MAX_UNSIGNED_INTEGER_WIDTH")
000000073 + 55 4e 53 49 47 4e 45 44 
000000081 + 5f 49 4e 54 45 47 45 52 
000000089 + 5f 57 49 44 54 48 
000000095   07 0a 53 5f 54 4f 50 5f  PROPNAME ("S_TOP_CELL")
000000103 + 43 45 4c 4c 
000000107   1c 17 01                 PROPERTY (id=1)
000000110   08 04                    value[0]=4 (type 8)
000000112   1c 0f 02                 PROPERTY (id=2)
000000115   1c 17 03                 PROPERTY (id=3)
000000118   0b 06 54 4f 50 54 4f 50  value[0]=TOPTOP (type 11)
000000126   0d 00                    CELL (0)
000000128   14 7b                    RECTANGLE
000000130   01                       layer=1
000000131   00                       datatype=0
000000132   ce 22                    width=4430
000000134   8b 12                    height=2315
000000136   fb 06                    x=-445
000000138   e3 04                    y=-305
000000140   0d 01                    CELL (1)
000000142   11 f2                    PLACEMENT
000000144   00                       id=0
000000145   96 01                    x=75
000000147   be 01                    y=95
000000149   03 03 54 4f 50           CELLNAME ("TOP")
000000154   1c 17 00                 PROPERTY (id=0)
000000157   08 7e                    value[0]=126 (type 8)
000000159   03 06 54 4f 50 54 4f 50  CELLNAME ("TOPTOP")
000000167   1c 11                    PROPERTY (same id)
000000169   08 8c 01                 value[0]=140 (type 8)
000000172   02                       END
000000173   01                       tables entry (1)
000000174   95 01                    tables entry (149)
000000176   01                       tables entry (1)
000000177   6b                       tables entry (107)
000000178   01                       tables entry (1)
000000179   16                       tables entry (22)
000000180   01                       tables entry (1)
000000181   6b                       tables entry (107)
000000182   01                       tables entry (1)
000000183   6b                       tables entry (107)
000000184   01                       tables entry (1)
000000185   00                       tables entry (0)
000000186   80 80 80 80 80 80 80 80  padding string ("")
000000194 + 80 80 80 80 80 80 80 80 
000000202 + 80 80 80 80 80 80 80 80 
000000210 + 80 80 80 80 80 80 80 80 
000000218 + 80 80 80 80 80 80 80 80 
000000226 + 80 80 80 80 80 80 80 80 
000000234 + 80 80 80 80 80 80 80 80 
000000242 + 80 80 80 80 80 80 80 80 
000000250 + 80 80 80 80 80 80 80 80 
000000258 + 80 80 80 80 80 80 80 80 
000000266 + 80 80 80 80 80 80 80 80 
000000274 + 80 80 80 80 80 80 80 80 
000000282 + 80 80 80 80 80 80 80 80 
000000290 + 80 80 80 80 80 80 80 80 
000000298 + 80 80 80 80 80 80 80 80 
000000306 + 80 80 80 80 80 80 80 80 
000000314 + 80 80 80 80 80 80 80 80 
000000322 + 80 80 80 80 80 80 80 80 
000000330 + 80 80 80 80 80 80 80 80 
000000338 + 80 80 80 80 80 80 80 80 
000000346 + 80 80 80 80 80 80 80 80 
000000354 + 80 80 80 80 80 80 80 80 
000000362 + 80 80 80 80 80 80 80 80 
000000370 + 80 80 80 80 80 80 80 80 
000000378 + 80 80 80 80 80 80 80 80 
000000386 + 80 80 80 80 80 80 80 80 
000000394 + 80 80 80 80 80 80 80 80 
000000402 + 80 80 80 80 80 80 80 80 
000000410 + 80 80 80 80 80 80 80 80 
000000418 + 80 80 80 80 80 80 80 80 
000000426 + 00 
000000427   00                       validation scheme (0)
000000428                            tail
```


