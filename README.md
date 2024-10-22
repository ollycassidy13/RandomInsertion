# Randomized Truth Table Insertion

This program inserts random values into a lookup table where *don't cares* are identified and outputs a Verilog file based on a specified rarity threshold and specified inputs.

## Description

The `RandomInsertion` program takes a truth table and corresponding inputs, filters the truth table based on a rarity threshold, and generates a corresponding Verilog file. This can be useful for reducing LUT utilization in FPGA designs.

## Usage

To compile the program, use a C++ compiler like g++:

```bash
g++ -o RandomInsertion.exe random-insertion.cpp
```

To run the program, use the following command:
```bash
./RandomInsertion.exe -table <truth_table_file> -input <inputs_file> -rarity <rarity_threshold> -output <output_path> -name <output_name> -n_out <n_outputs>
```

### Arguments

- `-table <truth_table_file>`: The path of the file containing the truth table data.
- `-input <inputs_file>`: The path of the file containing the specified inputs.
- `-rarity <rarity_threshold>`: The rarity threshold for filtering inputs (integer value).
- `-output <output_path>`: The directory where the output Verilog file will be saved.
- `-name <output_name>`: The name of the output Verilog module and file (without the .v extension).
- `-n_out <n_outputs>`: The output bitwidth (integer value).

**`-table`, `-input`, `-rarity`, `n_out` are all required arguements** 
### Example
```
./RandomInsertion.exe -table truth_table.txt -input inputs.txt -rarity 5 -output ./ -name output_file -n_out 4
```

This command will read the truth table from `truth_table.txt`, the specified inputs from `inputs.txt`, use a rarity threshold of 5, generate a Verilog file named `output_file.v` in the same directory as the script, and use an output bitwidth of 4.

**Directory Structure for this example**  
.<br>
├── random-insertion.cpp<br>
├── RandomInsertion.exe<br>
├── truth_table.txt<br>
├── inputs.txt<br>
└── output_file.v<br>

## Inputs

### Truth Table File

The truth table file should contain one hexadecimal value per line. Each line corresponds to an output for a given input pattern. The input patterns are assumed to be in increasing binary order. All outputs must be specified - the the file must contain $2^\beta$ values where $\beta$ is the input bitwidth.

Example:<br>
A<br>
B<br>
8<br>
D<br>
...

### Inputs File

The inputs file should contain one binary pattern per line. These patterns indicate which input combinations are passed to the model during the training process and are used for the purpose of filtering the truth table. Each pattern should be the same bitwidth as the modules input bitwidth.

Example:<br>
000000000000<br>
000000000001<br>
000000000010<br>
...

### Rarity

Inputs which occur in the inputs file rarity or more times will not have their corresponding output value replaced with a random value.

## Output

The program generates a Verilog file based on the filtered truth table. The Verilog file will have a module named <verilog_filename> with the specified input and output widths.

Example Verilog output:

```
module layer0_N0 ( input [11:0] M0, output [3:0] M1 );

    (*rom_style = "distributed" *) reg [3:0] M1r;
    assign M1 = M1r;
    always @ (M0) begin
        case (M0)
            12'b000000000000: M1r = 4'b0000;
            12'b000000000001: M1r = 4'b0001;
            // more cases...
        endcase
    end
endmodule
```
