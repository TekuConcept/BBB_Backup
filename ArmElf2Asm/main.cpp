#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <set>
// #include <map>

#include "elf_object.h"
#include "arm_disassembler.h"

elf_object obj;

void print() {
    for (const auto& section : obj.sections()) {
        std::cout << ((section.name.size() == 0) ? "[no name]" : section.name) << "\n";
    }

    std::cout << "\n\n====================\nSymbols\n\n";

    for (const auto& symbol : obj.symbols()) {
        std::string name = "[no name]";
        if (symbol.name.size() != 0)
            name = symbol.name;
        std::cout << "\t- " << name << std::endl;
    }

    std::cout << "\n\n====================\nRelocations\n\n";

    for (const auto& relocation : obj.relocations()) {
        std::string name = "[no name]";
        if (relocation.symbol->name.size() != 0)
            name = relocation.symbol->name;
        std::cout << "\t- " << name << " => ";
        std::cout << elf_object::reloc_type_string(relocation.type) << ", ";
        
        std::cout << std::endl;
    }
}

int read_elf_file(std::string filename) {
    std::ifstream elf_file;
    elf_file.open(filename);

    if (!elf_file.is_open()) {
        std::cout << "Error opening file " << filename << "\n";
        return 1;
    }

    try { obj = elf_object::parse(elf_file); }
    catch (std::exception& e) {
        std::cout << "Error parsing elf file\n";
        std::cout << e.what() << std::endl;
        return 1;
    }

    elf_file.close();
    return 0;
}

void print_formatted_code(unsigned int idx) {
    struct function_t {
        std::string name;
        unsigned int offset;
        std::vector<std::string> code;
    };

    const elf_object::section_t& section = obj.sections()[idx];
    if (section.raw_data.size() == 0) return;
    auto instructions = disassemble2array(section.raw_data);

    //
    // Apply relocation symbols
    //

    std::vector<std::string> leftovers;
    for (const auto& reloc : obj.relocations()) {
        if (reloc.sidx != idx) continue;
        if (reloc.symbol->name.size() == 0) continue;
        std::ostringstream os;
        auto idx = reloc.offset >> 2;
        os << " @ " << reloc.symbol->name;
        os << " [" << elf_object::reloc_type_string(reloc.type) << ": ";
        os << (reloc.symbol->section ? reloc.symbol->section->name : "") << "+";
        os << "0x" << std::hex << reloc.symbol->value << "]";
        if (idx >= instructions.size()) {
            os << " 0x" << reloc.offset;
            leftovers.push_back(os.str());
        }
        else instructions[idx] += os.str();
    }

    //
    // Format Instructions
    //

    for (size_t i = 0; i < instructions.size(); i++) {
        std::replace(instructions[i].begin(), instructions[i].end(), ';', '@');
        std::replace(instructions[i].begin(), instructions[i].end(), '\t', ' ');

        std::istringstream iss(instructions[i]);
        std::vector<std::string> tokens(
            std::istream_iterator<std::string>{iss},
            std::istream_iterator<std::string>());
        if (tokens.size() == 0) continue;

        std::ostringstream os;

        // instruction
        os << std::setw(12) << std::left << tokens[0] << std::right;

        for (size_t j = 1; j < tokens.size(); j++) {
            // comments
            if (tokens[j][0] == '@')
                os << std::setw(42 - os.str().size()) << "@ ";
            // arguments
            else os << tokens[j] << " ";
        }
        instructions[i] = os.str();

        // to uppercase
        auto k = instructions[i].find_first_of("@");
        std::string::iterator end = (k != std::string::npos) ?
            instructions[i].begin() + k : instructions[i].end();
        for (size_t k = 0; k < instructions[i].size(); k++) {
            if (instructions[i][k] == 'x') {
                if (k == 0 || instructions[i][k - 1] != '0')
                    instructions[i][k] = 'X';
            }
            else if (instructions[i][k] == '@') break;
            else instructions[i][k] = std::toupper(instructions[i][k]);
        }
    }

    //
    // divide instructions into functions
    //

    std::vector<function_t> functions;
    for (const auto* symbol : section.symbols) {
        if (symbol->type != elf_object::sym_type_t::FUNCTION) continue;
        function_t function;
        function.name = symbol->name;
        function.offset = symbol->value;
        function.code.insert(
            function.code.end(),
            instructions.begin() + (symbol->value >> 2),
            instructions.begin() + ((symbol->value + symbol->size) >> 2)
        );
        functions.push_back(function);
    }

    //
    // insert jump labels
    //

    for (auto& function : functions) {
        std::set<std::pair<unsigned int,std::string>> labels;
        for (auto& instruction : function.code) {
            if (instruction[0] != 'B') continue;
            char c1 = instruction[1];
            char c2 = instruction[2];
            if ((c1 == ' ') ||
                (c1 == 'E' &&  c2 == 'Q') ||
                (c1 == 'N' &&  c2 == 'E') ||
                (c1 == 'C' && (c2 == 'S'  || c2 == 'C')) ||
                (c1 == 'H' && (c2 == 'S'  || c2 == 'I')) ||
                (c1 == 'L' && (c2 == 'O'  || c2 == 'S'   || c2 == 'T' || c2 == 'E')) ||
                (c1 == 'M' &&  c2 == 'I') ||
                (c1 == 'P' &&  c2 == 'L') ||
                (c1 == 'V' && (c2 == 'S'  || c2 == 'C')) ||
                (c1 == 'G' && (c2 == 'E'  || c2 == 'T')) ||
                (c1 == 'A' &&  c2 == 'L'))
            {
                std::istringstream iss(instruction);
                std::ostringstream label;
                std::ostringstream oss;
                std::vector<std::string> tokens(
                    std::istream_iterator<std::string>{iss},
                    std::istream_iterator<std::string>());
                if (tokens.size() < 2) continue;
                if (tokens[1].size() < 3 || (tokens[1][0] != '0' && tokens[1][1] != 'x')) continue;
                unsigned int offset = std::strtoul(tokens[1].data(), 0, 16);
                if (offset <   function.offset ||
                    offset >= (function.offset + (function.code.size() << 2)))
                    continue;
                label << function.name << "_x" << std::hex << offset;
                oss << std::setw(12) << std::left << tokens[0] << std::right << label.str();
                for (size_t j = 2; j < tokens.size(); j++) {
                    if (tokens[j][0] == '@')
                        oss << std::setw(42 - oss.str().size()) << "@ ";
                    else oss << tokens[j] << " ";
                }
                instruction = oss.str();
                labels.insert(std::pair<unsigned int, std::string>(
                    (offset - function.offset) >> 2, label.str()));
            }
        }
        std::for_each(labels.rbegin(), labels.rend(), [&](const std::pair<unsigned int,std::string>& label) {
            // assume label within function bounds
            function.code.insert(function.code.begin() + label.first, label.second + ":");
        });
    }

    //
    // print section info
    //

    std::cout << "@ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n";
    std::cout << "@ " << section.name << "\n";
    std::cout << "@ Size: 0x" << std::hex << section.raw_data.size() << std::dec << "\n";
    std::cout << "@ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n\n";

    //
    // print formatted functions
    //

    std::cout << std::hex << std::setfill('0');
    for (const auto& function : functions) {
        std::cout << "FUNC_BEGIN " << function.name << "\n";
        for (size_t i = 0, j = 0; i < function.code.size(); i++, j++) {
            const auto& code = function.code[i];
            if (code[code.size() - 1] != ':')
                std::cout << "    /*" << std::setw(8) << (function.offset + (j << 2)) << "*/ ";
            else {
                std::cout << "    ";
                j--;
            }
            std::cout << code << "\n";
        }
        std::cout << "FUNC_END " << function.name << "\n\n\n";
    }
    std::cout << std::dec << std::setfill(' ');

    // if (leftovers.size() > 0) {
    //     std::cout << "@ -- Leftover Relocations --\n\n";
    //     for (const auto& token : leftovers)
    //         std::cout << token << "\n";
    //     std::cout << "\n\n";
    // }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <elf-binary>\n";
        return 1;
    }

    if (read_elf_file(std::string(argv[1])) != 0)
        return 1;

    for (const auto& section : obj.sections()) {
        if (section.name == ".bss")
            std::cout << "unsigned char bss[" << section.size << "];\n\n";
        else if (section.name == ".data") {
            std::cout << "unsigned char " << section.name.substr(1);
            std::cout << "[" << section.size << "] = {";
            std::cout << std::hex << std::setfill('0');
            for (size_t i = 0; i < section.raw_data.size(); i++) {
                if (i % 16 == 0) std::cout << "\n    /*" << std::setw(4) << i << "*/ ";
                std::cout << "0x" << std::setw(2);
                std::cout << (int)(0xFF & section.raw_data[i]) << ", ";
            }
            std::cout << std::dec << std::setfill(' ');
            std::cout << "\n};\n\n";
        }
        else if (section.name.find(".rodata") != section.name.npos) {
            std::cout << "unsigned char " << section.name.substr(1);
            std::cout << "[" << section.size << "] = {\n";
            const auto& data = section.raw_data;
            std::ostringstream os;
            os << std::hex << std::setfill('0');
            os << "    /*0000*/ \"";
            for (size_t i = 0; i < data.size(); i++) {
                if (data[i] >= ' ' && data[i] <= '~') os << data[i];
                else if (data[i] == '\n') os << "\\n";
                else if (data[i] == '\r') os << "\\r";
                else if (data[i] == '\t') os << "\\t";
                else if (data[i] == 0) {
                    os << "\\0\",\n";
                    std::cout << os.str();
                    os.str("");
                    os << "    /*" << std::setw(4) << (i + 1) << "*/ \"";
                }
                else os << "\\x" << std::setw(2) << (int)(0xFF & data[i]);
            }
            std::cout << os.str() << "\"";
            std::cout << "\n};\n\n";
        }
    }

    const auto& sections = obj.sections();
    for (size_t i = 0; i < sections.size(); i++) {
        if (sections[i].type == elf_object::section_type_t::PROGBITS &&
            sections[i].name.rfind(".text") != sections[i].name.npos)
            print_formatted_code(i);
    }

    return 0;
}
