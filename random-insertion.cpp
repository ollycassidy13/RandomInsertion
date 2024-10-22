#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <cstdlib>
#include <ctime>
#include <cmath>

class RandomizedTruthTable {
public:
    RandomizedTruthTable(int n_inputs, int n_outputs, const std::vector<std::pair<std::string, std::string>>& truth_table, int rarity_threshold)
        : n_inputs(n_inputs), n_outputs(n_outputs), truth_table(truth_table), rarity_threshold(rarity_threshold) {
        // std::srand(std::time(0));
    }

    void process_dont_cares(const std::vector<std::string>& neuron_inputs) {
        if (neuron_inputs.empty()) {
            std::cerr << "Error: No neuron inputs detected" << std::endl;
            exit(EXIT_FAILURE);
        }
        for (const auto& pattern : neuron_inputs) {
            input_occurrences[pattern]++;
        }
        int total_dc = 0;
        for (const auto& entry : truth_table) {
            if (input_occurrences[entry.first] < rarity_threshold) {
                std::string random_output = generate_random_output();
                filtered_truth_table.emplace_back(entry.first, random_output);
                total_dc++;
            } else {
                filtered_truth_table.push_back(entry);
            }
        }
        std::cout << "Don't care conditions detected: " << total_dc << std::endl;
    }

    void generate_verilog(const std::string& filename, const std::string& module_name) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        file << "module " << module_name << " ( ";
        file << "input [" << (n_inputs - 1) << ":0] M0, ";
        file << "output [" << (n_outputs - 1) << ":0] M1 ";
        file << ");\n\n";

        file << "    (*rom_style = \"distributed\" *) reg [" << (n_outputs - 1) << ":0] M1r;\n";
        file << "    assign M1 = M1r;\n";
        file << "    always @ (M0) begin\n";
        file << "        case (M0)\n";

        for (const auto& entry : filtered_truth_table) {
            file << "            " << n_inputs << "'b" << entry.first << ": M1r = " << n_outputs << "'b" << entry.second << ";\n";
        }

        file << "        endcase\n";
        file << "    end\n";
        file << "endmodule\n";
        file.close();
    }

private:
    int n_inputs;
    int n_outputs;
    int rarity_threshold;
    std::vector<std::pair<std::string, std::string>> truth_table;
    std::vector<std::pair<std::string, std::string>> filtered_truth_table;
    std::unordered_map<std::string, int> input_occurrences;

    std::string generate_random_output() const {
        int max_value = (1 << n_outputs) - 1; 
        int random_value = std::rand() % (max_value + 1); 

        return std::bitset<32>(random_value).to_string().substr(32 - n_outputs);
    }
};

int calculate_bit_width(int value) {
    if (value == 0) return 0;
    return std::floor(std::log2(value));
}

std::pair<std::vector<std::pair<std::string, std::string>>, int> read_truth_table(const std::string& filename, const int& n_outputs) {
    std::ifstream file(filename);
    std::vector<std::pair<std::string, std::string>> truth_table;
    std::string line;

    while (std::getline(file, line)) {
        truth_table.emplace_back(); 
    }

    int n_inputs = calculate_bit_width(truth_table.size()); 

    file.clear();
    file.seekg(0, std::ios::beg);
    int i = 0;

    while (std::getline(file, line)) {
        std::stringstream ss;
        ss << std::hex << line;
        unsigned int value;
        ss >> value;

        std::string input_pattern = std::bitset<32>(i).to_string().substr(32 - n_inputs); 
        std::string output_pattern = std::bitset<32>(value).to_string().substr(32 - n_outputs); 

        truth_table[i] = std::make_pair(input_pattern, output_pattern);
        i++;
    }

    return std::make_pair(truth_table, n_inputs);
}

std::vector<std::string> read_neuron_inputs(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> neuron_inputs;
    std::string line;

    while (std::getline(file, line)) {
        neuron_inputs.push_back(line);
    }

    return neuron_inputs;
}

int main(int argc, char* argv[]) {
    std::string table_path;
    std::string input_path;
    std::string output_path = "./";
    std::string output_name = "output_file";
    int rarity = -1;
    int n_outputs = 0;

    for (int i = 1; i < argc; i += 2) {
        std::string current_arg = argv[i];
        
        if (current_arg == "-table") {
            table_path = argv[i + 1];
        } else if (current_arg == "-input") {
            input_path = argv[i + 1];
        } else if (current_arg == "-rarity") {
            rarity = std::stoi(argv[i + 1]); 
        } else if (current_arg == "-name") {
            output_name = argv[i + 1];
        } else if (current_arg == "-output") {
            output_path = argv[i + 1];
        } else if (current_arg == "-n_out") {
            n_outputs = std::stoi(argv[i + 1]);
        } else {
            std::cerr << "Unknown argument: " << current_arg << std::endl;
            return 1;
        }
    }

    if (table_path.empty() || input_path.empty() || rarity < 0 || n_outputs <= 0) {
        std::cerr << "All arguments that are required: -table <truth_table_file> -input <inputs_file> -rarity <rarity_threshold> -output <output_path> -name <output_name> -n_out <output_bitwidth>\n";
        return 1;
    }

    std::pair<std::vector<std::pair<std::string, std::string>>, int> from_truth_table = read_truth_table(table_path, n_outputs);
    auto truth_table = from_truth_table.first;
    int n_inputs = from_truth_table.second;
    auto neuron_inputs = read_neuron_inputs(input_path);

    RandomizedTruthTable random(n_inputs, n_outputs, truth_table, rarity);
    random.process_dont_cares(neuron_inputs);

    std::string verilog_filename = output_path + "/" + output_name + ".v";
    random.generate_verilog(verilog_filename, output_name);
    std::cout << "Verilog file generated: " << verilog_filename << std::endl;

    return 0;
}