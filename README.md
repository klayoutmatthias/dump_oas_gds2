# dump_oas and dump_gds2 - binary dump utilities for OASIS and GDS2 (chip layout data) files 

This tool is a tool for debugging OASIS and GDS2 issues. They print a formatted binary dump of the file.

 * "dump_oas" is for dumping OASIS files. It supports CBLOCK unfolding.

 * "dump_gds2" is for dumping GDS2 files.

Both tools are derived from KLayout's sources (www.klayout.org).

## Building

A Makefile is provided for building the binaries. The requirements are currently:

 * gcc/g++
 * libz

In the source directory simply type

    make

The output binary will be "dump_oas" and "dump_gds2".

On Windows, it is possible to build the tool using a Linux emulation shell (like MSYS2).

## Usage

The usage of the tools is simply

    dump_oas <path-to-oasis-file>
    dump_gds2 <path-to-gds2-file>

Further options are:

 * *-h* to print the help text
 * *-s* for short output (no multiline hex dump)
 * *-n <num>* to set the number of bytes per line

## Sample Output of "dump_oas"

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

## Sample Output of "dump_gds2"

```
000000000   00 06 00 02              HEADER
000000004   02 58                      600
000000006   00 1c 01 02              BGNLIB
000000010   07 e0 00 0c 00 07 00 17    2016-12-07 23:16:57
000000018 + 00 10 00 39 
000000022   07 e0 00 0c 00 07 00 17    2016-12-07 23:16:57
000000030 + 00 10 00 39 
000000034   00 08 02 06              LIBNAME
000000038   4c 49 42 00                "LIB"
000000042   00 14 03 05              UNITS
000000046   3e 41 89 37 4b c6 a7 f0    0.001
000000054   39 44 b8 2f a0 9b 5a 54    1e-09
000000062   00 1c 05 02              BGNSTR
000000066   07 e0 00 0c 00 07 00 17    2016-12-07 23:16:57
000000074 + 00 10 00 39 
000000078   07 e0 00 0c 00 07 00 17    2016-12-07 23:16:57
000000086 + 00 10 00 39 
000000090   00 08 06 06              STRNAME
000000094   54 4f 50 00                "TOP"
000000098   00 04 08 00              BOUNDARY
000000102   00 06 0d 02              LAYER
000000106   00 01                      1
000000108   00 06 0e 02              DATATYPE
000000112   00 64                      100
000000114   00 4c 10 03              XY
000000118   00 4b e9 98 ff 67 69 80    4975000,-10000000
000000126   00 4b e9 98 ff f9 e5 80    4975000,-400000
000000134   ff ad fb e8 ff f9 e5 80    -5375000,-400000
000000142   ff ad fb e8 00 00 00 00    -5375000,0
000000150   00 4b e9 98 00 00 00 00    4975000,0
000000158   00 4b e9 98 00 98 96 80    4975000,10000000
000000166   00 52 04 18 00 98 96 80    5375000,10000000
000000174   00 52 04 18 ff 67 69 80    5375000,-10000000
000000182   00 4b e9 98 ff 67 69 80    4975000,-10000000
000000190   00 04 11 00              ENDEL
000000194   00 04 07 00              ENDSTR
000000198   00 04 04 00              ENDLIB
```
