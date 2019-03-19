#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdlib>

// the ROM is 0x10000 in size
const int rom_size = 0x10000;

// clear the ROM image
void clear_rom_image (unsigned short *rom, int rom_sz) {
        
        // zero out the ROM image
        memset (rom, 0, rom_sz);
}

// write the fetch cycle into the ROM image
// this is the basic two-step fetch cycle for each instruction
void write_fetch_cycle (unsigned short *rom, int rom_sz) {

        // base fetch cycle. 0x0005 0x0128
        for (size_t i = 0; i < rom_sz; i += 8) {

                rom[i] = 0x0005;                // read program counter into memory address register
                rom[i+1] = 0x0128;              // read instruction from memory and inc pc
        }
}

// write the halt instruction into memory
// write the fetch cycle for memory arguments
void write_fetch_memory_source (unsigned short *rom, int rom_sz) {

        for (size_t i = 0x10; i < rom_sz; i += 0x80) {

                rom[i+2] = rom[i+0x22] = 0x0005;
                rom[i+3] = rom[i+0x23] = 0x0148;              // read integer into IR2 and inc pc     
        }

        for (size_t i = 0x40; i < rom_sz; i += 0x80) {

                rom[i+2] = rom[i+0x22] = 0x0005;
                rom[i+3] = rom[i+0x23] = 0x148;               // read integer into IR2 and inc pc
        }
}

// write the fetch cycle for immediate arguments (only as source, cannot be destination arguments)
void write_fetch_immediate_arg (unsigned short *rom, int rom_sz) {

        for (size_t i = 0x18; i < rom_sz; i += 0x80) {

                rom[i+2] = rom[i+0x22] = 0x0005;
                rom[i+3] = rom[i+0x23] = 0x0148;                // read integer into IR2 and inc pc             
        }
}

void save_rom_file (unsigned short *rom_data, int rom_sz) {

        std::string fname;
        std::ofstream fo;

        std::cout << "Enter filename to save: ";
        std::cin >> fname;
        std::cout << std::endl;

        fo.open (fname.c_str());
        fo << "v2.0 raw" << (char) 0x0a;
        fo << std::setbase(16);

        std::cout << std::setbase (16);

        // write the ROM data
        for (size_t i = 0; i < rom_sz; ) {

                for (size_t j = 0; j < 8; j++) {

                        // build the next entry
                        size_t count = 1;

                        while (rom_data[i] == rom_data[i+count] && i+count < rom_sz)
                                count++;

                        if (count > 1)
                                fo << std::setbase (10) << count << '*' << std::setbase (16) << (int) (rom_data[i]) << ' ';
                        else
                                fo << std::setbase (16) << (int)(rom_data[i]) << ' ';

                        i += count;
                }

                fo << (char) 0x0a;
        }

        fo.flush ();
        fo.close ();
}

// display the menu
void print_menu () {

        std::cout << "Options:" << std::endl;
        std::cout << " 1: write fetch cycle to ROM image" << std::endl;
        std::cout << " 2: add an instruction definition to ROM" << std::endl;
        std::cout << " 3: save ROM image" << std::endl;
        std::cout << " 4: show this menu again" << std::endl;
        std::cout << " 5: exit" << std::endl << std::endl;
}

// create Logisim ROM files for instruction encoding
int main (int argc, char *argv[]) {

        std::cout << "Logisim ROM writer v0.1" << std::endl;
        std::cout << "Ben Melikant, 02-Aug-2017" << std::endl << std::endl;

        std::cout << "Clearing ROM image..." << std::endl;
        
        unsigned short *rom_data = new unsigned short[rom_size];
        clear_rom_image (rom_data, rom_size);

        std::cout << "Done." << std::endl;

        for (;;) {

                int choice = 0;
                print_menu ();
                std::cin >> choice;

                switch (choice) {

                case 1:

                        write_fetch_cycle (rom_data, rom_size);
                        write_fetch_immediate_arg (rom_data, rom_size);
                        write_fetch_memory_source (rom_data, rom_size);

                        break;

                case 3:

                        save_rom_file (rom_data, rom_size);
                        break;

                case 5:

                        exit (0);

                default:

                        std::cout << "Command not recognized or not implemented yet!" << std::endl;
                        break;
                }
        }
}

