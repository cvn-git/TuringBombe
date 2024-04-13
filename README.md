# TuringBombe
Simulation of Enigma machine and Turing's bombe

## `enigma_app.exe`

This is an Enigma simulator, supporting most military Enigma variants (including Kriegmarine M4).

Usage: `enigma_app <numrotors> <UKW> <R1> <R2> <R3> [R4] [steckers] <ring> <grun>`

| Param    | Description |
|----------|------------------|
|numrotors | Number of rotors (3 or 4)  |
|UKW       | Reflector (1:beta 2:gamma) |
|R1-R4     | Rotors (1-8, 1:beta 2:gamma) |
|ring      | Ringstellung (ring settings) |
|grun      | Grundstellung (Starting rotor display) |

(All rotor settings must be in left-to-right order)

Examples:
```dos
echo "The quick brown fox jumps over the lazy dog" | ./enigma_app 4 2 2 1 2 3 VXYZ PQDV
```
cypher text will be: `IRPMRQDZVBQIQDRDPCXGBAWODPBFIPISCAW`

```dos
echo "The quick brown fox jumps over the lazy dog" | ./enigma_app 4 2 2 1 2 3 AB:CD:EF:GH VXYZ PQDV
```
cypher text will be: `ICOMRQDZSAQIQMRCPDXHABWQCPWFIUISCBV`

To verify the above outputs, just decipher them using Rijmenants' Enigma simulator:
https://www.ciphermachinesandcryptology.com/en/enigmasim.htm

If settings are input correctly, Rijmenants' Enigma simulator will return the following result:
`THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG`

## `turing_bombe.exe`

This application runs the bombe for a given wheel order

Usage: `turing_bombe <menufile> <UKW> <R1> <R2> <R3> [R4]`

| Param    | Description |
|----------|------------------|
|menufile  | name of menu file |
|UKW       | Reflector (1:beta 2:gamma) |
|R1-R4     | Rotors (1-8, 1:beta 2:gamma) |

(All rotor settings must be in left-to-right order)

Examples:

```dos
./turing_bombe data/menu.txt 1 2 1 3
Bombe run takes 0.1127151 sec
1 2 1 3    BGX E:X
```

```dos
./turing_bombe data/test_menu.txt 1 1 2 4 1
Bombe run takes 6.6594702 sec
101 101 2 4 1    AXTW N:R
101 101 2 4 1    HJIE N:N
101 101 2 4 1    LIOE N:B
101 101 2 4 1    UIFO N:T
101 101 2 4 1    VJNG N:W
```

## `turing_bombe_all_wheels.exe`

This application runs the bombe for all M3/M4 wheel orders

Usage: `turing_bombe_all_wheels <menufile> <threads>`

| Param    | Description |
|----------|------------------|
|menufile  | name of menu file |
|threads   | Number of CPU cores |

Examples:

```dos
./enigma_app_all_wheels data/menu.txt 8
Work allocation:
Thread #1: 15 wheel orders
Thread #2: 15 wheel orders
Thread #3: 15 wheel orders
Thread #4: 15 wheel orders
Thread #5: 15 wheel orders
Thread #6: 15 wheel orders
Thread #7: 15 wheel orders
Thread #8: 15 wheel orders
Total: 120 wheel orders
Total 56 stops
All bombe runs take 0.842236 sec
```

```dos
./enigma_app_all_wheels data/test_menu.txt 8
Work allocation:
Thread #1: 168 wheel orders
Thread #2: 168 wheel orders
Thread #3: 168 wheel orders
Thread #4: 168 wheel orders
Thread #5: 168 wheel orders
Thread #6: 168 wheel orders
Thread #7: 168 wheel orders
Thread #8: 168 wheel orders
Total: 1344 wheel orders
Total 5298 stops
All bombe runs take 538.718 sec
```
