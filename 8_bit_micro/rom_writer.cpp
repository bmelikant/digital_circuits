#include <iostream>
#include <fstream>
#include <iomanip>
#include <bits/stdc++.h>

#include <cstdlib>
#include <cstring>

const int EXIT_UTILITY = 7;
const int VIEW_MENU = 6;

using namespace std;

int count_varbits (string inp) {

        int count = 0;

        for (size_t i = 0; i < inp.length (); i++)
                if (inp[i] == 'x' || inp[i] == 'X')
                        count++;

        return count;
}

void get_bit_positions (string inp, int *varbit_loc, int step_bits) {

        int count = 0;
        string cpy = inp;

        reverse(cpy.begin(), cpy.end());
        cout << "Reversed string: " << cpy << endl;

        for (size_t i = 0; i < cpy.length(); i++)
               if (cpy[i] == 'x' || cpy[i] == 'X')
                        varbit_loc[count++] = i + step_bits;
}

int transform_varbits (string inp, bool one) {

        string new_value = "";

        for (size_t i = 0; i < inp.length (); i++) {

                if (inp[i] == 'x' || inp[i] == 'X')
                        new_value += (one ? '1' : '0');
                else
                        new_value += inp[i];
        }

        // translate the number!
        return strtol (new_value.c_str(), NULL, 2);
}

void write_fetch_cycle (unsigned int *rom_data, int rom_sz, int step_count) {

	int fetch_steps = 0;
	unsigned int *fetch_step_data = NULL;

	cout << "Okay, write fetch cycle." << endl;
	cout << "How many fetch steps will we be writing? ";
	cin >> fetch_steps;

	fetch_step_data = new unsigned int [fetch_steps];

	for (size_t i = 0; i < fetch_steps; i++) {

		cout << "Enter the ROM data for fetch step " << i+1 << ": ";
		cin >> fetch_step_data[i];
		cout << endl << "Verified step " << i+1 << " as " << setbase(16) << "0x" << fetch_step_data[i] << setbase(10) << endl;
	}

	cout << "Okay, that's great!" << endl;
	int increment = step_count;			// the number to increment by each loop

	for (size_t i = 0; i < rom_sz; i += increment)
		for (size_t j = 0; j < fetch_steps; j++)
			*(rom_data+(i+j)) = *(fetch_step_data+j);

	delete [] fetch_step_data;
	cout << "Okay, done writing fetch cycle to ROM!" << endl << endl;
}

void write_instruction (unsigned int *rom_data, int rom_sz, int step_bits) {

	int instr_steps = 0;
        int start_step = 0;
	unsigned int *instr_step_data = NULL;
        string instr_addr;

	cout << "Okay, write instruction cycle." << endl;
	cout << "How many steps is the instruction? ";
	cin >> instr_steps;

        cout << "On which step should the instruction start? ";
        cin >> start_step;
        
        cout << "Enter the address bits for the instruction." << endl;
        cout << "It should be a binary number. Use x or X to denote a number" << endl;
        cout << "that can be either 1 or 0. Don't enter the " << step_bits << " step bits." << endl << endl;

        cout << "Instruction address: ";
        cin >> instr_addr;

        int varbits = count_varbits (instr_addr);
        int *varbit_loc = new int [varbits];
        get_bit_positions (instr_addr, varbit_loc, step_bits);

        int start_pos = transform_varbits (instr_addr, false) << step_bits;
        int end_pos = transform_varbits (instr_addr, true) << step_bits;

        for (size_t i = 0; i < varbits; i++)
                cout << "Varbit location " << i << ": " << varbit_loc[i] << endl;

        cout << "I found " << varbits << " variable bits in the instruction address." << endl;
        cout << "I'll have to execute " << (1 << varbits) << " loops to insert this instruction." << endl;
        cout << "With the " << instr_steps << " steps, the loop total goes up to " << (1 << varbits) * instr_steps << endl;
        cout << "I'll start writing at address 0x" << setbase(16) << start_pos << " and go up to address 0x" << end_pos << setbase(10) << endl;

	instr_step_data = new unsigned int [instr_steps];

	for (size_t i = 0; i < instr_steps; i++) {

		cout << "Enter the ROM data for instruction step " << i+1 << ": ";
		cin >> instr_step_data[i];
		cout << endl << "Step " << i+1 << " contains " << setbase(16) << "0x" << instr_step_data[i] << setbase(10) << endl;
	}

        int counter = 0;

        for (size_t i = start_pos; i < end_pos; ) {

                i = start_pos;

                // compute the new rom_data location
                for (size_t j = 0; j < varbits; j++)
                        if (counter & (1 << j))
                                i = i | (1 << varbit_loc[j]);

                for (size_t j = 0; j < instr_steps; j++)
                        rom_data[i+(j+start_step)] = instr_step_data[j];

                counter++;
        }
}

void save_rom_file (unsigned int *rom_data, int rom_sz) {

        string fname;
        ofstream fo;

        cout << "Enter filename to save: ";
        cin >> fname;
        cout << endl;

        fo.open (fname.c_str());
        fo << "v2.0 raw" << (char) 0x0a;
        fo << setbase(16);

        cout << setbase (16);

        // write the ROM data
        for (size_t i = 0; i < rom_sz; ) {

                for (size_t j = 0; j < 8; j++) {

                        // build the next entry
                        size_t count = 1;

                        while (rom_data[i] == rom_data[i+count] && i+count < rom_sz)
                                count++;

                        if (count > 1)
                                fo << setbase (10) << count << '*' << setbase (16) << (int) (rom_data[i]) << ' ';
                        else
                                fo << setbase (16) << (int)(rom_data[i]) << ' ';

                        i += count;
                }

                fo << (char) 0x0a;
        }

        fo.flush ();
        fo.close ();
}

void print_menu () {

	cout << "Main Menu - Options:" << endl << endl;
	cout << " 1: Write fetch cycle to ROM image" << endl;
	cout << " 2: Write new instruction to ROM image" << endl;
	cout << " 3: Save ROM image to Logisim ROM file" << endl;
	cout << " 4: Load ROM image from Logisim ROM file" << endl;
	cout << " 5: Clear ROM image" << endl;
	cout << " 6: View this menu again" << endl;
	cout << " 7: Exit the utility" << endl << endl;
}

int main () {

	unsigned int *rom_data = NULL;
	int rom_bits = 0;
	int step_bits = 0;
	int rom_size = 0;

	cin.unsetf (ios::dec);
	cin.unsetf (ios::hex);
	cin.unsetf (ios::oct);

	cout << "Welcome to 8 Bit Micro ROM Writer for Logisim!" << endl;
	cout << "v1.0 (C version) Ben Melikant 4-Aug-17" << endl << endl;

	cout << "Okay, how many address bits are in the ROM file? ";
	cin >> rom_bits;
	cout << "Okay, I got " << rom_bits << " ROM address bits." << endl;
	
	rom_size = 1 << rom_bits;

	// get the number of bits that count as step bits
	cout << "Okay, and of the " << rom_bits << " ROM address bits, how many are step bits? ";
	cin >> step_bits;

	// allocate the ROM data area
	cout << "Okay, there will be " << step_bits << " step bits for a max step count of " << (1 << step_bits) << endl << endl;
	rom_data = new unsigned int [rom_size];
	memset (rom_data, 0, rom_size);

	print_menu ();

	for (;;) {

		int choice = 0;
		cout << "Enter your choice: ";
		cin >> choice;

		switch (choice) {

		case 1:

			write_fetch_cycle (rom_data, rom_size, (1 << step_bits));
			break;

		case 2:

			write_instruction (rom_data, rom_size, step_bits);
			break;

		case 3:

			save_rom_file (rom_data, rom_size);
			break;

		case 5:

			memset ((void *) rom_data, 0, rom_size);
			cout << "ROM cleared." << endl;
			break;

		case EXIT_UTILITY:

			cout << "Goodbye!" << endl << endl;
			return 0;

		case VIEW_MENU:

			print_menu ();
			break;

		default:

			cout << "Command not recognized or not implemented yet." << endl << endl;
			break;
		}
	}

	// free the memory allocated for the ROM file
	delete [] rom_data;
	return 0;
}
