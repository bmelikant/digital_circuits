#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_UTILITY 7
#define VIEW_MENU    6

void write_fetch_cycle (unsigned int *rom_data, int rom_sz, int step_count) {

	int fetch_steps = 0;
	unsigned int *fetch_step_data = NULL;

	printf ("Okay, write fetch cycle.\n");
	printf ("How many fetch steps will we be writing? ");
	scanf ("%d", &fetch_steps);

	fetch_step_data = (unsigned int *) malloc ((fetch_steps*sizeof(unsigned int)));

	for (size_t i = 0; i < fetch_steps; i++) {

		int fetch_data = 0;

		printf ("Enter the ROM data for fetch step %d: ", i+1);
		scanf ("%x", &fetch_data);
		printf ("\nVerified step %d data as 0x%x\n", i+1, fetch_data);
		fetch_step_data[i] = fetch_data;
	}

	printf ("Okay, that's great!\n");
	int increment = step_count;			// the number to increment by each loop
	printf ("I will write to a ROM of size %d. There are %d steps, so I will increment by %d each loop.\n\n", rom_sz, step_count, increment);

	for (size_t i = 0; i < rom_sz; i += increment)
		for (size_t j = 0; j < fetch_steps; j++)
			*(rom_data+i) = fetch_step_data[j];

	printf ("Okay, done writing fetch cycle to ROM!\n\n");
}

void write_instruction () {
}

void print_menu () {

	printf ("Main Menu - Options:\n\n");
	printf (" 1: Write fetch cycle to ROM image\n");
	printf (" 2: Write new instruction to ROM image\n");
	printf (" 3: Save ROM image to Logisim ROM file\n");
	printf (" 4: Load ROM image from Logisim ROM file\n");
	printf (" 5: Clear ROM image\n");
	printf (" 6: View this menu again\n");
	printf (" 7: Exit the utility\n\n");
}

int main () {

	unsigned int *rom_data = NULL;
	int rom_bits = 0;
	int step_bits = 0;
	int rom_size = 0;

	printf ("Welcome to 8 Bit Micro ROM Writer for Logisim!\n");
	printf ("v1.0 (C version) Ben Melikant 4-Aug-17\n\n");

	printf ("Okay, how many address bits are in the ROM file? ");
	scanf ("%d", &rom_bits);
	printf ("Thanks, %d rom bits!\n", rom_bits);
	
	rom_size = 1 << rom_bits;

	// get the number of bits that count as step bits
	printf ("Okay, and of the %d rom address bits, how many are step bits? ", rom_bits);
	scanf ("%d", &step_bits);

	// allocate the ROM data area
	printf ("Okay, there will be %d step bits for a max step count of %d.\n\n", step_bits, (1 << step_bits));
	rom_data = (unsigned int *) malloc ((rom_size*sizeof (unsigned int)));
	memset (rom_data, 0, rom_size);

	print_menu ();

	for (;;) {

		int choice = 0;
		printf ("Enter your choice: ");
		scanf ("%d", &choice);

		switch (choice) {

		case 1:

			write_fetch_cycle (rom_data, rom_size, (1 << step_bits));
			break;

		case 2:

			write_instruction ();
			break;

		case 5:

			memset ((void *) rom_data, 0, rom_size);
			printf ("ROM cleared.\n");
			break;

		case EXIT_UTILITY:

			printf ("Goodbye!\n\n");
			return 0;

		case VIEW_MENU:

			print_menu ();
			break;

		default:

			printf ("Command not recognized or not implemented yet.\n\n");
			break;
		}
	}

	// free the memory allocated for the ROM file
	free (rom_data);
	return 0;
}