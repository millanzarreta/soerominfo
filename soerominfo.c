/*************************************************************
SoE Rom Info v0.1 - 2021

Author: millanzarreta    Date: 17/01/2022

License: GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007
*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#if (defined(_WIN32) || defined(_WIN64)) && (!defined(__MINGW32__) || defined(__MINGW64__))
#include <processthreadsapi.h>
#else
#include <unistd.h>
#endif

#define MAX_FILENAME_LEN 4096
#define MAX_STR_LEN 2560
#define MAX_BUFF_SIZE 20480

typedef struct {
	char filein[MAX_FILENAME_LEN];			// input filename (ROM)
	char fileout[MAX_FILENAME_LEN];			// output filename (general)
	int type;								// 0 = autodetect, 1 = unheadered ROM, 2 = headered ROM
	unsigned int hsize;						// header size in bytes
	char spacer;							// spacer in the scv output file
}param_options_type;

typedef struct {
	char filename[MAX_FILENAME_LEN];
	char header_present;
	unsigned int header_size;
	unsigned short int maker_code;
	char maker_code_str[3];
	unsigned int companyId;
	char company_str[MAX_STR_LEN];
	unsigned int game_code;
	char game_code_str[5];
	char valid_fixed_values;
	unsigned char expansion_RAM_size_code;
	unsigned int expansion_RAM_size;
	unsigned char special_version;
	unsigned char cartridge_type;
	char rom_name[22];
	unsigned char map_mode;
	char map_mode_str[MAX_STR_LEN];
	unsigned char type;
	char type_str[MAX_STR_LEN];
	unsigned char region;
	char region_str[MAX_STR_LEN];
	char video_type[MAX_STR_LEN];
	unsigned char rom_size_code;
	unsigned int rom_size;
	unsigned int calculated_rom_size;
	unsigned char sram_size_code;
	unsigned int sram_size;
	char version;
	unsigned short int rom_checksum;
	unsigned short int rom_complement_check;
	char valid_checksum_and_complement;
	unsigned short int real_checksum;
	unsigned short int real_complement_check;
	char coincident_checksum_and_complement;
}rom_info_type;

typedef struct {
	short int index;
	char name[MAX_STR_LEN];
	short int hp;
	short int attack;
	short int defense;
	short int magicdef;
	short int evade;
	short int hit;
	short int aggression_range;
	unsigned short int aggression_chance_code;
	double aggression_chance;
	unsigned short int unique_trait_flags;
	char unique_trait_flags_str[MAX_STR_LEN];
	unsigned short int attack_properties;
	char attack_properties_str[MAX_STR_LEN];
	int experience;
	short int money;
	unsigned char chance_prize_drop_code;
	double chance_prize_drop;
	short int chargeup_limit;
	short int chargeup_gain_per_frame;
	short int unknown_maybe_collision_related;
	unsigned short int palette;
	unsigned short int death_script_nospoils;
	unsigned short int death_script_spoils;
	unsigned short int block_script;
	int stat_ptr;
	int name_ptr;
}npc_stats_type;

typedef struct {
	short int index;						// reverse alphabetical order
	char name[MAX_STR_LEN];
	unsigned short int sprite_info_ptr;
	int sprite_ptr;
	int name_ptr;
	unsigned int ingredient_graphic_setup;	// different order: (alphabetical order except Acorn placed last and Atlas Amulet place between Gunpowder and Iron)
}ingredient_info_type;

typedef struct {
	short int index;
	char name[MAX_STR_LEN];
	char description[MAX_STR_LEN];
	unsigned short int spell_might_value;
	unsigned short int target_flags;
	char target_flags_str[MAX_STR_LEN];
	unsigned char ingredient1;
	unsigned char ingredient2;
	unsigned char ingredient1_cost;
	unsigned char ingredient2_cost;
	char ingredient1_name[MAX_STR_LEN];
	char ingredient2_name[MAX_STR_LEN];
	short int spell_influence_type;
	char spell_influence_type_str[MAX_STR_LEN];
	unsigned short int learned_alchemy_flags_addresses;
	unsigned short int learned_alchemy_flags_bit_weights;
	unsigned short int behaviour_script_ptr;
	unsigned short int invocation_script_ptr;
	unsigned short int sprite_info_ptr;
	unsigned short int unknown_maybe_sprite_related;
	int name_ptr;
	int description_ptr;
}alchemy_info_type;

typedef struct {
	short int index;
	char spell_name[MAX_STR_LEN];
	short int spell_influence_type;
	char spell_influence_type_str[MAX_STR_LEN];
	unsigned short int spell_might_value;
	//unsigned short int spell_power_value;		// Unknown [0x11ADD0-0x11AE30] (97 bytes)
	unsigned short int target_flags;			// Flare spell has a value but is ignored
	char target_flags_str[MAX_STR_LEN];
	unsigned short int invocation_script_ptr;
	unsigned short int sprite_info_ptr;
	int spell_name_ptr;							// 0xE85D4-0xE8653 (all regions) (0x80 bytes, 0x08 bytes each spell) (x00 name ptr) last 3 are messy (13->15, 14->13, 15->14)
	unsigned short int sprite_related_ptr;		// 0xE85D4-0xE8653 (all regions) (0x80 bytes, 0x08 bytes each spell) (x02 sprite related ptr) last 3 are messy (13->15, 14->13, 15->14)
	unsigned short int unknown_value1;			// 0xE85D4-0xE8653 (all regions) (0x80 bytes, 0x08 bytes each spell) (x04 unknown value1) last 3 are messy (13->15, 14->13, 15->14)
	unsigned short int unknown_value2;			// 0xE85D4-0xE8653 (all regions) (0x80 bytes, 0x08 bytes each spell) (x06 unknown value2) last 3 are messy (13->15, 14->13, 15->14)
}callbead_info_type;

typedef struct {
	short int index;
	char name[MAX_STR_LEN];
	char description[MAX_STR_LEN];
	short int attack_power;
	unsigned short int unknown_raw_data[16];
	unsigned short int obtained_flags_addresses;	//13*2=26 (Order: Swords, Axe, Spears, Baz)
	unsigned short int obtained_flags_bit_weights;	//13*2=26 (Order: Swords, Axe, Spears, Baz)
	unsigned short int familie;						//13*2=26 (Order: Swords, Axe, Spears, Baz)
	char familie_str[MAX_STR_LEN];
	int data_ptr;									//15*2=30 (Order: Swords, Axe, Spears, BazMunition)
	int name_ptr;
	int description_ptr;							//12*2=24 (Order: Swords, Axe, Spears)
}weapon_info_type;

typedef struct {
	short int index;
	char name[MAX_STR_LEN];
	short int deffense_value_effective;
	short int deffense_value_visual;
	unsigned short int unknown_pointer1;
	unsigned short int unknown_value1;
	short int default_sell_price;
	int data_ptr;
	unsigned short int sprite_info_ptr;
	int name_ptr;
}armor_info_type;

typedef struct {
	short int index;
	char unit[MAX_STR_LEN];
	int exp_needed_per_level[99];					// 0xC8BD0-0xC8CF8 (0x129) = Boy; 0xC8CFC-0xC8E24 = Dog (0x129) | total experience needed to reach level (99 entries, 3 bytes each)
														// All-non-USA: 0xC8F65-0xC908D [+0x395]; 0xC9091-0xC91B9 [+0x395]
	short int evade_per_level[111];					// 0xC8E25-0xC8F02 (0xDE) = Boy; 0xC8FE1-0xC90BE = Dog (0xDE) | Evade % by level (entries for 0-110, 2 bytes each, Thug's Cloak +5 to used level)
														// All-non-USA: 0xC91BA-0xC9297 [+0x395]; 0xC9376-0xC9453! [+0x395]
	short int hit_per_level[111];					// 0xC8F03-0xC8FE0 (0xDE) = Boy; 0xC90BF-0xC919C = Dog (0xDE) | Hit % by level (entries for 0-110, 2 bytes each, Jade Disk +5 to used level)
														// All-non-USA: 0xC9298-0xC9375 [+0x395]; 0xC9454-0xC9531 [+0x395]
	short int maxhp_per_level[111];					// 0xC919D-0xC927A (0xDE) = Boy; 0xC9515-0xC95F2 = Dog (0xDE) | Max HP by level (entries for 0-110, 2 bytes each, Chocobo Egg +5 to used level)
														// All-non-USA: 0xC9532-0xC960F [+0x395]; 0xC98AA-0xC9987 [+0x395]
	short int base_defense_per_level[111];			// 0xC927B-0xC9358 (0xDE) = Boy; 0xC95F3-0xC96D0 = Dog (0xDE) | base Defense by level (entries for 0-110, 2 bytes each, Staff of Life +5 to used level)
														// All-non-USA: 0xC9610-0xC96ED [+0x395]; 0xC9988-0xC9A65 [+0x395]
	short int base_attack_per_level[111];			// 0xC9359-0xC9436 (0xDE) = Boy; 0xC96D1-0xC97AE = Dog (0xDE) | base Attack by level (entries for 0-110, 2 bytes each, Sun Stone +5 to used level)
														// All-non-USA: 0xC96EE-0xC97CB [+0x395]; 0xC9A66-0xC9B43! [+0x395]
	short int magic_defense_per_level[111];			// 0xC9437-0xC9514 (0xDE) = Boy; 0xC97AF-0xC988C = Dog (0xDE) | Magic Defense by level (entries for 0-110, 2 bytes each, Wizard's Coin +7/+5 to used level)
														// All-non-USA: 0xC97CC-0xC98A9 [+0x395]; 0xC9B44-0xC9C21 [+0x395]
	char text_level_up_str[MAX_STR_LEN];			// 0xF8388-0xF8397 (0x10) = Boy Text: " reaches level "; 0xF856D-0xF857C (0x10) = Dog Text: " reaches level "
														// Europe:  0x51DE8-0x51DF7 [-0xA65A0]; 0x51DE8-0x51DF7 [-0xA6785]
														// Germany: 0x51F04-0x51F14 [-0xA6484]; 0x51F04-0x51F14 [-0xA6669]
														// France:  0x41C38-0x41C48 [-0xB6750]; 0x41C38-0x41C48 [-0xB6935]
														// Spain:   0x41FFD-0x4200D [-0xB638B]; 0x41FFD-0x4200D [-0xB6570]
	char text_weaponlevel_up_str[MAX_STR_LEN];		// 0xF97B1-0xF97BF (0x0F) = Boy Text: " is now level "; 0xF9801-0xF9816 (0x16) = Dog Text: "Dog attack now level "
														// Europe:  0x51DD9-0x51DE7 [-0xA79D8]; 0x51DC3-0x51DD8 [-0xA7A3E]
														// Germany: 0x51EF3-0x51F03 [-0xA78BE]; 0x51EDF-0x51EF2 [-0xA7922]
														// France:  0x41C2B-0x41C37 [-0xB7B86]; 0x41C13-0x41C2A [-0xB7BEE]
														// Spain:   0x41FED-0x41FFC [-0xB77C4]; 0x41FD4-0x41FEC [-0xB782D]
	char level_up_func_str[MAX_STR_LEN];											// 0xF8310-0xF8387 (0x78) = Boy's Level Up handled (function); 0xF84F5-0xF856C (0x78) = Dog's Level Up handled (function)
	unsigned int level_up_func_startaddr;												// Same for ALL!!; All non-USA: 0xF84E5-0xF855C [-0x10]
	unsigned int level_up_func_endaddr;
	unsigned int level_up_func_size;
	unsigned int level_up_func_crc;
	char stats_established_func_str[MAX_STR_LEN];									// 0xF8398-0xF84F4 (0x15D) = Boy's stats established (function); 0xF857D-0xF8688 (0x10C) = Dog's stats established (function)
	unsigned int stats_established_func_startaddr;										// All non-USA: 0xF8388-0xF84E4 [-0x10]; 0xF855D-0xF8668 [-0x20]
	unsigned int stats_established_func_endaddr;
	unsigned int stats_established_func_size;
	unsigned int stats_established_func_crc;
	char weapon_exp_handling_func_str[MAX_STR_LEN];									// 0xF973E-0xF97B0 (0x73) = Boy's Weapon EXP handling (function); 0xF97C0-0xF9800 (0x41) = Dog's Attack EXP handling (function)
	unsigned int weapon_exp_handling_func_startaddr;									// All non-USA: 0xF9714-0xF9786 [-0x2A]; 0xF9787-0xF97C7 [-0x39]
	unsigned int weapon_exp_handling_func_endaddr;
	unsigned int weapon_exp_handling_func_size;
	unsigned int weapon_exp_handling_func_crc;
	char attack_calculation_func_str[MAX_STR_LEN];									// 0xFB75A-0xFB77C (0x23) = Attack Calculations Script (function) $08: Boy Standard; 0xFB77D-0xFB79F (0x23) = Attack Calculations Script (function) $0A: Dog Standard
	unsigned int attack_calculation_func_startaddr;										// All non-USA: 0xFB70B-0xFB72D [-0x4F]; 0xFB72E-0xFB750 [-0x4F]
	unsigned int attack_calculation_func_endaddr;
	unsigned int attack_calculation_func_size;
	unsigned int attack_calculation_func_crc;
	char establish_main_ring_info_func_str[MAX_STR_LEN];							// 0xE9A86-0xE9B39 (0xB4) = Script (function) - Establishes Main Ring information for both Boy and Dog
	unsigned int establish_main_ring_info_func_startaddr;								// Same for ALL!!
	unsigned int establish_main_ring_info_func_endaddr;
	unsigned int establish_main_ring_info_func_size;
	unsigned int establish_main_ring_info_func_crc;
	char update_palettes_status_outline_func_str[MAX_STR_LEN];						// 0x10C9FC-0x10CA9A (0x9F) = Updates Boy and Dog's Palettes based on Status Outline handling (functions)
	unsigned int update_palettes_status_outline_func_startaddr;							// All non-USA: 0x10CA01-0x10CA9F [+0x5]
	unsigned int update_palettes_status_outline_func_endaddr;
	unsigned int update_palettes_status_outline_func_size;
	unsigned int update_palettes_status_outline_func_crc;
	char update_last2colors_status_outline_func_str[MAX_STR_LEN];					// 0x10CA9B-0x10CBB2 (0x118) = Updates last 2 colors of Boy/Dog's Palette for Status Outline (X is Character; alternates between statuses) (function)
	unsigned int update_last2colors_status_outline_func_startaddr;						// All non-USA: 0x10CAA0-0x10CBB7 [+0x5]
	unsigned int update_last2colors_status_outline_func_endaddr;
	unsigned int update_last2colors_status_outline_func_size;
	unsigned int update_last2colors_status_outline_func_crc;
	char establish_current_palette_row_default_and_udpate_func_str[MAX_STR_LEN];	// 0x10CDDC-0x10CE14 (0x39) = Establishes current Palette Row to default one, and also updates Boy/Dog's Palette Row accordingly (Y is entity) (function)
	unsigned int establish_current_palette_row_default_and_udpate_func_startaddr;		// All non-USA: 0x10CDFF-0x10CE37 [+0x23]
	unsigned int establish_current_palette_row_default_and_udpate_func_endaddr;
	unsigned int establish_current_palette_row_default_and_udpate_func_size;
	unsigned int establish_current_palette_row_default_and_udpate_func_crc;
	char spell_instruction_status_outline_palette_bits_set_func_str[MAX_STR_LEN];	// 0x11AC84-0x11ACBC (0x39) = Spell Instruction $44: Boy and Dog's Status Outline Palette Bits set (function)
	unsigned int spell_instruction_status_outline_palette_bits_set_func_startaddr;		// All non-USA: 0x11AC64-0x11AC9C [-0x20]
	unsigned int spell_instruction_status_outline_palette_bits_set_func_endaddr;
	unsigned int spell_instruction_status_outline_palette_bits_set_func_size;
	unsigned int spell_instruction_status_outline_palette_bits_set_func_crc;
	char update_status_outline_status_removal_func_str[MAX_STR_LEN];				// 0x11BB01-0x11BB26 (0x26) = Updates Boy/Dog's Status Outline according to to status removal (Y is entity) (function)
	unsigned int update_status_outline_status_removal_func_startaddr;					// All non-USA: 0x11B9F0-0x11BA15 [-0x111]
	unsigned int update_status_outline_status_removal_func_endaddr;
	unsigned int update_status_outline_status_removal_func_size;
	unsigned int update_status_outline_status_removal_func_crc;
}boydog_info_type;

//////////////////////
// TODO: Possible future data to add
// FriendlyNPCs -> Name (and ptr), Stats_Data (like monsters), Stats_Data_PTR
// Items -> Name (and ptr), TargetingFlags, InvocationScriptPtr, HPRecoveryAmount, SpriteInfoPtr, DefaultSellPrizes
// Charms/Rare Items -> Name (and ptr), Description (and ptr), Type (Charm or Rare), Sprite_ptr, item_flags_(addresses), item_flags_(bit_weights), #total_number_of_charms?
// Trade good Items -> Name (and ptr), Sprite_ptr, #total_number_trade_goods?
// Merchants -> NPC Name (and ptr), ¿?¿?¿?, Stats_Data_PTR
//////////////////////

// Function to show the main menu
void showmenu(char *filename, char *version) {
	printf("\nSoE ROM Info v%s - 2021 - millanzarreta\n", version);
	printf("\nUse: %s [options] <file_rom_in> [file_out_name]\n\n", filename);
	printf("Options:\n");
	printf("\t-s <char>,      --spacer <char> : spacer for the csv output files. Default: ,\n");
	printf("\t-t <0, 1 or 2>, --type <0..2>   : 0 = autodetect header, 1 = unheadered ROM, 2 = headered ROM. Default: 0 (auto)\n");
	printf("\t-h <size>,      --hsize <size>  : header size in bytes (only needed for headered ROM). Default: 512\n");
	printf("\t-?,             --help          : show this help message\n\n");
}

// Function to remove extension from filename string
char *remove_ext(char* myStr, char extSep) {
#if (defined(_WIN32) || defined(_WIN64))
	char *pathSep = "\\/";
#else
	char pathSep = '/';
#endif
	char *lastExt, *lastPath;
	if (myStr == NULL) return NULL;
	if (strlen(myStr) == 0u) return myStr;
	lastExt = strrchr(myStr, extSep);
#if (defined(_WIN32) || defined(_WIN64))
	lastPath = strrchr(myStr, pathSep[0]);
	if (lastPath == NULL) {
		lastPath = strrchr(myStr, pathSep[1]);
	}
#else
	lastPath = strrchr(myStr, pathSep);
#endif
	if (lastExt != NULL) {
		if (lastPath != NULL) {
			if (lastPath < lastExt) {
				*lastExt = '\0';
			}
		} else {
			*lastExt = '\0';
		}
	}
	return myStr;
}

// Function to remove path from filename string
char *remove_path(char* myStr) {
	if (myStr == NULL) return NULL;
	if (strlen(myStr) == 0u) return myStr;
	if ((myStr[strlen(myStr)-1] == '\\') || (myStr[strlen(myStr)-1] == '/')) {
		myStr[strlen(myStr)-1] = '\0';
	}
#if (defined(_WIN32) || defined(_WIN64))
	char *pathSep1 = "\\";
	char *pathSep2 = "/";
	char *ssc, *ssc2;
	ssc = strstr(myStr, pathSep1);
	ssc2 = strstr(myStr, pathSep2);
	ssc = (ssc == NULL) ? ((ssc2 == NULL) ? NULL : ssc2) : ((ssc2 == NULL) ? ssc : ((ssc > ssc2) ? ssc : ssc2));
	while(ssc != NULL){
		strcpy(myStr, ssc+1);
		ssc = strstr(myStr, pathSep1);
		ssc2 = strstr(myStr, pathSep2);
		ssc = (ssc == NULL) ? ((ssc2 == NULL) ? NULL : ssc2) : ((ssc2 == NULL) ? ssc : ((ssc > ssc2) ? ssc : ssc2));
	};
#else
	char *pathSep = "/";
	char *ssc;
	ssc = strstr(myStr, pathSep);
	while(ssc != NULL){
		strcpy(myStr, ssc+1);
		ssc = strstr(myStr, pathSep);
	};
#endif
	return myStr;
}

// Function to get the company name string from companyId number
void companyIdStr(unsigned int companyId, char *destText) {
	switch(companyId) {
		case 0:strcpy(destText, "INVALID COMPANY");break;
		case 1:strcpy(destText, "Nintendo");break;
		case 2:strcpy(destText, "Ajinomoto");break;
		case 3:strcpy(destText, "Imagineer-Zoom");break;
		case 4:strcpy(destText, "Chris Gray Enterprises Inc.");break;
		case 5:strcpy(destText, "Zamuse");break;
		case 6:strcpy(destText, "Falcom");break;
		case 7:strcpy(destText, "(unknown: 7)");break;
		case 8:strcpy(destText, "Capcom");break;
		case 9:strcpy(destText, "HOT-B");break;
		case 10:strcpy(destText, "Jaleco");break;
		case 11:strcpy(destText, "Coconuts");break;
		case 12:strcpy(destText, "Rage Software");break;
		case 13:strcpy(destText, "Micronet"); break; //Acc. ZFE
		case 14:strcpy(destText, "Technos");break;
		case 15:strcpy(destText, "Mebio Software");break;
		case 16:strcpy(destText, "SHOUEi System"); break; //Acc. ZFE
		case 17:strcpy(destText, "Starfish");break; //UCON 64
		case 18:strcpy(destText, "Gremlin Graphics");break;
		case 19:strcpy(destText, "Electronic Arts");break;
		case 20:strcpy(destText, "NCS / Masaya"); break; //Acc. ZFE
		case 21:strcpy(destText, "COBRA Team");break;
		case 22:strcpy(destText, "Human/Field");break;
		case 23:strcpy(destText, "KOEI");break;
		case 24:strcpy(destText, "Hudson Soft");break;
		case 25:strcpy(destText, "Game Village");break;//uCON64
		case 26:strcpy(destText, "Yanoman");break;
		case 27:strcpy(destText, "(unknown: 27)");break;
		case 28:strcpy(destText, "Tecmo");break;
		case 29:strcpy(destText, "(unknown: 29)");break;
		case 30:strcpy(destText, "Open System");break;
		case 31:strcpy(destText, "Virgin Games");break;
		case 32:strcpy(destText, "KSS");break;
		case 33:strcpy(destText, "Sunsoft");break;
		case 34:strcpy(destText, "POW");break;
		case 35:strcpy(destText, "Micro World");break;
		case 36:strcpy(destText, "(unknown: 36)");break;
		case 37:strcpy(destText, "(unknown: 37)");break;
		case 38:strcpy(destText, "Enix");break;
		case 39:strcpy(destText, "Loriciel/Electro Brain");break;//uCON64
		case 40:strcpy(destText, "Kemco");break;
		case 41:strcpy(destText, "Seta Co.,Ltd.");break;
		case 42:strcpy(destText, "Culture Brain"); break; //Acc. ZFE
		case 43:strcpy(destText, "Irem Japan");break;//Irem? Gun Force J
		case 44:strcpy(destText, "Pal Soft"); break; //Acc. ZFE
		case 45:strcpy(destText, "Visit Co.,Ltd.");break;
		case 46:strcpy(destText, "INTEC Inc."); break; //Acc. ZFE
		case 47:strcpy(destText, "System Sacom Corp."); break; //Acc. ZFE
		case 48:strcpy(destText, "Viacom New Media");break; //Zoop!
		case 49:strcpy(destText, "Carrozzeria");break;
		case 50:strcpy(destText, "Dynamic");break;
		case 51:strcpy(destText, "Nintendo");break;
		case 52:strcpy(destText, "Magifact");break;
		case 53:strcpy(destText, "Hect");break;
		case 54:strcpy(destText, "(unknown: 54)");break;
		case 55:strcpy(destText, "(unknown: 55)");break;
		case 56:strcpy(destText, "Capcom Europe");break;//Capcom? BOF2(E) MM7 (E)
		case 57:strcpy(destText, "Accolade Europe");break;//Accolade?Bubsy 2 (E)
		case 58:strcpy(destText, "(unknown: 58)");break;
		case 59:strcpy(destText, "Arcade Zone");break;//uCON64
		case 60:strcpy(destText, "Empire Software");break;
		case 61:strcpy(destText, "Loriciel");break;
		case 62:strcpy(destText, "Gremlin Graphics"); break; //Acc. ZFE
		case 63:strcpy(destText, "(unknown: 63)");break;
		case 64:strcpy(destText, "Seika Corp.");break;
		case 65:strcpy(destText, "UBI Soft");break;
		case 66:strcpy(destText, "(unknown: 66)");break;
		case 67:strcpy(destText, "(unknown: 67)");break;
		case 68:strcpy(destText, "LifeFitness Exertainment");break;//?? Exertainment Mountain Bike Rally (U).zip
		case 69:strcpy(destText, "(unknown: 69)");break;
		case 70:strcpy(destText, "System 3");break;
		case 71:strcpy(destText, "Spectrum Holobyte");break;
		case 72:strcpy(destText, "(unknown: 72)");break;
		case 73:strcpy(destText, "Irem");break;
		case 74:strcpy(destText, "(unknown: 74)");break;
		case 75:strcpy(destText, "Raya Systems/Sculptured Software");break;
		case 76:strcpy(destText, "Renovation Products");break;
		case 77:strcpy(destText, "Malibu Games/Black Pearl");break;
		case 78:strcpy(destText, "(unknown: 78)");break;
		case 79:strcpy(destText, "U.S. Gold");break;
		case 80:strcpy(destText, "Absolute Entertainment");break;
		case 81:strcpy(destText, "Acclaim");break;
		case 82:strcpy(destText, "Activision");break;
		case 83:strcpy(destText, "American Sammy");break;
		case 84:strcpy(destText, "GameTek");break;
		case 85:strcpy(destText, "Hi Tech Expressions");break;
		case 86:strcpy(destText, "LJN Toys");break;
		case 87:strcpy(destText, "(unknown: 87)");break;
		case 88:strcpy(destText, "(unknown: 88)");break;
		case 89:strcpy(destText, "(unknown: 89)");break;
		case 90:strcpy(destText, "Mindscape");break;
		case 91:strcpy(destText, "Romstar, Inc."); break; //Acc. ZFE
		case 92:strcpy(destText, "(unknown: 92)");break;
		case 93:strcpy(destText, "Tradewest");break;
		case 94:strcpy(destText, "(unknown: 94)");break;
		case 95:strcpy(destText, "American Softworks Corp.");break;
		case 96:strcpy(destText, "Titus");break;
		case 97:strcpy(destText, "Virgin Interactive Entertainment");break;
		case 98:strcpy(destText, "Maxis");break;
		case 99:strcpy(destText, "Origin/FCI/Pony Canyon");break;//uCON64
		case 100:strcpy(destText, "(unknown: 100)");break;
		case 101:strcpy(destText, "(unknown: 101)");break;
		case 102:strcpy(destText, "(unknown: 102)");break;
		case 103:strcpy(destText, "Ocean");break;
		case 104:strcpy(destText, "(unknown: 104)");break;
		case 105:strcpy(destText, "Electronic Arts");break;
		case 106:strcpy(destText, "(unknown: 106)");break;
		case 107:strcpy(destText, "Laser Beam");break;
		case 108:strcpy(destText, "(unknown: 108)");break;
		case 109:strcpy(destText, "(unknown: 109)");break;
		case 110:strcpy(destText, "Elite");break;
		case 111:strcpy(destText, "Electro Brain");break;
		case 112:strcpy(destText, "Infogrames");break;
		case 113:strcpy(destText, "Interplay");break;
		case 114:strcpy(destText, "LucasArts");break;
		case 115:strcpy(destText, "Parker Brothers");break;
		case 116:strcpy(destText, "Konami");break;//uCON64
		case 117:strcpy(destText, "STORM");break;
		case 118:strcpy(destText, "(unknown: 118)");break;
		case 119:strcpy(destText, "(unknown: 119)");break;
		case 120:strcpy(destText, "THQ Software");break;
		case 121:strcpy(destText, "Accolade Inc.");break;
		case 122:strcpy(destText, "Triffix Entertainment");break;
		case 123:strcpy(destText, "(unknown: 123)");break;
		case 124:strcpy(destText, "Microprose");break;
		case 125:strcpy(destText, "(unknown: 125)");break;
		case 126:strcpy(destText, "(unknown: 126)");break;
		case 127:strcpy(destText, "Kemco");break;
		case 128:strcpy(destText, "Misawa");break;
		case 129:strcpy(destText, "Teichio");break;
		case 130:strcpy(destText, "Namco Ltd.");break;
		case 131:strcpy(destText, "Lozc");break;
		case 132:strcpy(destText, "Koei");break;
		case 133:strcpy(destText, "(unknown: 133)");break;
		case 134:strcpy(destText, "Tokuma Shoten Intermedia");break;
		case 135:strcpy(destText, "Tsukuda Original"); break; //Acc. ZFE
		case 136:strcpy(destText, "DATAM-Polystar");break;
		case 137:strcpy(destText, "(unknown: 137)");break;
		case 138:strcpy(destText, "(unknown: 138)");break;
		case 139:strcpy(destText, "Bullet-Proof Software");break;
		case 140:strcpy(destText, "Vic Tokai");break;
		case 141:strcpy(destText, "(unknown: 141)");break;
		case 142:strcpy(destText, "Character Soft");break;
		case 143:strcpy(destText, "I\'\'Max");break;
		case 144:strcpy(destText, "Takara");break;
		case 145:strcpy(destText, "CHUN Soft");break;
		case 146:strcpy(destText, "Video System Co., Ltd.");break;
		case 147:strcpy(destText, "BEC");break;
		case 148:strcpy(destText, "(unknown: 148)");break;
		case 149:strcpy(destText, "Varie");break;
		case 150:strcpy(destText, "Yonezawa / S'Pal Corp."); break; //Acc. ZFE
		case 151:strcpy(destText, "Kaneco");break;
		case 152:strcpy(destText, "(unknown: 152)");break;
		case 153:strcpy(destText, "Pack in Video");break;
		case 154:strcpy(destText, "Nichibutsu");break;
		case 155:strcpy(destText, "TECMO");break;
		case 156:strcpy(destText, "Imagineer Co.");break;
		case 157:strcpy(destText, "(unknown: 157)");break;
		case 158:strcpy(destText, "(unknown: 158)");break;
		case 159:strcpy(destText, "(unknown: 159)");break;
		case 160:strcpy(destText, "Telenet");break;
		case 161:strcpy(destText, "Hori"); break; //Acc. uCON64
		case 162:strcpy(destText, "(unknown: 162)");break;
		case 163:strcpy(destText, "(unknown: 163)");break;
		case 164:strcpy(destText, "Konami");break;
		case 165:strcpy(destText, "K.Amusement Leasing Co.");break;
		case 166:strcpy(destText, "(unknown: 166)");break;
		case 167:strcpy(destText, "Takara");break;
		case 168:strcpy(destText, "(unknown: 168)");break;
		case 169:strcpy(destText, "Technos Jap.");break;
		case 170:strcpy(destText, "JVC");break;
		case 171:strcpy(destText, "(unknown: 171)");break;
		case 172:strcpy(destText, "Toei Animation");break;
		case 173:strcpy(destText, "Toho");break;
		case 174:strcpy(destText, "(unknown: 174)");break;
		case 175:strcpy(destText, "Namco Ltd.");break;
		case 176:strcpy(destText, "Media Rings Corp."); break; //Acc. ZFE
		case 177:strcpy(destText, "ASCII Co. Activison");break;
		case 178:strcpy(destText, "Bandai");break;
		case 179:strcpy(destText, "(unknown: 179)");break;
		case 180:strcpy(destText, "Enix America");break;
		case 181:strcpy(destText, "(unknown: 181)");break;
		case 182:strcpy(destText, "Halken");break;
		case 183:strcpy(destText, "(unknown: 183)");break;
		case 184:strcpy(destText, "(unknown: 184)");break;
		case 185:strcpy(destText, "(unknown: 185)");break;
		case 186:strcpy(destText, "Culture Brain");break;
		case 187:strcpy(destText, "Sunsoft");break;
		case 188:strcpy(destText, "Toshiba EMI");break;
		case 189:strcpy(destText, "Sony Imagesoft");break;
		case 190:strcpy(destText, "(unknown: 190)");break;
		case 191:strcpy(destText, "Sammy");break;
		case 192:strcpy(destText, "Taito");break;
		case 193:strcpy(destText, "(unknown: 193)");break;
		case 194:strcpy(destText, "Kemco");break;
		case 195:strcpy(destText, "Square");break;
		case 196:strcpy(destText, "Tokuma Soft");break;
		case 197:strcpy(destText, "Data East");break;
		case 198:strcpy(destText, "Tonkin House");break;
		case 199:strcpy(destText, "(unknown: 199)");break;
		case 200:strcpy(destText, "KOEI");break;
		case 201:strcpy(destText, "(unknown: 201)");break;
		case 202:strcpy(destText, "Konami USA");break;
		case 203:strcpy(destText, "NTVIC");break;
		case 204:strcpy(destText, "(unknown: 204)");break;
		case 205:strcpy(destText, "Meldac");break;
		case 206:strcpy(destText, "Pony Canyon");break;
		case 207:strcpy(destText, "Sotsu Agency/Sunrise");break;
		case 208:strcpy(destText, "Disco/Taito");break;
		case 209:strcpy(destText, "Sofel");break;
		case 210:strcpy(destText, "Quest Corp.");break;
		case 211:strcpy(destText, "Sigma");break;
		case 212:strcpy(destText, "Ask Kodansha Co., Ltd."); break; //Acc. ZFE
		case 213:strcpy(destText, "(unknown: 213)");break;
		case 214:strcpy(destText, "Naxat");break;
		case 215:strcpy(destText, "(unknown: 215)");break;
		case 216:strcpy(destText, "Capcom Co., Ltd.");break;
		case 217:strcpy(destText, "Banpresto");break;
		case 218:strcpy(destText, "Tomy");break;
		case 219:strcpy(destText, "Acclaim");break;
		case 220:strcpy(destText, "(unknown: 220)");break;
		case 221:strcpy(destText, "NCS");break;
		case 222:strcpy(destText, "Human Entertainment");break;
		case 223:strcpy(destText, "Altron");break;
		case 224:strcpy(destText, "Jaleco");break;
		case 225:strcpy(destText, "(unknown: 225)");break;
		case 226:strcpy(destText, "Yutaka");break;
		case 227:strcpy(destText, "(unknown: 227)");break;
		case 228:strcpy(destText, "T&ESoft");break;
		case 229:strcpy(destText, "EPOCH Co.,Ltd.");break;
		case 230:strcpy(destText, "(unknown: 230)");break;
		case 231:strcpy(destText, "Athena");break;
		case 232:strcpy(destText, "Asmik");break;
		case 233:strcpy(destText, "Natsume");break;
		case 234:strcpy(destText, "King Records");break;
		case 235:strcpy(destText, "Atlus");break;
		case 236:strcpy(destText, "Sony Music Entertainment");break;
		case 237:strcpy(destText, "(unknown: 237)");break;
		case 238:strcpy(destText, "IGS");break;
		case 239:strcpy(destText, "(unknown: 239");break;
		case 240:strcpy(destText, "(unknown: 240)");break;
		case 241:strcpy(destText, "Motown Software");break;
		case 242:strcpy(destText, "Left Field Entertainment");break;
		case 243:strcpy(destText, "Beam Software");break;
		case 244:strcpy(destText, "Tec Magik");break;
		case 245:strcpy(destText, "(unknown: 245)");break;
		case 246:strcpy(destText, "(unknown: 246)");break;
		case 247:strcpy(destText, "(unknown: 247)");break;
		case 248:strcpy(destText, "(unknown: 248)");break;
		case 249:strcpy(destText, "Cybersoft");break;
		case 250:strcpy(destText, "(unknown: 250)");break;
		case 251:strcpy(destText, "Psygnosis"); break; //Acc. ZFE
		case 252:strcpy(destText, "(unknown: 252)");break;
		case 253:strcpy(destText, "(unknown: 253)");break;
		case 254:strcpy(destText, "Davidson"); break; //Acc. uCON64
		case 255:strcpy(destText, "(unknown: 255)");break;
		default:strcpy(destText, "(unknown)");break;
	}
}

/////////////////////////////////////////////
// Function to convert the non standard ROM strings to ANSI strings (only for non-USA versions)
////////// All versions except USA //////////
// à (0xE0) <- (0xE0) | À (0xC0) <- (0xC8)
// è (0xE8) <- (0xE4) | È (0xC8) <- (0xCC)
// ì (0xEC) <- (0xE8) | Ì (0xCC) <- (0xD0)
// ò (0xF2) <- (0xEC) | Ò (0xD2) <- (0xD8)
// ù (0xF9) <- (0xF0) | Ù (0xD9) <- (0xDC)

// á (0xE1) <- (0xE1) | Á (0xC1) <- (0xC9)
// é (0xE9) <- (0xE5) | É (0xC9) <- (0xCD)
// í (0xED) <- (0xE9) | Í (0xCD) <- (0xD1)
// ó (0xF3) <- (0xED) | Ó (0xD3) <- (0xD9)
// ú (0xFA) <- (0xF1) | Ú (0xDA) <- (0xDD)

// â (0xE2) <- (0xE2) | Â (0xC2) <- (0xCA)
// ê (0xEA) <- (0xE6) | Ê (0xCA) <- (0xCE)
// î (0xEE) <- (0xEA) | Î (0xCE) <- (0xD2)
// ô (0xF4) <- (0xEE) | Ô (0xD4) <- (0xDA)
// û (0xFB) <- (0xF2) | Û (0xDB) <- (0xDE)

// ä (0xE4) <- (0xE3) | Ä (0xC4) <- (0xCB)
// ë (0xEB) <- (0xE7) | Ë (0xCB) <- (0xCF)
// ï (0xEF) <- (0xEB) | Ï (0xCF) <- (0xD3)
// ö (0xF6) <- (0xEF) | Ö (0xD6) <- (0xDB)
// ü (0xFC) <- (0xF3) | Ü (0xDC) <- (0xDF)

// ñ (0xF1) <- (0xD7) | Ñ (0xD1) <- (0xD6)
// ç (0xE7) <- (0xD5) | Ç (0xC7) <- (0xD4)
// ¡ (0xA1) <- (0xC0)
// ¿ (0xBF) <- (0xC1)
// ß (0xDF) <- (0xC6)
// ª (0xAA) <- (0xC2)
// º (0xBA) <- (0xC3)
// « (0xAB) <- (0xC4)
// » (0xBB) <- (0xC5)
/////////////////////////////////////////////
void conv_romstr_to_ansistr(char *str) {
	unsigned char norm_conv[256];
	for (int i = 0; i < 256; ++i) {
		norm_conv[i] = i;
	}
	norm_conv[0xE0] = 0xE0;	norm_conv[0xC8] = 0xC0;		// à (0xE0) <- (0xE0) | À (0xC0) <- (0xC8)
	norm_conv[0xE4] = 0xE8;	norm_conv[0xCC] = 0xC8;		// è (0xE8) <- (0xE4) | È (0xC8) <- (0xCC)
	norm_conv[0xE8] = 0xEC;	norm_conv[0xD0] = 0xCC;		// ì (0xEC) <- (0xE8) | Ì (0xCC) <- (0xD0)
	norm_conv[0xEC] = 0xF2;	norm_conv[0xD8] = 0xD2;		// ò (0xF2) <- (0xEC) | Ò (0xD2) <- (0xD8)
	norm_conv[0xF0] = 0xF9;	norm_conv[0xDC] = 0xD9;		// ù (0xF9) <- (0xF0) | Ù (0xD9) <- (0xDC)
	
	norm_conv[0xE1] = 0xE1;	norm_conv[0xC9] = 0xC1;		// á (0xE1) <- (0xE1) | Á (0xC1) <- (0xC9)
	norm_conv[0xE5] = 0xE9;	norm_conv[0xCD] = 0xC9;		// é (0xE9) <- (0xE5) | É (0xC9) <- (0xCD)
	norm_conv[0xE9] = 0xED;	norm_conv[0xD1] = 0xCD;		// í (0xED) <- (0xE9) | Í (0xCD) <- (0xD1)
	norm_conv[0xED] = 0xF3;	norm_conv[0xD9] = 0xD3;		// ó (0xF3) <- (0xED) | Ó (0xD3) <- (0xD9)
	norm_conv[0xF1] = 0xFA;	norm_conv[0xDD] = 0xDA;		// ú (0xFA) <- (0xF1) | Ú (0xDA) <- (0xDD)
	
	norm_conv[0xE2] = 0xE2;	norm_conv[0xCA] = 0xC2;		// â (0xE2) <- (0xE2) | Â (0xC2) <- (0xCA)
	norm_conv[0xE6] = 0xEA;	norm_conv[0xCE] = 0xCA;		// ê (0xEA) <- (0xE6) | Ê (0xCA) <- (0xCE)
	norm_conv[0xEA] = 0xEE;	norm_conv[0xD2] = 0xCE;		// î (0xEE) <- (0xEA) | Î (0xCE) <- (0xD2)
	norm_conv[0xEE] = 0xF4;	norm_conv[0xDA] = 0xD4;		// ô (0xF4) <- (0xEE) | Ô (0xD4) <- (0xDA)
	norm_conv[0xF2] = 0xFB;	norm_conv[0xDE] = 0xDB;		// û (0xFB) <- (0xF2) | Û (0xDB) <- (0xDE)
	
	norm_conv[0xE3] = 0xE4;	norm_conv[0xCB] = 0xC4;		// ä (0xE4) <- (0xE3) | Ä (0xC4) <- (0xCB)
	norm_conv[0xE7] = 0xEB;	norm_conv[0xCF] = 0xCB;		// ë (0xEB) <- (0xE7) | Ë (0xCB) <- (0xCF)
	norm_conv[0xEB] = 0xEF;	norm_conv[0xD3] = 0xCF;		// ï (0xEF) <- (0xEB) | Ï (0xCF) <- (0xD3)
	norm_conv[0xEF] = 0xF6;	norm_conv[0xDB] = 0xD6;		// ö (0xF6) <- (0xEF) | Ö (0xD6) <- (0xDB)
	norm_conv[0xF3] = 0xFC;	norm_conv[0xDF] = 0xDC;		// ü (0xFC) <- (0xF3) | Ü (0xDC) <- (0xDF)
	
	norm_conv[0xD7] = 0xF1;	norm_conv[0xD6] = 0xD1;		// ñ (0xF1) <- (0xD7) | Ñ (0xD1) <- (0xD6)
	norm_conv[0xD5] = 0xE7;	norm_conv[0xD4] = 0xC7;		// ç (0xE7) <- (0xD5) | Ç (0xC7) <- (0xD4)
	norm_conv[0xC0] = 0xA1;								// ¡ (0xA1) <- (0xC0)
	norm_conv[0xC1] = 0xBF;								// ¿ (0xBF) <- (0xC1)
	norm_conv[0xC6] = 0xDF;								// ß (0xDF) <- (0xC6)
	norm_conv[0xC2] = 0xAA;								// ª (0xAA) <- (0xC2)
	norm_conv[0xC3] = 0xBA;								// º (0xBA) <- (0xC3)
	norm_conv[0xC4] = 0xAB;								// « (0xAB) <- (0xC4)
	norm_conv[0xC5] = 0xBB;								// » (0xBB) <- (0xC5)
	
	for (size_t i = 0; i < strlen(str); ++i) {
		unsigned char i_char = *(unsigned char *)(str+i);
		if (norm_conv[i_char] != i_char) {
			str[i] = norm_conv[i_char];
		}
	}
}

// Function to get the description of TargetFlags flag
char *spell_targetflags_setdesc(char *dstStr, unsigned short int targetflags) {
	switch(targetflags) {
		case 0x0008:
			strcpy(dstStr, "Allies: Boy, Dog or multiple Boy & Dog (dog not selectable if dead)");
			break;
		case 0x0800:
			strcpy(dstStr, "Allies: only Boy allowed");
			break;
		case 0x0208:
			strcpy(dstStr, "Allies: only multiple Boy & Dog allowed (don't select him if it's dead)");
			break;
		case 0x0288:
			strcpy(dstStr, "Allies: only multiple Boy & Dog allowed (selects him both alive or dead)");
			break;
		case 0x2010:
			strcpy(dstStr, "Enemies: any enemy single target or multiple");
			break;
		case 0x0210:
			strcpy(dstStr, "Enemies: multiple is forced, all enemies on screen");
			break;
		case 0x0022:
			strcpy(dstStr, "Objects: boulders");
			break;
		case 0x4002:
			strcpy(dstStr, "Objects: hidden paths");
			break;
		case 0x1400:
			strcpy(dstStr, "Allies: only dead Dog allowed");
			break;
		case 0x000A:
			strcpy(dstStr, "Allies: only one target allowed, Boy or Dog");
			break;
		default:
			strcpy(dstStr, "(unknown)");
			break;
	}
	return dstStr;
}

// Function to get the description of InfluenceType flag
char *spell_influencetype_setdesc(char *dstStr, short int spell_influence_type) {
	switch(spell_influence_type) {
		case -1:
			strcpy(dstStr, "Other");
			break;
		case 0:
			strcpy(dstStr, "Direct Attack");
			break;
		case 2:
			strcpy(dstStr, "Healing");
			break;
		default:
			strcpy(dstStr, "(unknown)");
			break;
	}
	return dstStr;
}

// Function to convert a SNES Map memory address to ROM address. Only valid for HiROM memory map. Int version.
int mapaddr_to_romaddr(int addr) {
	if (addr > 0xFFFFFF) {
		return -1;
	} else if (addr >= 0xC00000) {
		return addr - 0xC00000;
	} else if ((addr >= 0x800000) && ((addr & 0x00FFFF) >= 0x8000)) {
		return addr - 0x800000;
	} else if (addr >= 0x7E0000) {
		return -1;
	} else if (addr >= 0x400000) {
		return addr - 0x400000;
	} else if ((addr >= 0x000000) && ((addr & 0x00FFFF) >= 0x8000)) {
		return addr;
	} else if (addr == 0x000000) {
		return 0;
	} else {
		return -1;
	}
}

// Function to convert a ROM address to SNES Map memory address. Only valid for HiROM memory map and designed to return FastROM addresses
int romaddr_to_mapaddr(int addr, int strict) {
	if (strict) {
		if (addr >= 0x400000) {
			return -1;
		} else {
			return addr + 0xC00000;
		}
	} else {
		if (addr > 0xFFFFFF) {
			return -1;
		} else if (addr >= 0xC00000) {
			return addr;
		} else if ((addr >= 0x800000) && ((addr & 0x00FFFF) >= 0x8000)) {
			return addr + 0x400000;
		} else if (addr >= 0x7E0000) {
			return -1;
		} else if (addr >= 0x400000) {
			return addr + 0x800000;
		} else if (addr >= 0x000000) {
			return addr + 0xC00000;
		} else {
			return -1;
		}
	}
}

// Function to calculate CRC32 of bytedata
unsigned int crc32(unsigned char *buf, int len) {
	static unsigned int crc32_table[] = {
		0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
		0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
		0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
		0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
		0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
		0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
		0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
		0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
		0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
		0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
		0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
		0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
		0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
		0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
		0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
		0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
		0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
		0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
		0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
		0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
		0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
		0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
		0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
		0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
		0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
		0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
		0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
		0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
		0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
		0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
		0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
		0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
	};
	unsigned int crc = 0xFFFFFFFF;
	while (len--) {
		crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ *buf) & 255];
		buf++;
	}
	return crc;
}

// Internal helper function to calculate checksum
unsigned short int checksum_calc_sum(unsigned char *data, unsigned int length) {
	unsigned short int sum = 0;
	for (unsigned int i = 0; i < length; i++) {
		sum += data[i];
	}
	return sum;
}

// Internal helper function to calculate checksum
unsigned short int checksum_mirror_sum(unsigned char *start, unsigned int *length, unsigned int mask) {
	while (!((*length) & mask) && mask) {
		mask >>= 1;
	}
	unsigned short int part1 = checksum_calc_sum(start, mask);
	unsigned short int part2 = 0;
	unsigned int next_length = (*length) - mask;
	if (next_length) {
		part2 = checksum_mirror_sum(start + mask, &next_length, mask >> 1);
		while (next_length < mask) {
			next_length += next_length;
			part2 += part2;
		}
		(*length) = mask + mask;
	}
	return (part1 + part2);
}

// Main function to calculate the checksum of byte array
unsigned short int checksum_calculate(unsigned char *data, unsigned int isize) {
	unsigned short int sum = 0;
	if (isize & 0x7fff) {
		sum = checksum_calc_sum(data, isize);
	} else {
		unsigned int length = isize;
		sum = checksum_mirror_sum(data, &length, 0x800000);
	}
	return sum;
}

// Function to generate the hexadecimal string from raw binary data. The 'inst_mask' allows separate bytes with ; instead spaces (used in last byte of 65816 instructions)
char* bin_to_hexstr(char *str, unsigned int bufflen, unsigned char *data, unsigned int datalen, char *inst_mask) {
	if (bufflen < datalen*3) {
		if (bufflen > 0) {
			str[0] = 0;
		}
		return NULL;
	}
	unsigned char * pin = data;
	const char * hex = "0123456789ABCDEF";
	char * pout = str;
	for(unsigned int i = 0; i < datalen-1; ++i){
		*pout++ = hex[(*pin>>4)&0xF];
		*pout++ = hex[(*pin++)&0xF];
		if ((inst_mask == NULL) || (inst_mask[i] == 0)) {
			*pout++ = ' ';
		} else {
			*pout++ = ';';
		}
	}
	*pout++ = hex[(*pin>>4)&0xF];
	*pout++ = hex[(*pin)&0xF];
	*pout = 0;
	return str;
}

// Function that get the 65816 instruction length from the opcode 
int get_instruction_length(unsigned char opcode, int m_flag, int x_flag) {
	int ila[256];
	if (m_flag == 0) {						// [12] Add 1 byte if m=0 (16-bit memory/accumulator)
		ila[0x09] = 3; ila[0x29] = 3; ila[0x49] = 3; ila[0x69] = 3; ila[0x89] = 3; ila[0xA9] = 3; ila[0xC9] = 3; ila[0xE9] = 3;
	} else {
		ila[0x09] = 2; ila[0x29] = 2; ila[0x49] = 2; ila[0x69] = 2; ila[0x89] = 2; ila[0xA9] = 2; ila[0xC9] = 2; ila[0xE9] = 2;
	}
	ila[0x00] = 2; ila[0x02] = 2;			// [13] Opcode is 1 byte, but program counter value pushed onto stack is incremented by 2 allowing for optional signature byte
	if (x_flag == 0) {						// [14] Add 1 byte if x=0 (16-bit index registers)
		ila[0xA0] = 3; ila[0xA2] = 3; ila[0xC0] = 3; ila[0xE0] = 3;
	} else {
		ila[0xA0] = 2; ila[0xA2] = 2; ila[0xC0] = 2; ila[0xE0] = 2;
	}
	ila[0x01] = 2; ila[0x03] = 2;
	ila[0x04] = 2; ila[0x05] = 2; ila[0x06] = 2; ila[0x07] = 2; ila[0x08] = 1; ila[0x0A] = 1; ila[0x0B] = 1; ila[0x0C] = 3;
	ila[0x0D] = 3; ila[0x0E] = 3; ila[0x0F] = 4; ila[0x10] = 2; ila[0x11] = 2; ila[0x12] = 2; ila[0x13] = 2; ila[0x14] = 2;
	ila[0x15] = 2; ila[0x16] = 2; ila[0x17] = 2; ila[0x18] = 1; ila[0x19] = 3; ila[0x1A] = 1; ila[0x1B] = 1; ila[0x1C] = 3;
	ila[0x1D] = 3; ila[0x1E] = 3; ila[0x1F] = 4; ila[0x20] = 3; ila[0x21] = 2; ila[0x22] = 4; ila[0x23] = 2; ila[0x24] = 2;
	ila[0x25] = 2; ila[0x26] = 2; ila[0x27] = 2; ila[0x28] = 1; ila[0x2A] = 1; ila[0x2B] = 1; ila[0x2C] = 3; ila[0x2D] = 3;
	ila[0x2E] = 3; ila[0x2F] = 4; ila[0x30] = 2; ila[0x31] = 2; ila[0x32] = 2; ila[0x33] = 2; ila[0x34] = 2; ila[0x35] = 2;
	ila[0x36] = 2; ila[0x37] = 2; ila[0x38] = 1; ila[0x39] = 3; ila[0x3A] = 1; ila[0x3B] = 1; ila[0x3C] = 3; ila[0x3D] = 3;
	ila[0x3E] = 3; ila[0x3F] = 4; ila[0x40] = 1; ila[0x41] = 2; ila[0x42] = 2; ila[0x43] = 2; ila[0x44] = 3; ila[0x45] = 2;
	ila[0x46] = 2; ila[0x47] = 2; ila[0x48] = 1; ila[0x4A] = 1; ila[0x4B] = 1; ila[0x4C] = 3; ila[0x4D] = 3; ila[0x4E] = 3;
	ila[0x4F] = 4; ila[0x50] = 2; ila[0x51] = 2; ila[0x52] = 2; ila[0x53] = 2; ila[0x54] = 3; ila[0x55] = 2; ila[0x56] = 2;
	ila[0x57] = 2; ila[0x58] = 1; ila[0x59] = 3; ila[0x5A] = 1; ila[0x5B] = 1; ila[0x5C] = 4; ila[0x5D] = 3; ila[0x5E] = 3;
	ila[0x5F] = 4; ila[0x60] = 1; ila[0x61] = 2; ila[0x62] = 3; ila[0x63] = 2; ila[0x64] = 2; ila[0x65] = 2; ila[0x66] = 2;
	ila[0x67] = 2; ila[0x68] = 1; ila[0x6A] = 1; ila[0x6B] = 1; ila[0x6C] = 3; ila[0x6D] = 3; ila[0x6E] = 3; ila[0x6F] = 4;
	ila[0x70] = 2; ila[0x71] = 2; ila[0x72] = 2; ila[0x73] = 2; ila[0x74] = 2; ila[0x75] = 2; ila[0x76] = 2; ila[0x77] = 2;
	ila[0x78] = 1; ila[0x79] = 3; ila[0x7A] = 1; ila[0x7B] = 1; ila[0x7C] = 3; ila[0x7D] = 3; ila[0x7E] = 3; ila[0x7F] = 4;
	ila[0x80] = 2; ila[0x81] = 2; ila[0x82] = 3; ila[0x83] = 2; ila[0x84] = 2; ila[0x85] = 2; ila[0x86] = 2; ila[0x87] = 2;
	ila[0x88] = 1; ila[0x8A] = 1; ila[0x8B] = 1; ila[0x8C] = 3; ila[0x8D] = 3; ila[0x8E] = 3; ila[0x8F] = 4; ila[0x90] = 2;
	ila[0x91] = 2; ila[0x92] = 2; ila[0x93] = 2; ila[0x94] = 2; ila[0x95] = 2; ila[0x96] = 2; ila[0x97] = 2; ila[0x98] = 1;
	ila[0x99] = 3; ila[0x9A] = 1; ila[0x9B] = 1; ila[0x9C] = 3; ila[0x9D] = 3; ila[0x9E] = 3; ila[0x9F] = 4; ila[0xA1] = 2;
	ila[0xA3] = 2; ila[0xA4] = 2; ila[0xA5] = 2; ila[0xA6] = 2; ila[0xA7] = 2; ila[0xA8] = 1; ila[0xAA] = 1; ila[0xAB] = 1;
	ila[0xAC] = 3; ila[0xAD] = 3; ila[0xAE] = 3; ila[0xAF] = 4; ila[0xB0] = 2; ila[0xB1] = 2; ila[0xB2] = 2; ila[0xB3] = 2;
	ila[0xB4] = 2; ila[0xB5] = 2; ila[0xB6] = 2; ila[0xB7] = 2; ila[0xB8] = 1; ila[0xB9] = 3; ila[0xBA] = 1; ila[0xBB] = 1;
	ila[0xBC] = 3; ila[0xBD] = 3; ila[0xBE] = 3; ila[0xBF] = 4; ila[0xC1] = 2; ila[0xC2] = 2; ila[0xC3] = 2; ila[0xC4] = 2;
	ila[0xC5] = 2; ila[0xC6] = 2; ila[0xC7] = 2; ila[0xC8] = 1; ila[0xCA] = 1; ila[0xCB] = 1; ila[0xCC] = 3; ila[0xCD] = 3;
	ila[0xCE] = 3; ila[0xCF] = 4; ila[0xD0] = 2; ila[0xD1] = 2; ila[0xD2] = 2; ila[0xD3] = 2; ila[0xD4] = 2; ila[0xD5] = 2;
	ila[0xD6] = 2; ila[0xD7] = 2; ila[0xD8] = 1; ila[0xD9] = 3; ila[0xDA] = 1; ila[0xDB] = 1; ila[0xDC] = 3; ila[0xDD] = 3;
	ila[0xDE] = 3; ila[0xDF] = 4; ila[0xE1] = 2; ila[0xE2] = 2; ila[0xE3] = 2; ila[0xE4] = 2; ila[0xE5] = 2; ila[0xE6] = 2;
	ila[0xE7] = 2; ila[0xE8] = 1; ila[0xEA] = 1; ila[0xEB] = 1; ila[0xEC] = 3; ila[0xED] = 3; ila[0xEE] = 3; ila[0xEF] = 4;
	ila[0xF0] = 2; ila[0xF1] = 2; ila[0xF2] = 2; ila[0xF3] = 2; ila[0xF4] = 3; ila[0xF5] = 2; ila[0xF6] = 2; ila[0xF7] = 2;
	ila[0xF8] = 1; ila[0xF9] = 3; ila[0xFA] = 1; ila[0xFB] = 1; ila[0xFC] = 3; ila[0xFD] = 3; ila[0xFE] = 3; ila[0xFF] = 4;
	return ila[opcode];
}

// Function to fill the mask with the info of 65816 instructions
void set_inst_mask(char *inst_mask, unsigned char *funcstart, unsigned int funclen) {
	for (int i = 0; i < funclen; i++) {
		inst_mask[i] = 0;
	}
	unsigned char opcode;
	for (int i = 0; i < funclen; i++) {
		opcode = *(funcstart+i);
		i += (get_instruction_length(opcode, 0, 0) - 1);
		inst_mask[i] = 1;
	}
}

// Main function of the program
int main(int argc, char *argv[]) {
	FILE *idf;
	unsigned char *tmp_buff;
	unsigned int idf_size;
	
	char app_filename[MAX_FILENAME_LEN];
	char *app_version = "0.1";
	char fileout_rominfo[MAX_FILENAME_LEN+20];
	char fileout_npcinfo[MAX_FILENAME_LEN+20];
	char fileout_ingredientsinfo[MAX_FILENAME_LEN+20];
	char fileout_alchemyinfo[MAX_FILENAME_LEN+20];
	char fileout_callbeadinfo[MAX_FILENAME_LEN+20];
	char fileout_weaponinfo[MAX_FILENAME_LEN+20];
	char fileout_armorinfo[MAX_FILENAME_LEN+20];
	//char fileout_merchantinfo[MAX_FILENAME_LEN+20];			// TODO: Merchant pending
	char fileout_boydoginfo[MAX_FILENAME_LEN+20];
	//char fileout_miscinfo[MAX_FILENAME_LEN+20];				// TODO: Misc pending
	
	int npcStartPoint = 0xEC536;		// All regions
	int npcEndPoint = 0xEDF84;			// All regions
	
	int ingredientStartPoint = 0x45B10;	// USA; Europe & Germany = 0x50176; France & Spain = 0x40176
	int ingredientEndPoint = 0x45B3B;	// USA; Europe & Germany = 0x501A1; France & Spain = 0x401A1
	
	int alchemyStartPoint = 0x45C3B;	// USA; Europe & Germany = 0x502A1; France & Spain = 0x402A1
	int alchemyEndPoint = 0x45C80;		// USA; Europe & Germany = 0x502E6; France & Spain = 0x402E6
	
	int callbeadStartPoint = 0x45EF7;	// USA; Europe & Germany = 0x5055D; France & Spain = 0x4055D
	int callbeadEndPoint = 0x45F16;		// USA; Europe & Germany = 0x5057C; France & Spain = 0x4057C
	
	int weaponStartPoint = 0x43876;		// USA; Europe & Germany = 0x43EB6; France & Spain = 0x33EB6
	int weaponEndPoint = 0x43893;		// USA; Europe & Germany = 0x43ED3; France & Spain = 0x33ED3
	int weaponStartPoint2 = 0x459A6;	// USA; Europe & Germany = 0x5000C; France & Spain = 0x50025
	int weaponEndPoint2 = 0x459BF;		// USA; Europe & Germany = 0x4000C; France & Spain = 0x40025
	
	int armorStartPoint = 0x43896;		// USA; Europe & Germany = 0x43ED6; France & Spain = 0x33ED6
	int armorEndPoint = 0x438E5;		// USA; Europe & Germany = 0x43F25; France & Spain = 0x33F25
	int armorStartPoint2 = 0x45AC0;		// USA; Europe & Germany = 0x50126; France & Spain = 0x40126
	int armorEndPoint2 = 0x45B0F;		// USA; Europe & Germany = 0x50175; France & Spain = 0x40175
	
	int boyInfoStartPoint = 0xC8BD0;	// USA; Europe & Germany & France & Spain = 0xC8F65
	int dogInfoStartPoint = 0xC8CFC;	// USA; Europe & Germany & France & Spain = 0xC9091
	
	strcpy(app_filename, (argc > 0) ? argv[0] : "");
	remove_path(app_filename);
	
	{
#if (defined(_WIN32) || defined(_WIN64)) && (!defined(__MINGW32__) || defined(__MINGW64__))
		long int pid = GetCurrentProcessId();
#else
		long int pid = getpid();
#endif
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		srand((ts.tv_nsec+1) * (ts.tv_sec+1) * (pid+1));
	}
	
	param_options_type param_opt;
	param_opt.type = 0;				// Default --type parameter: 0 (autodetect)
	param_opt.hsize = 512;			// Default --hsize parameter: 512 
	param_opt.spacer = ',';			// Default --spacer parameter: ,
	
	int type = param_opt.type;
	
	// Read parameters
	if (argc < 2) {
		showmenu(app_filename, app_version);
		return 1;
	}
	if ((strcmp(argv[1],"-?")==0) || (strcmp(argv[1],"--help")==0)) {
		showmenu(app_filename, app_version);
		return 0;
	}
	int iparam = 1;
	for(iparam=1; iparam<(argc-1); iparam++) {
		if ((strcmp(argv[iparam],"-?")==0) || (strcmp(argv[iparam],"--help")==0)) {
			showmenu(app_filename, app_version);
			return 0;
		} else if ((strcmp(argv[iparam],"-t")==0) || (strcmp(argv[iparam],"--type")==0)) {
			if (++iparam < (argc-1)) {
				if ((strcmp(argv[iparam],"auto")==0) || (strcmp(argv[iparam],"autodetect")==0) || (strcmp(argv[iparam],"0")==0)) {
					param_opt.type = 0;
					type = param_opt.type;
				} else if ((strcmp(argv[iparam],"unheader")==0) || (strcmp(argv[iparam],"1")==0)) {
					param_opt.type = 1;
					type = param_opt.type;
				} else if ((strcmp(argv[iparam],"header")==0) || (strcmp(argv[iparam],"2")==0)) {
					param_opt.type = 2;
					type = param_opt.type;
				} else {
					printf("[!] Error: wrong -t or --type parameter\n");
					return -2;
				}
			} else {
				printf("[!] Error: wrong number of parameters\n");
				return -2;
			}
		} else if ((strcmp(argv[iparam],"-h")==0) || (strcmp(argv[iparam],"--hsize")==0)) {
			if (++iparam < (argc-1)) {
				long int tmp = strtol(argv[iparam], NULL, 10);
				if (tmp > 0) {
					param_opt.hsize = (unsigned int) tmp;
				} else {
					printf("[!] Error: wrong -h or --hsize parameter\n");
					return -2;
				}
			} else {
				printf("[!] Error: wrong number of parameters\n");
				return -2;
			}
		} else if ((strcmp(argv[iparam],"-s")==0) || (strcmp(argv[iparam],"--spacer")==0)) {
			if (++iparam < (argc-1)) {
				param_opt.spacer = argv[iparam][0];
			} else {
				printf("[!] Error: wrong number of parameters\n");
				return -2;
			}
		} else {
			if (iparam < (argc-2)) {
				printf("[!] Error: unrecognized parameter '%s'\n",argv[iparam]);
				return -2;
			} else {
				break;
			}
		}
	}
	if (strlen(argv[iparam]) < MAX_FILENAME_LEN) {
		strcpy(param_opt.filein, argv[iparam]);
	} else {
		printf("[!] Error: filename too long\n");
		return -3;
	}
	++iparam;
	if (iparam < argc) {
		if (strlen(argv[iparam]) < MAX_FILENAME_LEN) {
			strcpy(param_opt.fileout, argv[iparam]);
		} else {
			printf("[!] Error: filename too long\n");
			return -3;
		}
		remove_ext(param_opt.fileout, '.');
	} else {
		char tempInputFilename[MAX_STR_LEN];
		strcpy(tempInputFilename, param_opt.filein);
		remove_ext(tempInputFilename, '.');
		remove_path(tempInputFilename);
		for (size_t i = 0; i < strlen(tempInputFilename); i++) {
			if (tempInputFilename[i] == ' ') {
				tempInputFilename[i] = '_';
			} else if (tempInputFilename[i] == '"') {
				tempInputFilename[i] = '_';
			}
		}
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		time_t t = ts.tv_sec;
		struct tm *tmvp;
		tmvp = localtime(&t);
		sprintf(param_opt.fileout, "rominforesult_%s_%02d%02d%02d_%02d%02d%02d_%05d", ((strlen(tempInputFilename)+27u < MAX_FILENAME_LEN) ? tempInputFilename : "res"), tmvp->tm_year%100, tmvp->tm_mon+1, tmvp->tm_mday, tmvp->tm_hour, tmvp->tm_min, tmvp->tm_sec, rand()%90000+10000);
		printf("autoNameOut: %s\n", param_opt.fileout);
	}
	strcpy(fileout_rominfo, param_opt.fileout);
	strcpy(fileout_npcinfo, param_opt.fileout);
	strcpy(fileout_ingredientsinfo, param_opt.fileout);
	strcpy(fileout_alchemyinfo, param_opt.fileout);
	strcpy(fileout_callbeadinfo, param_opt.fileout);
	strcpy(fileout_weaponinfo, param_opt.fileout);
	strcpy(fileout_armorinfo, param_opt.fileout);
	//strcpy(fileout_merchantinfo, param_opt.fileout);			// TODO: Merchant pending
	strcpy(fileout_boydoginfo, param_opt.fileout);
	//strcpy(fileout_miscinfo, param_opt.fileout);				// TODO: Misc pending
	strcat(fileout_rominfo, "_rominfo.csv");
	strcat(fileout_npcinfo, "_npcinfo.csv");
	strcat(fileout_ingredientsinfo, "_ingredientsinfo.csv");
	strcat(fileout_alchemyinfo, "_alchemyinfo.csv");
	strcat(fileout_callbeadinfo, "_callbeadinfo.csv");
	strcat(fileout_weaponinfo, "_weaponinfo.csv");
	strcat(fileout_armorinfo, "_armorinfo.csv");
	//strcat(fileout_merchantinfo, "_merchantinfo.csv");		// TODO: Merchant pending
	strcat(fileout_boydoginfo, "_boydoginfo.csv");
	//strcat(fileout_miscinfo, "_miscinfo.csv");				// TODO: Misc pending
	
	printf("[+] Start ROM Info extraction from SoE ROM\n\n");
	
	// Open input file and read to buffer
	if ((idf=fopen(param_opt.filein, "rb"))==NULL) {
		printf("[!] Error: unable to open the input file \"%s\"\n",param_opt.filein);
		return -3;
	}
	fseek(idf,0,SEEK_END);
	idf_size = (unsigned int) ftell(idf);
	fseek(idf,0,SEEK_SET);
	
	if (idf_size <= 0) {
		printf("[!] Error: the input file is empty or can't be readed\n");
		fclose(idf);
		return -3;
	}
	
	if ((tmp_buff = (unsigned char *) malloc((idf_size+1)*sizeof(unsigned char)))==NULL) {
		printf("[!] Error: not enough memory\n");
		fclose(idf);
		return -4;
	}
	size_t fres = fread(tmp_buff, idf_size, 1, idf);
	if (fres != 1) {
		printf("[!] Error: error reading input file \n");
		fclose(idf);
		return -3;
	}
	fclose(idf);
	
	// Check ROM size
	if (idf_size < (0x2DCEE5 + ((type == 1) ? 0 : param_opt.hsize))) {
		printf("[!] Error: the input file is not a valid ROM file\n");
		return -10;
	}
	
	// Check if its a Headered or Unheadered ROM
	if (type == 0) {
		if ((idf_size % 0x8000) == 0) {
			type = 1;
		} else if ((idf_size % 0x8000) == param_opt.hsize) {
			type = 2;
			npcStartPoint += param_opt.hsize;
			npcEndPoint += param_opt.hsize;
			ingredientStartPoint += param_opt.hsize;
			ingredientEndPoint += param_opt.hsize;
			alchemyStartPoint += param_opt.hsize;
			alchemyEndPoint += param_opt.hsize;
			callbeadStartPoint += param_opt.hsize;
			callbeadEndPoint += param_opt.hsize;
			weaponStartPoint += param_opt.hsize;
			weaponEndPoint += param_opt.hsize;
			weaponStartPoint2 += param_opt.hsize;
			weaponEndPoint2 += param_opt.hsize;
			armorStartPoint += param_opt.hsize;
			armorEndPoint += param_opt.hsize;
			armorStartPoint2 += param_opt.hsize;
			armorEndPoint2 += param_opt.hsize;
			boyInfoStartPoint += param_opt.hsize;
			dogInfoStartPoint += param_opt.hsize;
		} else {
			printf("[!] Error: unable to determine if ROM has header or not\n");
			return -6;
		}
	}
	
	// [[Start main procedure]]: read data -> parse data -> write data
	////////////////////////////////////////////////////////////////////////////////////
	// [ROM Info] Section
	rom_info_type rom_info;
	{
		// [ROM Info] Read data from ROM
		unsigned int i = 0 + (type == 2 ? param_opt.hsize : 0);
		strcpy(rom_info.filename, param_opt.filein);
		remove_path(rom_info.filename);
		rom_info.header_present = (type == 2);
		rom_info.header_size = (type == 2 ? param_opt.hsize : 0);
		rom_info.maker_code = *(unsigned short int *)(tmp_buff+i+0x00FFB0);
		rom_info.maker_code_str[0] = *(char *)(tmp_buff+i+0x00FFB0);
		rom_info.maker_code_str[1] = *(char *)(tmp_buff+i+0x00FFB1);
		rom_info.maker_code_str[2] = 0;
		int l = toupper(*(char *)(tmp_buff+i+0x00FFB0));
		int r = toupper(*(char *)(tmp_buff+i+0x00FFB1));
		int l2 = (l > '9') ? l - '7' : l - '0';
		int r2 = (r > '9') ? r - '7' : r - '0';
		rom_info.companyId = l2 * 36 + r2;
		companyIdStr(rom_info.companyId, rom_info.company_str);
		rom_info.game_code = *(unsigned int *)(tmp_buff+i+0x00FFB2);
		rom_info.game_code_str[0] = *(char *)(tmp_buff+i+0x00FFB2);
		rom_info.game_code_str[1] = *(char *)(tmp_buff+i+0x00FFB2+1);
		rom_info.game_code_str[2] = *(char *)(tmp_buff+i+0x00FFB2+2);
		rom_info.game_code_str[3] = *(char *)(tmp_buff+i+0x00FFB2+3);
		rom_info.game_code_str[4] = 0;
		rom_info.valid_fixed_values = 1;
		for (int j = 0; j < 7; j++) {
			if (*(char *)(tmp_buff+i+0x00FFB6+j) != 0x00) {
				rom_info.valid_fixed_values = 0;
				break;
			}
		}
		if (*(char *)(tmp_buff+i+0x00FFDA) != 0x33) {
			rom_info.valid_fixed_values = 0;
		}
		rom_info.expansion_RAM_size_code = *(unsigned char *)(tmp_buff+i+0x00FFBD);
		switch(rom_info.expansion_RAM_size_code) {
			case 0x00:
				rom_info.expansion_RAM_size = 0;
				break;
			case 0x01:
				rom_info.expansion_RAM_size = 16;
				break;
			case 0x03:
				rom_info.expansion_RAM_size = 64;
				break;
			case 0x05:
				rom_info.expansion_RAM_size = 256;
				break;
			case 0x06:
				rom_info.expansion_RAM_size = 512;
				break;
			case 0x07:
				rom_info.expansion_RAM_size = 1024;
				break;
			default:
				rom_info.expansion_RAM_size = 0;
				break;
		}
		rom_info.special_version = *(unsigned char *)(tmp_buff+i+0x00FFBE);
		rom_info.cartridge_type = *(unsigned char *)(tmp_buff+i+0x00FFBF);
		memcpy(rom_info.rom_name, (char *)(tmp_buff+i+0x00FFC0), 21);
		rom_info.rom_name[21] = 0;
		for (int i = 20; i >= 0; i--) {
			if (rom_info.rom_name[i] == ' ') {
				rom_info.rom_name[i] = 0;
			} else {
				break;
			}
		}
		rom_info.map_mode = *(unsigned char *)(tmp_buff+i+0x00FFD5);
		if (rom_info.map_mode == 0x20) {
			strcpy(rom_info.map_mode_str, "LoROM");
		} else if (rom_info.map_mode == 0x21) {
			strcpy(rom_info.map_mode_str, "HiROM");
		} else if (rom_info.map_mode == 0x23) {
			strcpy(rom_info.map_mode_str, "SA-1 ROM");
		} else if (rom_info.map_mode == 0x30) {
			strcpy(rom_info.map_mode_str, "LoROM + FastROM");
		} else if (rom_info.map_mode == 0x31) {
			strcpy(rom_info.map_mode_str, "HiROM + FastROM");
		} else if (rom_info.map_mode == 0x32) {
			strcpy(rom_info.map_mode_str, "ExLoROM");
		} else if (rom_info.map_mode == 0x35) {
			strcpy(rom_info.map_mode_str, "ExHiROM");
		} else {
			strcpy(rom_info.map_mode_str, "(unknown)");
		}
		rom_info.type = *(unsigned char *)(tmp_buff+i+0x00FFD6);
		if ((rom_info.type & 0x0F) == 0x00) {
			strcpy(rom_info.type_str, "ROM Only");
		} else if ((rom_info.type & 0x0F) == 0x01) {
			strcpy(rom_info.type_str, "ROM + RAM");
		} else if ((rom_info.type & 0x0F) == 0x02) {
			strcpy(rom_info.type_str, "ROM + RAM + SRAM");
		} else {
			if ((rom_info.type & 0x0F) == 0x03) {
				strcpy(rom_info.type_str, "ROM");
			} else if ((rom_info.type & 0x0F) == 0x04) {
				strcpy(rom_info.type_str, "ROM + RAM");
			} else if ((rom_info.type & 0x0F) == 0x05) {
				strcpy(rom_info.type_str, "ROM + RAM + SRAM");
			} else if ((rom_info.type & 0x0F) == 0x06) {
				strcpy(rom_info.type_str, "ROM + SRAM");
			} else {
				strcpy(rom_info.type_str, "(unknown)");
			}
			if ((rom_info.type & 0xF0) == 0x00) {
				strcat(rom_info.type_str, " + DSP");
			} else if ((rom_info.type & 0xF0) == 0x10) {
				strcat(rom_info.type_str, " + SuperFX");
			} else if ((rom_info.type & 0xF0) == 0x20) {
				strcat(rom_info.type_str, " + OBC1");
			} else if ((rom_info.type & 0xF0) == 0x30) {
				strcat(rom_info.type_str, " + SA-1");
			} else if ((rom_info.type & 0xF0) == 0x40) {
				strcat(rom_info.type_str, " + SD01");
			} else if ((rom_info.type & 0xF0) == 0x50) {
				strcat(rom_info.type_str, " + SRTC");
			} else if ((rom_info.type & 0xF0) == 0xE0) {
				strcat(rom_info.type_str, " + OtherChip");
			} else if ((rom_info.type & 0xF0) == 0xF0) {
				strcat(rom_info.type_str, " + CustomChip");
			} else {
				strcat(rom_info.type_str, " + (unknown)");
			}
		}
		rom_info.region = *(unsigned char *)(tmp_buff+i+0x00FFD9);
		switch(rom_info.region) {
			char temp[MAX_STR_LEN];
			case 0x00:
				strcpy(rom_info.region_str, "Japan");
				strcpy(rom_info.video_type, "NTSC");
				break;
			case 0x01:
				strcpy(rom_info.region_str, "USA/Canada");
				strcpy(rom_info.video_type, "NTSC");
				break;
			case 0x02:
				strcpy(rom_info.region_str, "Europe, Asia and Oceania");
				strcpy(rom_info.video_type, "PAL");
				break;
			case 0x03:
				strcpy(rom_info.region_str, "Sweden");
				strcpy(rom_info.video_type, "PAL");
				break;
			case 0x04:
				strcpy(rom_info.region_str, "Finland");
				strcpy(rom_info.video_type, "PAL");
				break;
			case 0x05:
				strcpy(rom_info.region_str, "Denmark");
				strcpy(rom_info.video_type, "PAL");
				break;
			case 0x06:
				strcpy(rom_info.region_str, "France");
				strcpy(rom_info.video_type, "SECAM");
				break;
			case 0x07:
				strcpy(rom_info.region_str, "Holland");
				strcpy(rom_info.video_type, "PAL");
				break;
			case 0x08:
				strcpy(rom_info.region_str, "Spain");
				strcpy(rom_info.video_type, "PAL");
				break;
			case 0x09:
				strcpy(rom_info.region_str, "Germany, Austria, and Switzerland");
				strcpy(rom_info.video_type, "PAL");
				break;
			case 0x0A:
				strcpy(rom_info.region_str, "Italy");
				strcpy(rom_info.video_type, "PAL");
				break;
			case 0x0B:
				strcpy(rom_info.region_str, "China and Hong Kong");
				strcpy(rom_info.video_type, "PAL");
				break;
			case 0x0C:
				strcpy(rom_info.region_str, "Indonesia");
				strcpy(rom_info.video_type, "PAL");
				break;
			case 0x0D:
				strcpy(rom_info.region_str, "South Korea");
				strcpy(rom_info.video_type, "NTSC");
				break;
			case 0x0E:
				strcpy(rom_info.region_str, "Global");
				strcpy(rom_info.video_type, "(unknown)");
				break;
			case 0x0F:
				strcpy(rom_info.region_str, "Canada");
				strcpy(rom_info.video_type, "NTSC");
				break;
			case 0x10:
				strcpy(rom_info.region_str, "Brazil");
				strcpy(rom_info.video_type, "PAL-M");
				break;
			case 0x11:
				strcpy(rom_info.region_str, "Australia");
				strcpy(rom_info.video_type, "PAL");
				break;
			default:
				sprintf(temp, "Unknown region %d", rom_info.region);
				strcpy(rom_info.region_str, temp);
				strcpy(rom_info.video_type, "(unknown)");
				break;
		}
		rom_info.version = *(char *)(tmp_buff+i+0x00FFDB);
		rom_info.rom_size_code = *(unsigned char *)(tmp_buff+i+0x00FFD7);
		rom_info.rom_size = (1 << rom_info.rom_size_code) * 8;
		rom_info.calculated_rom_size = idf_size - i;
		rom_info.sram_size_code = *(unsigned char *)(tmp_buff+i+0x00FFD8);
		rom_info.sram_size = (1 << rom_info.sram_size_code) * 8;
		rom_info.rom_checksum = *(unsigned short int *)(tmp_buff+i+0x00FFDE);
		rom_info.rom_complement_check = *(unsigned short int *)(tmp_buff+i+0x00FFDC);
		rom_info.valid_checksum_and_complement = ((rom_info.rom_checksum ^ rom_info.rom_complement_check) == 0xFFFFu);
		rom_info.real_checksum = checksum_calculate((unsigned char *)(tmp_buff+i), idf_size - i);
		rom_info.real_complement_check = rom_info.real_checksum ^ 0xFFFF;
		rom_info.coincident_checksum_and_complement = ((rom_info.rom_checksum == rom_info.real_checksum) && (rom_info.rom_complement_check == rom_info.real_complement_check));
		// [ROM Info] Write to file
		if ((idf=fopen(fileout_rominfo, "wb"))==NULL) {
			printf("[!] Error: unable to open the output file \"%s\"\n", fileout_rominfo);
			return -3;
		}
		char buff_line[MAX_BUFF_SIZE];
		snprintf(buff_line, MAX_BUFF_SIZE, "Filename%cHeader Present%cHeader Size%cValid fixed values%cROM Checksum%cROM Complement Check%cValid ROM Checksum & Complement%cROM Real Checksum%cROM Real Complement%cCoincident Real & ROM Checksums%cROM Name%cVersion%cRegion%cRegion Desc%cVideo Type%cMap Mode%cMap Mode Desc%cType Code%cType Code Desc%cMaker Code%cMaker Code Desc%cCompanyId%cCompanyId Desc%cGame Code%cGame Code Desc%cROM Size Code%cROM Size (Mbit)%cCalculated ROM Size (Mbit)%cSRAM Size Code%cSRAM Size (Kbit)%cExpansion RAM Size Code%cExpansion RAM Size (Kbit)%cSpecial Version%cCartridge Type\n", param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		snprintf(buff_line, MAX_BUFF_SIZE, "\"%s\"%c%u%c%u%c%u%c0x%04X%c0x%04X%c%u%c0x%04X%c0x%04X%c%u%c\"%s\"%c\"1.%u\"%c0x%02X%c\"%s\"%c\"%s\"%c0x%02X%c\"%s\"%c0x%02X%c\"%s\"%c0x%04X%c\"%s\"%c%u%c\"%s\"%c0x%08X%c\"%s\"%c0x%02X%c%u%c%u%c0x%02X%c%u%c0x%02X%c%u%c0x%02X%c0x%02X\n", rom_info.filename, param_opt.spacer, rom_info.header_present, param_opt.spacer, rom_info.header_size, param_opt.spacer, rom_info.valid_fixed_values, param_opt.spacer, rom_info.rom_checksum, param_opt.spacer, rom_info.rom_complement_check, param_opt.spacer, rom_info.valid_checksum_and_complement, param_opt.spacer, rom_info.real_checksum, param_opt.spacer, rom_info.real_complement_check, param_opt.spacer, rom_info.coincident_checksum_and_complement, param_opt.spacer, rom_info.rom_name, param_opt.spacer, rom_info.version, param_opt.spacer, rom_info.region, param_opt.spacer, rom_info.region_str, param_opt.spacer, rom_info.video_type, param_opt.spacer, rom_info.map_mode, param_opt.spacer, rom_info.map_mode_str, param_opt.spacer, rom_info.type, param_opt.spacer, rom_info.type_str, param_opt.spacer, rom_info.maker_code, param_opt.spacer, rom_info.maker_code_str, param_opt.spacer, rom_info.companyId, param_opt.spacer, rom_info.company_str, param_opt.spacer, rom_info.game_code, param_opt.spacer, rom_info.game_code_str, param_opt.spacer, rom_info.rom_size_code, param_opt.spacer, (rom_info.rom_size / 1024u), param_opt.spacer, (rom_info.calculated_rom_size * 8u / 1024u / 1024u), param_opt.spacer, rom_info.sram_size_code, param_opt.spacer, rom_info.sram_size, param_opt.spacer, rom_info.expansion_RAM_size_code, param_opt.spacer, rom_info.expansion_RAM_size, param_opt.spacer, rom_info.special_version, param_opt.spacer, rom_info.cartridge_type);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		fclose(idf);
	}
	printf("\n");
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// [NPC Info] Section
	unsigned int npc_count = (npcEndPoint-1-npcStartPoint)/0x4A+1;	//usually 0x5B, 91
	npc_stats_type npc_stats[npc_count];
	// [NPC Info] Read data from ROM
	{
		unsigned int i = npcStartPoint;
		unsigned int j = 0;
		while(i < npcEndPoint) {
			npc_stats[j].index = j;
			npc_stats[j].name_ptr = (unsigned char)*(tmp_buff+i+0x00) + ((unsigned char)*(tmp_buff+i+0x01) << 8) + ((unsigned char)*(tmp_buff+i+0x02) << 16);
			int name_txt_ptr = mapaddr_to_romaddr(npc_stats[j].name_ptr);
			if ((name_txt_ptr < 0) || (name_txt_ptr > idf_size)) {
				strcpy(npc_stats[j].name, "(invalid_ptr)");
			} else if (name_txt_ptr == 0) {
				strcpy(npc_stats[j].name, "(unnamed)");
			} else {
				strcpy(npc_stats[j].name, (char *)(tmp_buff+name_txt_ptr+(type == 2 ? param_opt.hsize : 0)));
				if (rom_info.region != 0x01) {
					conv_romstr_to_ansistr(npc_stats[j].name);
				}
			}
			npc_stats[j].hp = *(short int *)(tmp_buff+i+0x0F);
			npc_stats[j].attack = *(short int *)(tmp_buff+i+0x19);
			npc_stats[j].defense = *(short int *)(tmp_buff+i+0x1B);
			npc_stats[j].magicdef = *(short int *)(tmp_buff+i+0x1D);
			npc_stats[j].evade = *(short int *)(tmp_buff+i+0x1F);
			npc_stats[j].hit = *(short int *)(tmp_buff+i+0x21);
			npc_stats[j].aggression_range = *(short int *)(tmp_buff+i+0x13);
			npc_stats[j].aggression_chance_code = *(unsigned short int *)(tmp_buff+i+0x15);
			npc_stats[j].aggression_chance = npc_stats[j].aggression_chance_code / 256.0 * 100.0;
			npc_stats[j].unique_trait_flags = *(unsigned short int *)(tmp_buff+i+0x07);
			char firstFlagStr = 1;
			if (npc_stats[j].unique_trait_flags & 0x0001) {
				if (firstFlagStr) {
					strcpy(npc_stats[j].unique_trait_flags_str, "(FLAG01_TARSKULL_SALABOG)");
					firstFlagStr = 0;
				} else {
					strcat(npc_stats[j].unique_trait_flags_str, "+ (FLAG01_TARSKULL_SALABOG)");
				}
			}
			if (npc_stats[j].unique_trait_flags & 0x0002) {
				if (firstFlagStr) {
					strcpy(npc_stats[j].unique_trait_flags_str, "(FLAG_MINUS30_HIT_PENALTY_VS_AI_PLAYER_CHARACTER)");
					firstFlagStr = 0;
				} else {
					strcat(npc_stats[j].unique_trait_flags_str, "+ (FLAG_MINUS30_HIT_PENALTY_VS_AI_PLAYER_CHARACTER)");
				}
			}
			if (npc_stats[j].unique_trait_flags & 0x0004) {
				if (firstFlagStr) {
					strcpy(npc_stats[j].unique_trait_flags_str, "(FLAG03_ROCK)");
					firstFlagStr = 0;
				} else {
					strcat(npc_stats[j].unique_trait_flags_str, "+ (FLAG03_ROCK)");
				}
			}
			if (npc_stats[j].unique_trait_flags & 0x0008) {
				if (firstFlagStr) {
					strcpy(npc_stats[j].unique_trait_flags_str, "(FLAG04_BRIDGE)");
					firstFlagStr = 0;
				} else {
					strcat(npc_stats[j].unique_trait_flags_str, "+ (FLAG04_BRIDGE)");
				}
			}
			if (npc_stats[j].unique_trait_flags & 0x0010) {
				if (firstFlagStr) {
					strcpy(npc_stats[j].unique_trait_flags_str, "(FLAG_PROJECTILE_ATTACKS_WONT_HIT)");
					firstFlagStr = 0;
				} else {
					strcat(npc_stats[j].unique_trait_flags_str, "+ (FLAG_PROJECTILE_ATTACKS_WONT_HIT)");
				}
			}
			if (npc_stats[j].unique_trait_flags & 0xFFE0) {
				if (firstFlagStr) {
					strcpy(npc_stats[j].unique_trait_flags_str, "(UNKNOWN_FLAGS)");
					firstFlagStr = 0;
				} else {
					strcat(npc_stats[j].unique_trait_flags_str, "+ (UNKNOWN_FLAGS)");
				}
			}
			if (firstFlagStr) {
				strcpy(npc_stats[j].unique_trait_flags_str, "NONE");
			}
			npc_stats[j].attack_properties = *(unsigned short int *)(tmp_buff+i+0x30);
			switch(npc_stats[j].attack_properties) {
				case 0:
					strcpy(npc_stats[j].attack_properties_str, "Normal");
					break;
				case 12:
					strcpy(npc_stats[j].attack_properties_str, "If hits, don't do normal damage, but grab and do a few rounds of periodic damage");
					break;
				case 14:
					strcpy(npc_stats[j].attack_properties_str, "Don't attempt attack if Insect Incense possessed");
					break;
				case 18:
					strcpy(npc_stats[j].attack_properties_str, "If hits, 1/4 chance of inflicting Poison");
					break;
				case 20:
					strcpy(npc_stats[j].attack_properties_str, "Don't attempt attack if Insect Incense possessed + If hits, 1/4 chance of inflicting Poison");
					break;
				case 22:
					strcpy(npc_stats[j].attack_properties_str, "If hits, 1/8 chance of inflicting Plague");
					break;
				case 24:
					strcpy(npc_stats[j].attack_properties_str, "If hits, inflicts Poison");
					break;
				case 28:
					strcpy(npc_stats[j].attack_properties_str, "If hits, 1/8 chance of inflicting Plague, 1/8 chance of inflicting Confound, 3/4 chance of inflicting nothing");
					break;
				default:
					strcpy(npc_stats[j].attack_properties_str, "(unknown)");
					break;
			}
			npc_stats[j].experience = *(int *)(tmp_buff+i+0x23);
			npc_stats[j].money = *(short int *)(tmp_buff+i+0x27);
			npc_stats[j].chance_prize_drop_code = *(unsigned char *)(tmp_buff+i+0x29);
			npc_stats[j].chance_prize_drop = npc_stats[j].chance_prize_drop_code / 128.0 * 100.0;
			npc_stats[j].chargeup_limit = *(short int *)(tmp_buff+i+0x2C);
			npc_stats[j].chargeup_gain_per_frame = *(short int *)(tmp_buff+i+0x2E);
			npc_stats[j].unknown_maybe_collision_related = *(short int *)(tmp_buff+i+0x0D);
			npc_stats[j].palette = *(unsigned short int *)(tmp_buff+i+0x09);
			npc_stats[j].death_script_nospoils = *(unsigned short int *)(tmp_buff+i+0x42);
			npc_stats[j].death_script_spoils = *(unsigned short int *)(tmp_buff+i+0x44);
			npc_stats[j].block_script = *(unsigned short int *)(tmp_buff+i+0x48);
			npc_stats[j].stat_ptr = i - (type == 2 ? param_opt.hsize : 0);
			npc_stats[j].stat_ptr = romaddr_to_mapaddr(npc_stats[j].stat_ptr, 0);
			i += 0x4A;
			j++;
		}
		// [NPC Info] Write to file
		if ((idf=fopen(fileout_npcinfo, "wb"))==NULL) {
			printf("[!] Error: unable to open the output file \"%s\"\n", fileout_npcinfo);
			return -3;
		}
		char buff_line[MAX_BUFF_SIZE];
		snprintf(buff_line, MAX_BUFF_SIZE, "Index%cNPC Name%cHP%cAttack%cDefense%cMagic Defense%cEvade%cHit%cAggresion Range%cAggresion Chance Code%cAggresion Chance (%%)%cUnique Trait Flags%cUnique Trait Flags Desc%cAttack Properties%cAttack Properties Desc%cExperience%cMoney%cChanze Prize Drop Code%cChanze Prize Drop (%%)%cChargeup limit%cChargeup gain per frame%cUnknown maybe collision related%cPalette%cDeath Script - NoSpoils%cDeath Script - Spoils%cBlock Script%c[PTR] NPC Stats%c[PTR] NPC Name\n", param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		for(unsigned int j = 0; j < npc_count; j++) {
			double aggression_chance_std = round(npc_stats[j].aggression_chance*10000)/10000;
			double chance_prize_drop_std = round(npc_stats[j].chance_prize_drop*10000)/10000;
			snprintf(buff_line, MAX_BUFF_SIZE, "%d%c\"%s\"%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c0x%04X%c%.04f%c%d%c\"%s\"%c%d%c\"%s\"%c%d%c%d%c0x%02X%c%.04f%c%d%c%d%c%d%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%06X%c0x%06X\n", npc_stats[j].index, param_opt.spacer, npc_stats[j].name, param_opt.spacer, npc_stats[j].hp, param_opt.spacer, npc_stats[j].attack, param_opt.spacer, npc_stats[j].defense, param_opt.spacer, npc_stats[j].magicdef, param_opt.spacer, npc_stats[j].evade, param_opt.spacer, npc_stats[j].hit, param_opt.spacer, npc_stats[j].aggression_range, param_opt.spacer, npc_stats[j].aggression_chance_code, param_opt.spacer, aggression_chance_std, param_opt.spacer, npc_stats[j].unique_trait_flags, param_opt.spacer, npc_stats[j].unique_trait_flags_str, param_opt.spacer, npc_stats[j].attack_properties, param_opt.spacer, npc_stats[j].attack_properties_str, param_opt.spacer, npc_stats[j].experience, param_opt.spacer, npc_stats[j].money, param_opt.spacer, npc_stats[j].chance_prize_drop_code, param_opt.spacer, chance_prize_drop_std, param_opt.spacer, npc_stats[j].chargeup_limit, param_opt.spacer, npc_stats[j].chargeup_gain_per_frame, param_opt.spacer, npc_stats[j].unknown_maybe_collision_related, param_opt.spacer, npc_stats[j].palette, param_opt.spacer, npc_stats[j].death_script_nospoils, param_opt.spacer, npc_stats[j].death_script_spoils, param_opt.spacer, npc_stats[j].block_script, param_opt.spacer, npc_stats[j].stat_ptr, param_opt.spacer, npc_stats[j].name_ptr);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		fclose(idf);
	}
	printf("\n");
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// [Ingredients Info] Section
	switch(rom_info.region) {
		// USA: 0x45B10 - 0x45B3B
		case 0x02:	// Europe, Asia and Oceania
		case 0x09:	// Germany
			ingredientStartPoint += 0xA666;	// 0x50176
			ingredientEndPoint += 0xA666;	// 0x501A1
			break;
		case 0x06:	// France
		case 0x08:	// Spain
			ingredientStartPoint -= 0x599A;	// 0x40176
			ingredientEndPoint -= 0x599A;	// 0x401A1
			break;
	}
	unsigned int ingredient_count = (ingredientEndPoint-1-ingredientStartPoint)/0x02+1;	//usually 0x16, 22
	ingredient_info_type ingredient_info[ingredient_count];
	{
		// [Ingredients Info] Read data from ROM
		unsigned int i = ingredientStartPoint;
		unsigned int j = 0;
		while(i < ingredientEndPoint) {
			ingredient_info[j].index = j;
			ingredient_info[j].name_ptr = (unsigned char)*(tmp_buff+i+0x4E3+0x00) + ((unsigned char)*(tmp_buff+i+0x4E3+0x01) << 8) + (romaddr_to_mapaddr(i & 0xFF0000, 0));
			int name_txt_ptr = mapaddr_to_romaddr(ingredient_info[j].name_ptr);
			if ((name_txt_ptr < 0) || (name_txt_ptr > idf_size)) {
				strcpy(ingredient_info[j].name, "(invalid_ptr)");
			} else if (name_txt_ptr == 0) {
				strcpy(ingredient_info[j].name, "(unnamed)");
			} else {
				strcpy(ingredient_info[j].name, (char *)(tmp_buff+name_txt_ptr+(type == 2 ? param_opt.hsize : 0)));
				if (rom_info.region != 0x01) {
					conv_romstr_to_ansistr(ingredient_info[j].name);
				}
			}
			ingredient_info[j].sprite_info_ptr = *(unsigned short int *)(tmp_buff+i);
			ingredient_info[j].sprite_ptr = (unsigned char)*(tmp_buff+i+j+0x1B7+0x00) + ((unsigned char)*(tmp_buff+i+j+0x1B7+0x01) << 8) + ((unsigned char)*(tmp_buff+i+j+0x1B7+0x02) << 16);
			ingredient_info[j].ingredient_graphic_setup = *(unsigned int *)(tmp_buff+i+(int)j*2+(20-(int)j*2+(j == 21 ? 22 : (j == 19 ? 9 : (j < 19 && j > 9 ? -1 : 0))))*4+0x2A5D0+((rom_info.region == 0x02 || rom_info.region == 0x09) ? 0x10000: 0));
			i += 0x02;
			j++;
		}
		// [Ingredients Info] Write to file
		if ((idf=fopen(fileout_ingredientsinfo, "wb"))==NULL) {
			printf("[!] Error: unable to open the output file \"%s\"\n", fileout_ingredientsinfo);
			return -3;
		}
		char buff_line[MAX_BUFF_SIZE];
		snprintf(buff_line, MAX_BUFF_SIZE, "Index%cIngredient Name%cGraphic Setup%c[PTR] Sprite Info%c[PTR] Sprite%c[PTR] Ingredient Name\n", param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		for(unsigned int j = 0; j < ingredient_count; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "%d%c\"%s\"%c0x%08X%c0x%04X%c0x%06X%c0x%06X\n", ingredient_info[j].index, param_opt.spacer, ingredient_info[j].name, param_opt.spacer, ingredient_info[j].ingredient_graphic_setup, param_opt.spacer, ingredient_info[j].sprite_info_ptr, param_opt.spacer, ingredient_info[j].sprite_ptr, param_opt.spacer, ingredient_info[j].name_ptr);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		fclose(idf);
	}
	printf("\n");
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// [Alchemy Info] Section
	switch(rom_info.region) {
		// USA: 0x45C3B - 0x45C80
		case 0x02:	// Europe, Asia and Oceania
		case 0x09:	// Germany
			alchemyStartPoint += 0xA666;	// 0x502A1
			alchemyEndPoint += 0xA666;		// 0x502E6
			break;
		case 0x06:	// France
		case 0x08:	// Spain
			alchemyStartPoint -= 0x599A;	// 0x402A1
			alchemyEndPoint -= 0x599A;		// 0x402E6
			break;
	}
	unsigned int alchemy_count = (alchemyEndPoint-1-alchemyStartPoint)/0x02+1;	//usually 0x46, 70
	alchemy_info_type alchemy_info[alchemy_count];
	{
		// [Alchemy Info] Read data from ROM
		unsigned int i = alchemyStartPoint;
		unsigned int j = 0;
		while(i < alchemyEndPoint) {
			alchemy_info[j].index = j;
			alchemy_info[j].name_ptr = (unsigned char)*(tmp_buff+i+0xCE +0x00) + ((unsigned char)*(tmp_buff+i+0xCE +0x01) << 8) + (romaddr_to_mapaddr(i & 0xFF0000, 0));
			int name_txt_ptr = mapaddr_to_romaddr(alchemy_info[j].name_ptr);
			if ((name_txt_ptr < 0) || (name_txt_ptr > idf_size)) {
				strcpy(alchemy_info[j].name, "(invalid_ptr)");
			} else if (name_txt_ptr == 0) {
				strcpy(alchemy_info[j].name, "(unnamed)");
			} else {
				strcpy(alchemy_info[j].name, (char *)(tmp_buff+name_txt_ptr+(type == 2 ? param_opt.hsize : 0)));
				if (rom_info.region != 0x01) {
					conv_romstr_to_ansistr(alchemy_info[j].name);
				}
			}
			alchemy_info[j].description_ptr = (unsigned char)*(tmp_buff+i+0x116+0x00) + ((unsigned char)*(tmp_buff+i+0x116+0x01) << 8) + (romaddr_to_mapaddr(i & 0xFF0000, 0));
			int description_txt_ptr = mapaddr_to_romaddr(alchemy_info[j].description_ptr);
			if ((description_txt_ptr < 0) || (description_txt_ptr > idf_size)) {
				strcpy(alchemy_info[j].description, "(invalid_ptr)");
			} else if (description_txt_ptr == 0) {
				strcpy(alchemy_info[j].description, "(undescribed)");
			} else {
				strcpy(alchemy_info[j].description, (char *)(tmp_buff+description_txt_ptr+(type == 2 ? param_opt.hsize : 0)));
				if (rom_info.region != 0x01) {
					conv_romstr_to_ansistr(alchemy_info[j].description);
				}
			}
			alchemy_info[j].spell_might_value = *(unsigned short int *)(tmp_buff+i+0x230);
			alchemy_info[j].target_flags = *(unsigned short int *)(tmp_buff+i-0x46);
			spell_targetflags_setdesc(alchemy_info[j].target_flags_str, alchemy_info[j].target_flags);
			alchemy_info[j].ingredient1 = *(unsigned char *)(tmp_buff+i+j*2+0x3E4+0x00);
			alchemy_info[j].ingredient2 = *(unsigned char *)(tmp_buff+i+j*2+0x3E4+0x01);
			alchemy_info[j].ingredient1_cost = *(unsigned char *)(tmp_buff+i+j*2+0x3E4+0x02);
			alchemy_info[j].ingredient2_cost = *(unsigned char *)(tmp_buff+i+j*2+0x3E4+0x03);
			strcpy(alchemy_info[j].ingredient1_name, ingredient_info[alchemy_info[j].ingredient1].name);
			strcpy(alchemy_info[j].ingredient2_name, ingredient_info[alchemy_info[j].ingredient2].name);
			alchemy_info[j].spell_influence_type = *(short int *)(tmp_buff+i+0x276);
			spell_influencetype_setdesc(alchemy_info[j].spell_influence_type_str, alchemy_info[j].spell_influence_type);
			alchemy_info[j].learned_alchemy_flags_addresses = *(unsigned short int *)(tmp_buff+i+0x00);
			alchemy_info[j].learned_alchemy_flags_bit_weights = *(unsigned short int *)(tmp_buff+i+0x46);
			alchemy_info[j].behaviour_script_ptr = *(unsigned short int *)(tmp_buff+i+0x15E);
			alchemy_info[j].invocation_script_ptr = *(unsigned short int *)(tmp_buff+i+0x1A4);
			alchemy_info[j].sprite_info_ptr = *(unsigned short int *)(tmp_buff+i-0x8C);
			alchemy_info[j].unknown_maybe_sprite_related = *(unsigned short int *)(tmp_buff+i+0x1EA);
			i += 0x02;
			j++;
		}
		// [Alchemy Info] Write to file
		if ((idf=fopen(fileout_alchemyinfo, "wb"))==NULL) {
			printf("[!] Error: unable to open the output file \"%s\"\n", fileout_alchemyinfo);
			return -3;
		}
		char buff_line[MAX_BUFF_SIZE];
		snprintf(buff_line, MAX_BUFF_SIZE, "Index%cAlchemy Name%cAlchemy Description%cSpell Might Value%cTargetFlags%cTargetFlags Description%cIngredient1 Index%cIngredient2 Index%cIngredient1 Name%cIngredient2 Name%cIngredient1 Cost%cIngredient2 Cost%cInfluence Type%cInfluence Type Desc%cLearned Alchemy Flags (Addresses)%cLearned Alchemy Flags (Bit weights)%c[PTR] Behaviour Script%c[PTR] Invocation Script%c[PTR] Sprite Info%cUnknown maybe Sprite related%c[PTR] Alchemy Name%c[PTR] Alchemy Description\n", param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		for(unsigned int j = 0; j < alchemy_count; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "%d%c\"%s\"%c\"%s\"%c%u%c0x%04X%c\"%s\"%c%d%c%d%c\"%s\"%c\"%s\"%c%d%c%d%c%d%c\"%s\"%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%06X%c0x%06X\n", alchemy_info[j].index, param_opt.spacer, alchemy_info[j].name, param_opt.spacer, alchemy_info[j].description, param_opt.spacer, alchemy_info[j].spell_might_value, param_opt.spacer, alchemy_info[j].target_flags, param_opt.spacer, alchemy_info[j].target_flags_str, param_opt.spacer, alchemy_info[j].ingredient1, param_opt.spacer, alchemy_info[j].ingredient2, param_opt.spacer, alchemy_info[j].ingredient1_name, param_opt.spacer, alchemy_info[j].ingredient2_name, param_opt.spacer, alchemy_info[j].ingredient1_cost, param_opt.spacer, alchemy_info[j].ingredient2_cost, param_opt.spacer, alchemy_info[j].spell_influence_type, param_opt.spacer, alchemy_info[j].spell_influence_type_str, param_opt.spacer, alchemy_info[j].learned_alchemy_flags_addresses, param_opt.spacer, alchemy_info[j].learned_alchemy_flags_bit_weights, param_opt.spacer, alchemy_info[j].behaviour_script_ptr, param_opt.spacer, alchemy_info[j].invocation_script_ptr, param_opt.spacer, alchemy_info[j].sprite_info_ptr, param_opt.spacer, alchemy_info[j].unknown_maybe_sprite_related, param_opt.spacer, alchemy_info[j].name_ptr, param_opt.spacer, alchemy_info[j].description_ptr);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		fclose(idf);
	}
	printf("\n");
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// [Call Bead Info] Section
	switch(rom_info.region) {
		// USA: 0x45C3B - 0x45C80
		case 0x02:	// Europe, Asia and Oceania
		case 0x09:	// Germany
			callbeadStartPoint += 0xA666;	// 0x5055D
			callbeadEndPoint += 0xA666;		// 0x4055D
			break;
		case 0x06:	// France
		case 0x08:	// Spain
			callbeadStartPoint -= 0x599A;	// 0x5057C
			callbeadEndPoint -= 0x599A;		// 0x4057C
			break;
	}
	unsigned int callbead_count = (callbeadEndPoint-1-callbeadStartPoint)/0x02+1;	//usually 0x10, 16
	callbead_info_type callbead_info[callbead_count];
	{
		// [Call Bead Info] Read data from ROM
		unsigned int i = callbeadStartPoint;
		unsigned int j = 0;
		while(i < callbeadEndPoint) {
			callbead_info[j].index = j;
			callbead_info[j].spell_name_ptr = (unsigned char)*(tmp_buff+0xE85D4+(type == 2 ? param_opt.hsize : 0)+(j > 12 ? (j > 13 ? j-1 : j+2) : j)*8+0x00) + ((unsigned char)*(tmp_buff+0xE85D4+(type == 2 ? param_opt.hsize : 0)+(j > 12 ? (j > 13 ? j-1 : j+2) : j)*8+0x01) << 8) + (romaddr_to_mapaddr(i & 0xFF0000, 0));
			int spell_name_txt_ptr = mapaddr_to_romaddr(callbead_info[j].spell_name_ptr);
			if ((spell_name_txt_ptr < 0) || (spell_name_txt_ptr > idf_size)) {
				strcpy(callbead_info[j].spell_name, "(invalid_ptr)");
			} else if (spell_name_txt_ptr == 0) {
				strcpy(callbead_info[j].spell_name, "(unnamed)");
			} else {
				strcpy(callbead_info[j].spell_name, (char *)(tmp_buff+spell_name_txt_ptr+(type == 2 ? param_opt.hsize : 0)));
				if (rom_info.region != 0x01) {
					conv_romstr_to_ansistr(callbead_info[j].spell_name);
				}
			}
			callbead_info[j].spell_influence_type = *(short int *)(tmp_buff+i+0x40);
			spell_influencetype_setdesc(callbead_info[j].spell_influence_type_str, callbead_info[j].spell_influence_type);
			callbead_info[j].spell_might_value = *(unsigned short int *)(tmp_buff+i+0x20);
			callbead_info[j].target_flags = *(unsigned short int *)(tmp_buff+i+0x00);
			spell_targetflags_setdesc(callbead_info[j].target_flags_str, callbead_info[j].target_flags);
			callbead_info[j].invocation_script_ptr = *(unsigned short int *)(tmp_buff+i+0x60);
			callbead_info[j].sprite_info_ptr = *(unsigned short int *)(tmp_buff+i+0x80);
			callbead_info[j].sprite_related_ptr = *(unsigned short int *)(tmp_buff+0xE85D4+(type == 2 ? param_opt.hsize : 0)+(j > 12 ? (j > 13 ? j-1 : j+2) : j)*8+0x02);
			callbead_info[j].unknown_value1 = *(unsigned short int *)(tmp_buff+0xE85D4+(type == 2 ? param_opt.hsize : 0)+(j > 12 ? (j > 13 ? j-1 : j+2) : j)*8+0x04);
			callbead_info[j].unknown_value2 = *(unsigned short int *)(tmp_buff+0xE85D4+(type == 2 ? param_opt.hsize : 0)+(j > 12 ? (j > 13 ? j-1 : j+2) : j)*8+0x06);
			i += 0x02;
			j++;
		}		
		// [Call Bead Info] Write to file
		if ((idf=fopen(fileout_callbeadinfo, "wb"))==NULL) {
			printf("[!] Error: unable to open the output file \"%s\"\n", fileout_callbeadinfo);
			return -3;
		}
		char buff_line[MAX_BUFF_SIZE];
		snprintf(buff_line, MAX_BUFF_SIZE, "Index%cCall Bead Spell Name%cSpell Might Value%cTargetFlags%cTargetFlags Description%cInfluence Type%cInfluence Type Desc%c[PTR] Invocation Script%c[PTR] Sprite Info%c[PTR] Sprite Related%cUnknown Value 1%cUnknown Value 2%c[PTR] Call Bead Spell Name\n", param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		for(unsigned int j = 0; j < callbead_count; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "%d%c\"%s\"%c%u%c0x%04X%c\"%s\"%c%d%c\"%s\"%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%06X\n", callbead_info[j].index, param_opt.spacer, callbead_info[j].spell_name, param_opt.spacer, callbead_info[j].spell_might_value, param_opt.spacer, callbead_info[j].target_flags, param_opt.spacer, callbead_info[j].target_flags_str, param_opt.spacer, callbead_info[j].spell_influence_type, param_opt.spacer, callbead_info[j].spell_influence_type_str, param_opt.spacer, callbead_info[j].invocation_script_ptr, param_opt.spacer, callbead_info[j].sprite_info_ptr, param_opt.spacer, callbead_info[j].sprite_related_ptr, param_opt.spacer, callbead_info[j].unknown_value1, param_opt.spacer, callbead_info[j].unknown_value2, param_opt.spacer, callbead_info[j].spell_name_ptr);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		fclose(idf);
	}
	printf("\n");
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// [Weapon Info] Section
	switch(rom_info.region) {
		// USA: 0x43876 - 0x43893; 0x459A6 - 0x459BF
		case 0x02:	// Europe, Asia and Oceania
		case 0x09:	// Germany
			weaponStartPoint += 0x640;		// 0x43EB6
			weaponEndPoint += 0x640;		// 0x43ED3
			weaponStartPoint2 += 0xA666;	// 0x5000C
			weaponEndPoint2 += 0xA666;		// 0x50025
			break;
		case 0x06:	// France
		case 0x08:	// Spain
			weaponStartPoint -= 0xF9C0;		// 0x33EB6
			weaponEndPoint -= 0xF9C0;		// 0x33ED3
			weaponStartPoint2 -= 0x599A;	// 0x4000C
			weaponEndPoint2 -= 0x599A;		// 0x40025
			break;
	}
	unsigned int weapon_count = (weaponEndPoint-1-weaponStartPoint)/0x02+1;	//usually 0x0F, 15
	weapon_info_type weapon_info[weapon_count];
	{
		// [Weapon Info] Read data from ROM
		unsigned int i = weaponStartPoint;
		unsigned int i2 = weaponStartPoint2;
		unsigned int j = 0;
		while(i < weaponEndPoint) {
			weapon_info[j].index = j;
			weapon_info[j].data_ptr = (unsigned char)*(tmp_buff+i+0x00) + ((unsigned char)*(tmp_buff+i+0x01) << 8) + (romaddr_to_mapaddr(i & 0xFF0000, 0));
			int data_weapon_ptr = mapaddr_to_romaddr(weapon_info[j].data_ptr);
			if ((data_weapon_ptr <= 0) || (data_weapon_ptr > idf_size)) {
				weapon_info[j].name_ptr = -1;
				strcpy(weapon_info[j].name, "(invalid_ptr)");
				weapon_info[j].attack_power = 0;
				for (int k = 0; k < 16; ++k) {
					weapon_info[j].unknown_raw_data[k] = 0u;
				}
			} else {
				unsigned char * weapon_data_fullptr = (unsigned char *)(tmp_buff+data_weapon_ptr+(type == 2 ? param_opt.hsize : 0));
				weapon_info[j].name_ptr = (unsigned char)*(weapon_data_fullptr+0x02 +0x00) + ((unsigned char)*(weapon_data_fullptr+0x02 +0x01) << 8) + (romaddr_to_mapaddr(i2 & 0xFF0000, 0));
				int name_txt_ptr = mapaddr_to_romaddr(weapon_info[j].name_ptr);
				if ((name_txt_ptr < 0) || (name_txt_ptr > idf_size)) {
					strcpy(weapon_info[j].name, "(invalid_ptr)");
				} else if (name_txt_ptr == 0) {
					strcpy(weapon_info[j].name, "(unnamed)");
				} else {
					strcpy(weapon_info[j].name, (char *)(tmp_buff+name_txt_ptr+(type == 2 ? param_opt.hsize : 0)));
					if (rom_info.region != 0x01) {
						conv_romstr_to_ansistr(weapon_info[j].name);
					}
				}
				weapon_info[j].attack_power = *(short int *)(weapon_data_fullptr+0x00);
				for (int k = 0; k < 16; ++k) {
					weapon_info[j].unknown_raw_data[k] = *(unsigned short int *)(weapon_data_fullptr+0x04+(k*0x02));
				}
			}
			if (j < 12) {	//Only Swords, Axes and Spears have description
				weapon_info[j].description_ptr = (unsigned char)*(tmp_buff+i2+0x54+0x00) + ((unsigned char)*(tmp_buff+i2+0x54+0x01) << 8) + (romaddr_to_mapaddr(i2 & 0xFF0000, 0));
			} else {
				weapon_info[j].description_ptr = 0;
			}
			int description_txt_ptr = mapaddr_to_romaddr(weapon_info[j].description_ptr);
			if ((description_txt_ptr < 0) || (description_txt_ptr > idf_size)) {
				strcpy(weapon_info[j].description, "(invalid_ptr)");
			} else if (description_txt_ptr == 0) {
				strcpy(weapon_info[j].description, "(undescribed)");
			} else {
				strcpy(weapon_info[j].description, (char *)(tmp_buff+description_txt_ptr+(type == 2 ? param_opt.hsize : 0)));
				if (rom_info.region != 0x01) {
					conv_romstr_to_ansistr(weapon_info[j].description);
				}
			}
			weapon_info[j].obtained_flags_addresses = *(unsigned short int *)(tmp_buff+i2+0x00-(j<13 ? 0 : (j-12)*0x02));
			weapon_info[j].obtained_flags_bit_weights = *(unsigned short int *)(tmp_buff+i2+0x1C-(j<13 ? 0 : (j-12)*0x02));
			weapon_info[j].familie = *(unsigned short int *)(tmp_buff+i2+0x38-(j<13 ? 0 : (j-12)*0x02));
			switch(weapon_info[j].familie) {
				case 0x00:
					strcpy(weapon_info[j].familie_str, "Sword");
					break;
				case 0x02:
					strcpy(weapon_info[j].familie_str, "Axe");
					break;
				case 0x04:
					strcpy(weapon_info[j].familie_str, "Spear");
					break;
				case 0x06:
					strcpy(weapon_info[j].familie_str, "Bazooka");
					break;
				default:
					strcpy(weapon_info[j].familie_str, "(unknown)");
			}
			i += 0x02;
			i2 += 0x02;
			j++;
		}
		// [Weapon Info] Write to file
		if ((idf=fopen(fileout_weaponinfo, "wb"))==NULL) {
			printf("[!] Error: unable to open the output file \"%s\"\n", fileout_weaponinfo);
			return -3;
		}
		char buff_line[MAX_BUFF_SIZE];
		snprintf(buff_line, MAX_BUFF_SIZE, "Index%cWeapon Name%cAttack Power%cFamilie%cFamilie Desc%cObtained Flags (Addresses)%cObtained Flags (Bit weights)%cDescription%cUnknown Raw Data [01]%cUnknown Raw Data [02]%cUnknown Raw Data [03]%cUnknown Raw Data [04]%cUnknown Raw Data [05]%cUnknown Raw Data [06]%cUnknown Raw Data [07]%cUnknown Raw Data [08]%cUnknown Raw Data [09]%cUnknown Raw Data [10]%cUnknown Raw Data [11]%cUnknown Raw Data [12]%cUnknown Raw Data [13]%cUnknown Raw Data [14]%cUnknown Raw Data [15]%cUnknown Raw Data [16]%c[PTR] Data Info%c[PTR] Weapon Name%c[PTR] Weapon Description\n", param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		for(unsigned int j = 0; j < weapon_count; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "%d%c\"%s\"%c%d%c0x%04X%c\"%s\"%c0x%04X%c0x%04X%c\"%s\"%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%04X%c0x%06X%c0x%06X%c0x%06X\n", weapon_info[j].index, param_opt.spacer, weapon_info[j].name, param_opt.spacer, weapon_info[j].attack_power, param_opt.spacer, weapon_info[j].familie, param_opt.spacer, weapon_info[j].familie_str, param_opt.spacer, weapon_info[j].obtained_flags_addresses, param_opt.spacer, weapon_info[j].obtained_flags_bit_weights, param_opt.spacer, weapon_info[j].description, param_opt.spacer, weapon_info[j].unknown_raw_data[0], param_opt.spacer, weapon_info[j].unknown_raw_data[1], param_opt.spacer, weapon_info[j].unknown_raw_data[2], param_opt.spacer, weapon_info[j].unknown_raw_data[3], param_opt.spacer, weapon_info[j].unknown_raw_data[4], param_opt.spacer, weapon_info[j].unknown_raw_data[5], param_opt.spacer, weapon_info[j].unknown_raw_data[6], param_opt.spacer, weapon_info[j].unknown_raw_data[7], param_opt.spacer, weapon_info[j].unknown_raw_data[8], param_opt.spacer, weapon_info[j].unknown_raw_data[9], param_opt.spacer, weapon_info[j].unknown_raw_data[10], param_opt.spacer, weapon_info[j].unknown_raw_data[11], param_opt.spacer, weapon_info[j].unknown_raw_data[12], param_opt.spacer, weapon_info[j].unknown_raw_data[13], param_opt.spacer, weapon_info[j].unknown_raw_data[14], param_opt.spacer, weapon_info[j].unknown_raw_data[15], param_opt.spacer, weapon_info[j].data_ptr, param_opt.spacer, weapon_info[j].name_ptr, param_opt.spacer, weapon_info[j].description_ptr);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		fclose(idf);
	}
	printf("\n");
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// [Armor Info] Section
	switch(rom_info.region) {
		// USA: 0x43896 - 0x438E5; 0x45AC0 - 0x45B0F
		case 0x02:	// Europe, Asia and Oceania
		case 0x09:	// Germany
			armorStartPoint += 0x640;		// 0x43ED6
			armorEndPoint += 0x640;			// 0x43F25
			armorStartPoint2 += 0xA666;		// 0x50126
			armorEndPoint2 += 0xA666;		// 0x50175
			break;
		case 0x06:	// France
		case 0x08:	// Spain
			armorStartPoint -= 0xF9C0;		// 0x33ED6
			armorEndPoint -= 0xF9C0;		// 0x33F25
			armorStartPoint2 -= 0x599A;		// 0x40126
			armorEndPoint2 -= 0x599A;		// 0x40175
			break;
	}
	unsigned int armor_count = (armorEndPoint-1-armorStartPoint)/0x02+1;	//usually 0x28, 40
	armor_info_type armor_info[armor_count];
	{
		// [Armor Info] Read data from ROM
		unsigned int i = armorStartPoint;
		unsigned int i2 = armorStartPoint2;
		unsigned int j = 0;
		while(i < armorEndPoint) {
			armor_info[j].index = j;
			armor_info[j].data_ptr = (unsigned char)*(tmp_buff+i+0x00) + ((unsigned char)*(tmp_buff+i+0x01) << 8) + (romaddr_to_mapaddr(i & 0xFF0000, 0));
			int data_armor_ptr = mapaddr_to_romaddr(armor_info[j].data_ptr);
			if ((data_armor_ptr <= 0) || (data_armor_ptr > idf_size)) {
				armor_info[j].name_ptr = -1;
				strcpy(armor_info[j].name, "(invalid_ptr)");
				armor_info[j].deffense_value_effective = 0;
				armor_info[j].deffense_value_visual = 0;
				armor_info[j].unknown_pointer1 = 0u;
				armor_info[j].unknown_value1 = 0u;
			} else {
				unsigned char * armor_data_fullptr = (unsigned char *)(tmp_buff+data_armor_ptr+(type == 2 ? param_opt.hsize : 0));
				armor_info[j].name_ptr = (unsigned char)*(armor_data_fullptr+0x04 +0x00) + ((unsigned char)*(armor_data_fullptr+0x04 +0x01) << 8) + (romaddr_to_mapaddr(i2 & 0xFF0000, 0));
				int name_txt_ptr = mapaddr_to_romaddr(armor_info[j].name_ptr);
				if ((name_txt_ptr < 0) || (name_txt_ptr > idf_size)) {
					strcpy(armor_info[j].name, "(invalid_ptr)");
				} else if (name_txt_ptr == 0) {
					strcpy(armor_info[j].name, "(unnamed)");
				} else {
					strcpy(armor_info[j].name, (char *)(tmp_buff+name_txt_ptr+(type == 2 ? param_opt.hsize : 0)));
					if (rom_info.region != 0x01) {
						conv_romstr_to_ansistr(armor_info[j].name);
					}
				}
				armor_info[j].deffense_value_effective = *(short int *)(armor_data_fullptr+0x00);
				armor_info[j].deffense_value_visual = *(short int *)(armor_data_fullptr+0x02);
				armor_info[j].unknown_pointer1 = *(unsigned short int *)(armor_data_fullptr+0x06);
				armor_info[j].unknown_value1 = *(unsigned short int *)(armor_data_fullptr+0x08);
			}
			armor_info[j].default_sell_price = *(short int *)(tmp_buff+i2+0x8A);
			armor_info[j].sprite_info_ptr = *(unsigned short int *)(tmp_buff+i2);
			i += 0x02;
			i2 += 0x02;
			j++;
		}
		// [Armor Info] Write to file
		if ((idf=fopen(fileout_armorinfo, "wb"))==NULL) {
			printf("[!] Error: unable to open the output file \"%s\"\n", fileout_armorinfo);
			return -3;
		}
		char buff_line[MAX_BUFF_SIZE];
		snprintf(buff_line, MAX_BUFF_SIZE, "Index%cArmor Name%cDeffense Effective Value%cDeffense Visual Value%cDefault Sell Price%cUnknown_Pointer1%cUnknown_Value1%c[PTR] Sprite Info%c[PTR] Data Info%c[PTR] Armor Name\n", param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		for(unsigned int j = 0; j < armor_count; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "%d%c\"%s\"%c%d%c%d%c%d%c0x%04X%c0x%04X%c0x%04X%c0x%06X%c0x%06X\n", armor_info[j].index, param_opt.spacer, armor_info[j].name, param_opt.spacer, armor_info[j].deffense_value_effective, param_opt.spacer, armor_info[j].deffense_value_visual, param_opt.spacer, armor_info[j].default_sell_price, param_opt.spacer, armor_info[j].unknown_pointer1, param_opt.spacer, armor_info[j].unknown_value1, param_opt.spacer, armor_info[j].sprite_info_ptr, param_opt.spacer, armor_info[j].data_ptr, param_opt.spacer, armor_info[j].name_ptr);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		fclose(idf);
	}
	printf("\n");
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// TODO: [Merchant Info] Section
	{
		// [Merchant Info] Read data from ROM
		// [Merchant Info] Write to file
	}
	////////////////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////////////////
	// [Boy&Dog Info] Section
	////
	switch(rom_info.region) {
		// USA: 0xC8BD0; 0xC8CFC
		case 0x02:	// Europe, Asia and Oceania
		case 0x09:	// Germany
		case 0x06:	// France
		case 0x08:	// Spain
			boyInfoStartPoint += 0x395;		// 0xC8F65
			dogInfoStartPoint += 0x395;		// 0xC9091
			break;
	}
	unsigned int boydog_count = 2u;
	boydog_info_type boydog_info[boydog_count];
	{
		// [Boy&Dog Info] Read data from ROM
		//unsigned int j = 0;
		//while(j < boydog_count) {
		boydog_info[0].index = 0;
		boydog_info[1].index = 1;
		strcpy(boydog_info[0].unit, "Boy");
		strcpy(boydog_info[1].unit, "Dog");
		for (int k = 0; k < 99; k++) {
			// 0xC8BD0-0xC8CF8 (0x129) = Boy; 0xC8CFC-0xC8E24 = Dog (0x129) | total experience needed to reach level (99 entries, 3 bytes each)
				// All-non-USA: 0xC8F65-0xC908D [+0x395]; 0xC9091-0xC91B9 [+0x395]
			boydog_info[0].exp_needed_per_level[k] = (unsigned char)*(tmp_buff+boyInfoStartPoint+(k*3)+0x00) + ((unsigned char)*(tmp_buff+boyInfoStartPoint+(k*3)+0x01) << 8) + ((unsigned char)*(tmp_buff+boyInfoStartPoint+(k*3)+0x02) << 16);
			boydog_info[1].exp_needed_per_level[k] = (unsigned char)*(tmp_buff+dogInfoStartPoint+(k*3)+0x00) + ((unsigned char)*(tmp_buff+dogInfoStartPoint+(k*3)+0x01) << 8) + ((unsigned char)*(tmp_buff+dogInfoStartPoint+(k*3)+0x02) << 16);
		}
		for (int k = 0; k < 111; k++) {
			// 0xC8E25-0xC8F02 (0xDE) = Boy; 0xC8FE1-0xC90BE = Dog (0xDE) | Evade % by level (entries for 0-110, 2 bytes each, Thug's Cloak +5 to used level)
				// All-non-USA: 0xC91BA-0xC9297 [+0x395]; 0xC9376-0xC9453! [+0x395]
			boydog_info[0].evade_per_level[k] = (unsigned short int)*(tmp_buff+boyInfoStartPoint+0x255+(k*2));
			boydog_info[1].evade_per_level[k] = (unsigned short int)*(tmp_buff+dogInfoStartPoint+0x2E5+(k*2));
			// 0xC8F03-0xC8FE0 (0xDE) = Boy; 0xC90BF-0xC919C = Dog (0xDE) | Hit % by level (entries for 0-110, 2 bytes each, Jade Disk +5 to used level)
				// All-non-USA: 0xC9298-0xC9375 [+0x395]; 0xC9454-0xC9531 [+0x395]
			boydog_info[0].hit_per_level[k] = (unsigned short int)*(tmp_buff+boyInfoStartPoint+0x333+(k*2));
			boydog_info[1].hit_per_level[k] = (unsigned short int)*(tmp_buff+dogInfoStartPoint+0x3C3+(k*2));
			// 0xC919D-0xC927A (0xDE) = Boy; 0xC9515-0xC95F2 = Dog (0xDE) | Max HP by level (entries for 0-110, 2 bytes each, Chocobo Egg +5 to used level)
				// All-non-USA: 0xC9532-0xC960F [+0x395]; 0xC98AA-0xC9987 [+0x395]
			boydog_info[0].maxhp_per_level[k] = (unsigned short int)*(tmp_buff+boyInfoStartPoint+0x5CD+(k*2));
			boydog_info[1].maxhp_per_level[k] = (unsigned short int)*(tmp_buff+dogInfoStartPoint+0x819+(k*2));
			// 0xC927B-0xC9358 (0xDE) = Boy; 0xC95F3-0xC96D0 = Dog (0xDE) | base Defense by level (entries for 0-110, 2 bytes each, Staff of Life +5 to used level)
				// All-non-USA: 0xC9610-0xC96ED [+0x395]; 0xC9988-0xC9A65 [+0x395]
			boydog_info[0].base_defense_per_level[k] = (unsigned short int)*(tmp_buff+boyInfoStartPoint+0x6AB+(k*2));
			boydog_info[1].base_defense_per_level[k] = (unsigned short int)*(tmp_buff+dogInfoStartPoint+0x8F7+(k*2));
			// 0xC9359-0xC9436 (0xDE) = Boy; 0xC96D1-0xC97AE = Dog (0xDE) | base Attack by level (entries for 0-110, 2 bytes each, Sun Stone +5 to used level)
				// All-non-USA: 0xC96EE-0xC97CB [+0x395]; 0xC9A66-0xC9B43! [+0x395]
			boydog_info[0].base_attack_per_level[k] = (unsigned short int)*(tmp_buff+boyInfoStartPoint+0x789+(k*2));
			boydog_info[1].base_attack_per_level[k] = (unsigned short int)*(tmp_buff+dogInfoStartPoint+0x9D5+(k*2));
			// 0xC9437-0xC9514 (0xDE) = Boy; 0xC97AF-0xC988C = Dog (0xDE) | Magic Defense by level (entries for 0-110, 2 bytes each, Wizard's Coin +7/+5 to used level)
				// All-non-USA: 0xC97CC-0xC98A9 [+0x395]; 0xC9B44-0xC9C21 [+0x395]
			boydog_info[0].magic_defense_per_level[k] = (unsigned short int)*(tmp_buff+boyInfoStartPoint+0x867+(k*2));
			boydog_info[1].magic_defense_per_level[k] = (unsigned short int)*(tmp_buff+dogInfoStartPoint+0xAB3+(k*2));
		}
		// 0xF8388-0xF8397 (0x10) = Boy Text: " reaches level "; 0xF856D-0xF857C (0x10) = Dog Text: " reaches level "
			// Europe:  0x51DE8-0x51DF7 [-0xA65A0]; 0x51DE8-0x51DF7 [-0xA6785]
			// Germany: 0x51F04-0x51F14 [-0xA6484]; 0x51F04-0x51F14 [-0xA6669]
			// France:  0x41C38-0x41C48 [-0xB6750]; 0x41C38-0x41C48 [-0xB6935]
			// Spain:   0x41FFD-0x4200D [-0xB638B]; 0x41FFD-0x4200D [-0xB6570]
		//boydog_info[0/1].text_level_up_str
		// 0xF97B1-0xF97BF (0x0F) = Boy Text: " is now level "; 0xF9801-0xF9816 (0x16) = Dog Text: "Dog attack now level "
			// Europe:  0x51DD9-0x51DE7 [-0xA79D8]; 0x51DC3-0x51DD8 [-0xA7A3E]
			// Germany: 0x51EF3-0x51F03 [-0xA78BE]; 0x51EDF-0x51EF2 [-0xA7922]
			// France:  0x41C2B-0x41C37 [-0xB7B86]; 0x41C13-0x41C2A [-0xB7BEE]
			// Spain:   0x41FED-0x41FFC [-0xB77C4]; 0x41FD4-0x41FEC [-0xB782D]
		//boydog_info[0/1].text_weaponlevel_up_str
		switch(rom_info.region) {
		case 0x01:	// USA
			strcpy(boydog_info[0].text_level_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0xF8388);			// 0xF8388
			strcpy(boydog_info[1].text_level_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0xF856D);			// 0xF856D
			strcpy(boydog_info[0].text_weaponlevel_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0xF97B1);	// 0xF97B1
			strcpy(boydog_info[1].text_weaponlevel_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0xF9801);	// 0xF9801
			break;
		case 0x02:	// Europe, Asia and Oceania
			strcpy(boydog_info[0].text_level_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x51DE8);			// 0x51DE8
			strcpy(boydog_info[1].text_level_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x51DE8);			// 0x51DE8
			strcpy(boydog_info[0].text_weaponlevel_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x51DD9);	// 0x51DD9
			strcpy(boydog_info[1].text_weaponlevel_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x51DC3);	// 0x51DC3
			break;
		case 0x09:	// Germany
			strcpy(boydog_info[0].text_level_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x51F04);			// 0x51F04
			strcpy(boydog_info[1].text_level_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x51F04);			// 0x51F04
			strcpy(boydog_info[0].text_weaponlevel_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x51EF3);	// 0x51EF3
			strcpy(boydog_info[1].text_weaponlevel_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x51EDF);	// 0x51EDF
			break;
		case 0x06:	// France
			strcpy(boydog_info[0].text_level_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x41C38);			// 0x41C38
			strcpy(boydog_info[1].text_level_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x41C38);			// 0x41C38
			strcpy(boydog_info[0].text_weaponlevel_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x41C2B);	// 0x41C2B
			strcpy(boydog_info[1].text_weaponlevel_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x41C13);	// 0x41C13
			break;
		case 0x08:	// Spain
			strcpy(boydog_info[0].text_level_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x41FFD);			// 0x41FFD
			strcpy(boydog_info[1].text_level_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x41FFD);			// 0x41FFD
			strcpy(boydog_info[0].text_weaponlevel_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x41FED);	// 0x41FED
			strcpy(boydog_info[1].text_weaponlevel_up_str, (char *) tmp_buff+(type == 2 ? param_opt.hsize : 0)+0x41FD4);	// 0x41FD4
			break;
		}
		// 0xF8310-0xF8387 (0x78) = Boy's Level Up handled (function); 0xF84F5-0xF856C (0x78) = Dog's Level Up handled (function)
			// Same for ALL!!; All non-USA: 0xF84E5-0xF855C [-0x10]
		{
			unsigned int offsetBoy = (type == 2 ? param_opt.hsize : 0)+0xF8310;
			unsigned int offsetDog = (type == 2 ? param_opt.hsize : 0)+0xF84F5;
			unsigned int offsetBoyEnd = (type == 2 ? param_opt.hsize : 0)+0xF8387;
			unsigned int offsetDogEnd = (type == 2 ? param_opt.hsize : 0)+0xF856C;
			if (rom_info.region != 0x01) {	// USA
				offsetDog = (type == 2 ? param_opt.hsize : 0)+0xF84E5;
				offsetDogEnd = (type == 2 ? param_opt.hsize : 0)+0xF855C;
			}
			boydog_info[0].level_up_func_startaddr = offsetBoy - (type == 2 ? param_opt.hsize : 0);
			boydog_info[0].level_up_func_endaddr = offsetBoyEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenBoy = offsetBoyEnd - offsetBoy + 1;
			char funcstr1[lenBoy*3];
			char inst_mask1[lenBoy];
			set_inst_mask(inst_mask1, (tmp_buff+offsetBoy), lenBoy);
			bin_to_hexstr(funcstr1, lenBoy*3, (tmp_buff+offsetBoy), lenBoy, inst_mask1);
			strcpy(boydog_info[0].level_up_func_str, funcstr1);
			boydog_info[0].level_up_func_size = lenBoy;
			boydog_info[0].level_up_func_crc = crc32((tmp_buff+offsetBoy), boydog_info[0].level_up_func_size);
			boydog_info[1].level_up_func_startaddr = offsetDog - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].level_up_func_endaddr = offsetDogEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenDog = offsetDogEnd - offsetDog + 1;
			char funcstr2[lenDog*3];
			char inst_mask2[lenDog];
			set_inst_mask(inst_mask2, (tmp_buff+offsetDog), lenDog);
			bin_to_hexstr(funcstr2, lenDog*3, (tmp_buff+offsetDog), lenDog, inst_mask2);
			strcpy(boydog_info[1].level_up_func_str, funcstr2);
			boydog_info[1].level_up_func_size = lenDog;
			boydog_info[1].level_up_func_crc = crc32((tmp_buff+offsetDog), boydog_info[1].level_up_func_size);
		}
		// 0xF8398-0xF84F4 (0x15D) = Boy's stats established (function); 0xF857D-0xF8688 (0x10C) = Dog's stats established (function)
			// All non-USA: 0xF8388-0xF84E4 [-0x10]; 0xF855D-0xF8668 [-0x20]
		{
			unsigned int offsetBoy = (type == 2 ? param_opt.hsize : 0)+0xF8398;
			unsigned int offsetDog = (type == 2 ? param_opt.hsize : 0)+0xF857D;
			unsigned int offsetBoyEnd = (type == 2 ? param_opt.hsize : 0)+0xF84F4;
			unsigned int offsetDogEnd = (type == 2 ? param_opt.hsize : 0)+0xF8688;
			if (rom_info.region != 0x01) {	// USA
				offsetBoy = (type == 2 ? param_opt.hsize : 0)+0xF8388;
				offsetDog = (type == 2 ? param_opt.hsize : 0)+0xF855D;
				offsetBoyEnd = (type == 2 ? param_opt.hsize : 0)+0xF84E4;
				offsetDogEnd = (type == 2 ? param_opt.hsize : 0)+0xF8668;
			}
			boydog_info[0].stats_established_func_startaddr = offsetBoy - (type == 2 ? param_opt.hsize : 0);
			boydog_info[0].stats_established_func_endaddr = offsetBoyEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenBoy = offsetBoyEnd - offsetBoy + 1;
			char funcstr1[lenBoy*3];
			char inst_mask1[lenBoy];
			set_inst_mask(inst_mask1, (tmp_buff+offsetBoy), lenBoy);
			bin_to_hexstr(funcstr1, lenBoy*3, (tmp_buff+offsetBoy), lenBoy, inst_mask1);
			strcpy(boydog_info[0].stats_established_func_str, funcstr1);
			boydog_info[0].stats_established_func_size = lenBoy;
			boydog_info[0].stats_established_func_crc = crc32((tmp_buff+offsetBoy), boydog_info[0].stats_established_func_size);
			boydog_info[1].stats_established_func_startaddr = offsetDog - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].stats_established_func_endaddr = offsetDogEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenDog = offsetDogEnd - offsetDog + 1;
			char funcstr2[lenDog*3];
			char inst_mask2[lenDog];
			set_inst_mask(inst_mask2, (tmp_buff+offsetDog), lenDog);
			bin_to_hexstr(funcstr2, lenDog*3, (tmp_buff+offsetDog), lenDog, inst_mask2);
			strcpy(boydog_info[1].stats_established_func_str, funcstr2);
			boydog_info[1].stats_established_func_size = lenDog;
			boydog_info[1].stats_established_func_crc = crc32((tmp_buff+offsetDog), boydog_info[1].stats_established_func_size);
		}
		// 0xF973E-0xF97B0 (0x73) = Boy's Weapon EXP handling (function); 0xF97C0-0xF9800 (0x41) = Dog's Attack EXP handling (function)
			// All non-USA: 0xF9714-0xF9786 [-0x2A]; 0xF9787-0xF97C7 [-0x39]
		{
			unsigned int offsetBoy = (type == 2 ? param_opt.hsize : 0)+0xF973E;
			unsigned int offsetDog = (type == 2 ? param_opt.hsize : 0)+0xF97C0;
			unsigned int offsetBoyEnd = (type == 2 ? param_opt.hsize : 0)+0xF97B0;
			unsigned int offsetDogEnd = (type == 2 ? param_opt.hsize : 0)+0xF9800;
			if (rom_info.region != 0x01) {	// USA
				offsetBoy = (type == 2 ? param_opt.hsize : 0)+0xF9714;
				offsetDog = (type == 2 ? param_opt.hsize : 0)+0xF9787;
				offsetBoyEnd = (type == 2 ? param_opt.hsize : 0)+0xF9786;
				offsetDogEnd = (type == 2 ? param_opt.hsize : 0)+0xF97C7;
			}
			boydog_info[0].weapon_exp_handling_func_startaddr = offsetBoy - (type == 2 ? param_opt.hsize : 0);
			boydog_info[0].weapon_exp_handling_func_endaddr = offsetBoyEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenBoy = offsetBoyEnd - offsetBoy + 1;
			char funcstr1[lenBoy*3];
			char inst_mask1[lenBoy];
			set_inst_mask(inst_mask1, (tmp_buff+offsetBoy), lenBoy);
			bin_to_hexstr(funcstr1, lenBoy*3, (tmp_buff+offsetBoy), lenBoy, inst_mask1);
			strcpy(boydog_info[0].weapon_exp_handling_func_str, funcstr1);
			boydog_info[0].weapon_exp_handling_func_size = lenBoy;
			boydog_info[0].weapon_exp_handling_func_crc = crc32((tmp_buff+offsetBoy), boydog_info[0].weapon_exp_handling_func_size);
			boydog_info[1].weapon_exp_handling_func_startaddr = offsetDog - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].weapon_exp_handling_func_endaddr = offsetDogEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenDog = offsetDogEnd - offsetDog + 1;
			char funcstr2[lenDog*3];
			char inst_mask2[lenDog];
			set_inst_mask(inst_mask2, (tmp_buff+offsetDog), lenDog);
			bin_to_hexstr(funcstr2, lenDog*3, (tmp_buff+offsetDog), lenDog, inst_mask2);
			strcpy(boydog_info[1].weapon_exp_handling_func_str, funcstr2);
			boydog_info[1].weapon_exp_handling_func_size = lenDog;
			boydog_info[1].weapon_exp_handling_func_crc = crc32((tmp_buff+offsetDog), boydog_info[1].weapon_exp_handling_func_size);
		}
		// 0xFB75A-0xFB77C (0x23) = Attack Calculations Script (function) $08: Boy Standard; 0xFB77D-0xFB79F (0x23) = Attack Calculations Script (function) $0A: Dog Standard
			// All non-USA: 0xFB70B-0xFB72D [-0x4F]; 0xFB72E-0xFB750 [-0x4F]
		{
			unsigned int offsetBoy = (type == 2 ? param_opt.hsize : 0)+0xFB75A;
			unsigned int offsetDog = (type == 2 ? param_opt.hsize : 0)+0xFB77D;
			unsigned int offsetBoyEnd = (type == 2 ? param_opt.hsize : 0)+0xFB77C;
			unsigned int offsetDogEnd = (type == 2 ? param_opt.hsize : 0)+0xFB79F;
			if (rom_info.region != 0x01) {	// USA
				offsetBoy = (type == 2 ? param_opt.hsize : 0)+0xFB70B;
				offsetDog = (type == 2 ? param_opt.hsize : 0)+0xFB72E;
				offsetBoyEnd = (type == 2 ? param_opt.hsize : 0)+0xFB72D;
				offsetDogEnd = (type == 2 ? param_opt.hsize : 0)+0xFB750;
			}
			boydog_info[0].attack_calculation_func_startaddr = offsetBoy - (type == 2 ? param_opt.hsize : 0);
			boydog_info[0].attack_calculation_func_endaddr = offsetBoyEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenBoy = offsetBoyEnd - offsetBoy + 1;
			char funcstr1[lenBoy*3];
			char inst_mask1[lenBoy];
			set_inst_mask(inst_mask1, (tmp_buff+offsetBoy), lenBoy);
			bin_to_hexstr(funcstr1, lenBoy*3, (tmp_buff+offsetBoy), lenBoy, inst_mask1);
			strcpy(boydog_info[0].attack_calculation_func_str, funcstr1);
			boydog_info[0].attack_calculation_func_size = lenBoy;
			boydog_info[0].attack_calculation_func_crc = crc32((tmp_buff+offsetBoy), boydog_info[0].attack_calculation_func_size);
			boydog_info[1].attack_calculation_func_startaddr = offsetDog - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].attack_calculation_func_endaddr = offsetDogEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenDog = offsetDogEnd - offsetDog + 1;
			char funcstr2[lenDog*3];
			char inst_mask2[lenDog];
			set_inst_mask(inst_mask2, (tmp_buff+offsetDog), lenDog);
			bin_to_hexstr(funcstr2, lenDog*3, (tmp_buff+offsetDog), lenDog, inst_mask2);
			strcpy(boydog_info[1].attack_calculation_func_str, funcstr2);
			boydog_info[1].attack_calculation_func_size = lenDog;
			boydog_info[1].attack_calculation_func_crc = crc32((tmp_buff+offsetDog), boydog_info[1].attack_calculation_func_size);
		}
		// 0xE9A86-0xE9B39 (0xB4) = Script (function) - Establishes Main Ring information for both Boy and Dog
			// Same for ALL!!
		{
			unsigned int offset = (type == 2 ? param_opt.hsize : 0)+0xE9A86;
			unsigned int offsetEnd = (type == 2 ? param_opt.hsize : 0)+0xE9B39;
			boydog_info[0].establish_main_ring_info_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[0].establish_main_ring_info_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].establish_main_ring_info_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].establish_main_ring_info_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenFunc = offsetEnd - offset + 1;
			char funcstr1[lenFunc*3];
			char inst_mask1[lenFunc];
			set_inst_mask(inst_mask1, (tmp_buff+offset), lenFunc);
			bin_to_hexstr(funcstr1, lenFunc*3, (tmp_buff+offset), lenFunc, inst_mask1);
			strcpy(boydog_info[0].establish_main_ring_info_func_str, funcstr1);
			boydog_info[0].establish_main_ring_info_func_size = lenFunc;
			boydog_info[0].establish_main_ring_info_func_crc = crc32((tmp_buff+offset), boydog_info[0].establish_main_ring_info_func_size);
			strcpy(boydog_info[1].establish_main_ring_info_func_str, funcstr1);
			boydog_info[1].establish_main_ring_info_func_size = lenFunc;
			boydog_info[1].establish_main_ring_info_func_crc = crc32((tmp_buff+offset), boydog_info[1].establish_main_ring_info_func_size);
		}
		// 0x10C9FC-0x10CA9A (0x9F) = Updates Boy and Dog's Palettes based on Status Outline handling (functions)
			// All non-USA: 0x10CA01-0x10CA9F [+0x5]
		{
			unsigned int offset = (type == 2 ? param_opt.hsize : 0)+0x10C9FC;
			unsigned int offsetEnd = (type == 2 ? param_opt.hsize : 0)+0x10CA9A;
			if (rom_info.region != 0x01) {	// USA
				offset = (type == 2 ? param_opt.hsize : 0)+0x10CA01;
				offsetEnd = (type == 2 ? param_opt.hsize : 0)+0x10CA9F;
			}
			boydog_info[0].update_palettes_status_outline_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[0].update_palettes_status_outline_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].update_palettes_status_outline_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].update_palettes_status_outline_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenFunc = offsetEnd - offset + 1;
			char funcstr1[lenFunc*3];
			char inst_mask1[lenFunc];
			set_inst_mask(inst_mask1, (tmp_buff+offset), lenFunc);
			bin_to_hexstr(funcstr1, lenFunc*3, (tmp_buff+offset), lenFunc, inst_mask1);
			strcpy(boydog_info[0].update_palettes_status_outline_func_str, funcstr1);
			boydog_info[0].update_palettes_status_outline_func_size = lenFunc;
			boydog_info[0].update_palettes_status_outline_func_crc = crc32((tmp_buff+offset), boydog_info[0].update_palettes_status_outline_func_size);
			strcpy(boydog_info[1].update_palettes_status_outline_func_str, funcstr1);
			boydog_info[1].update_palettes_status_outline_func_size = lenFunc;
			boydog_info[1].update_palettes_status_outline_func_crc = crc32((tmp_buff+offset), boydog_info[1].update_palettes_status_outline_func_size);
		}
		// 0x10CA9B-0x10CBB2 (0x118) = Updates last 2 colors of Boy/Dog's Palette for Status Outline (X is Character; alternates between statuses) (function)
			// All non-USA: 0x10CAA0-0x10CBB7 [+0x5]
		{
			unsigned int offset = (type == 2 ? param_opt.hsize : 0)+0x10CA9B;
			unsigned int offsetEnd = (type == 2 ? param_opt.hsize : 0)+0x10CBB2;
			if (rom_info.region != 0x01) {	// USA
				offset = (type == 2 ? param_opt.hsize : 0)+0x10CAA0;
				offsetEnd = (type == 2 ? param_opt.hsize : 0)+0x10CBB7;
			}
			boydog_info[0].update_last2colors_status_outline_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[0].update_last2colors_status_outline_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].update_last2colors_status_outline_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].update_last2colors_status_outline_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenFunc = offsetEnd - offset + 1;
			char funcstr1[lenFunc*3];
			char inst_mask1[lenFunc];
			set_inst_mask(inst_mask1, (tmp_buff+offset), lenFunc);
			bin_to_hexstr(funcstr1, lenFunc*3, (tmp_buff+offset), lenFunc, inst_mask1);
			strcpy(boydog_info[0].update_last2colors_status_outline_func_str, funcstr1);
			boydog_info[0].update_last2colors_status_outline_func_size = lenFunc;
			boydog_info[0].update_last2colors_status_outline_func_crc = crc32((tmp_buff+offset), boydog_info[0].update_last2colors_status_outline_func_size);
			strcpy(boydog_info[1].update_last2colors_status_outline_func_str, funcstr1);
			boydog_info[1].update_last2colors_status_outline_func_size = lenFunc;
			boydog_info[1].update_last2colors_status_outline_func_crc = crc32((tmp_buff+offset), boydog_info[1].update_last2colors_status_outline_func_size);
		}
		// 0x10CDDC-0x10CE14 (0x39) = Establishes current Palette Row to default one, and also updates Boy/Dog's Palette Row accordingly (Y is entity) (function)
			// All non-USA: 0x10CDFF-0x10CE37 [+0x23]
		{
			unsigned int offset = (type == 2 ? param_opt.hsize : 0)+0x10CDDC;
			unsigned int offsetEnd = (type == 2 ? param_opt.hsize : 0)+0x10CE14;
			if (rom_info.region != 0x01) {	// USA
				offset = (type == 2 ? param_opt.hsize : 0)+0x10CDFF;
				offsetEnd = (type == 2 ? param_opt.hsize : 0)+0x10CE37;
			}
			boydog_info[0].establish_current_palette_row_default_and_udpate_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[0].establish_current_palette_row_default_and_udpate_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].establish_current_palette_row_default_and_udpate_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].establish_current_palette_row_default_and_udpate_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenFunc = offsetEnd - offset + 1;
			char funcstr1[lenFunc*3];
			char inst_mask1[lenFunc];
			set_inst_mask(inst_mask1, (tmp_buff+offset), lenFunc);
			bin_to_hexstr(funcstr1, lenFunc*3, (tmp_buff+offset), lenFunc, inst_mask1);
			strcpy(boydog_info[0].establish_current_palette_row_default_and_udpate_func_str, funcstr1);
			boydog_info[0].establish_current_palette_row_default_and_udpate_func_size = lenFunc;
			boydog_info[0].establish_current_palette_row_default_and_udpate_func_crc = crc32((tmp_buff+offset), boydog_info[0].establish_current_palette_row_default_and_udpate_func_size);
			strcpy(boydog_info[1].establish_current_palette_row_default_and_udpate_func_str, funcstr1);
			boydog_info[1].establish_current_palette_row_default_and_udpate_func_size = lenFunc;
			boydog_info[1].establish_current_palette_row_default_and_udpate_func_crc = crc32((tmp_buff+offset), boydog_info[1].establish_current_palette_row_default_and_udpate_func_size);
		}
		// 0x11AC84-0x11ACBC (0x39) = Spell Instruction $44: Boy and Dog's Status Outline Palette Bits set (function)
			// All non-USA: 0x11AC64-0x11AC9C [-0x20]
		{
			unsigned int offset = (type == 2 ? param_opt.hsize : 0)+0x11AC84;
			unsigned int offsetEnd = (type == 2 ? param_opt.hsize : 0)+0x11ACBC;
			if (rom_info.region != 0x01) {	// USA
				offset = (type == 2 ? param_opt.hsize : 0)+0x11AC64;
				offsetEnd = (type == 2 ? param_opt.hsize : 0)+0x11AC9C;
			}
			boydog_info[0].spell_instruction_status_outline_palette_bits_set_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[0].spell_instruction_status_outline_palette_bits_set_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].spell_instruction_status_outline_palette_bits_set_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].spell_instruction_status_outline_palette_bits_set_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenFunc = offsetEnd - offset + 1;
			char funcstr1[lenFunc*3];
			char inst_mask1[lenFunc];
			set_inst_mask(inst_mask1, (tmp_buff+offset), lenFunc);
			bin_to_hexstr(funcstr1, lenFunc*3, (tmp_buff+offset), lenFunc, inst_mask1);
			strcpy(boydog_info[0].spell_instruction_status_outline_palette_bits_set_func_str, funcstr1);
			boydog_info[0].spell_instruction_status_outline_palette_bits_set_func_size = lenFunc;
			boydog_info[0].spell_instruction_status_outline_palette_bits_set_func_crc = crc32((tmp_buff+offset), boydog_info[0].spell_instruction_status_outline_palette_bits_set_func_size);
			strcpy(boydog_info[1].spell_instruction_status_outline_palette_bits_set_func_str, funcstr1);
			boydog_info[1].spell_instruction_status_outline_palette_bits_set_func_size = lenFunc;
			boydog_info[1].spell_instruction_status_outline_palette_bits_set_func_crc = crc32((tmp_buff+offset), boydog_info[1].spell_instruction_status_outline_palette_bits_set_func_size);
		}
		// 0x11BB01-0x11BB26 (0x26) = Updates Boy/Dog's Status Outline according to to status removal (Y is entity) (function)
			// All non-USA: 0x11B9F0-0x11BA15 [-0x111]
		{
			unsigned int offset = (type == 2 ? param_opt.hsize : 0)+0x11BB01;
			unsigned int offsetEnd = (type == 2 ? param_opt.hsize : 0)+0x11BB26;
			if (rom_info.region != 0x01) {	// USA
				offset = (type == 2 ? param_opt.hsize : 0)+0x11B9F0;
				offsetEnd = (type == 2 ? param_opt.hsize : 0)+0x11BA15;
			}
			boydog_info[0].update_status_outline_status_removal_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[0].update_status_outline_status_removal_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].update_status_outline_status_removal_func_startaddr = offset - (type == 2 ? param_opt.hsize : 0);
			boydog_info[1].update_status_outline_status_removal_func_endaddr = offsetEnd - (type == 2 ? param_opt.hsize : 0);
			unsigned int lenFunc = offsetEnd - offset + 1;
			char funcstr1[lenFunc*3];
			char inst_mask1[lenFunc];
			set_inst_mask(inst_mask1, (tmp_buff+offset), lenFunc);
			bin_to_hexstr(funcstr1, lenFunc*3, (tmp_buff+offset), lenFunc, inst_mask1);
			strcpy(boydog_info[0].update_status_outline_status_removal_func_str, funcstr1);
			boydog_info[0].update_status_outline_status_removal_func_size = lenFunc;
			boydog_info[0].update_status_outline_status_removal_func_crc = crc32((tmp_buff+offset), boydog_info[0].update_status_outline_status_removal_func_size);
			strcpy(boydog_info[1].update_status_outline_status_removal_func_str, funcstr1);
			boydog_info[1].update_status_outline_status_removal_func_size = lenFunc;
			boydog_info[1].update_status_outline_status_removal_func_crc = crc32((tmp_buff+offset), boydog_info[1].update_status_outline_status_removal_func_size);
		}
		// [Boy&Dog Info] Write to file
		if ((idf=fopen(fileout_boydoginfo, "wb"))==NULL) {
			printf("[!] Error: unable to open the output file \"%s\"\n", fileout_boydoginfo);
			return -3;
		}
		char buff_line[MAX_BUFF_SIZE];
		snprintf(buff_line, MAX_BUFF_SIZE, "Index%cUnit Name%c", param_opt.spacer, param_opt.spacer);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		for (unsigned int j = 0; j < 99; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "Exp Needed Per Level [%02u]%c", j, param_opt.spacer);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		for (unsigned int j = 0; j < 111; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "Evade Per Level [%03u]%c", j, param_opt.spacer);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		for (unsigned int j = 0; j < 111; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "Hit Per Level [%03u]%c", j, param_opt.spacer);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		for (unsigned int j = 0; j < 111; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "MaxHP Per Level [%03u]%c", j, param_opt.spacer);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		for (unsigned int j = 0; j < 111; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "Base Defense Per Level [%03u]%c", j, param_opt.spacer);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		for (unsigned int j = 0; j < 111; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "Base Attack Per Level [%03u]%c", j, param_opt.spacer);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		for (unsigned int j = 0; j < 111; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "Magic Defense Per Level [%03u]%c", j, param_opt.spacer);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		snprintf(buff_line, MAX_BUFF_SIZE, "[FUNC][ADDR] Level_Up%c[FUNC][SIZE] Level_Up%c[FUNC][STR] Level_Up%c[FUNC][CRC] Level_Up%c[FUNC][ADDR] Stats_Established%c[FUNC][SIZE] Stats_Established%c[FUNC][STR] Stats_Established%c[FUNC][CRC] Stats_Established%c[FUNC][ADDR] Weapon_Exp_Handling%c[FUNC][SIZE] Weapon_Exp_Handling%c[FUNC][STR] Weapon_Exp_Handling%c[FUNC][CRC] Weapon_Exp_Handling%c[FUNC][ADDR] Attack_Calculation%c[FUNC][SIZE] Attack_Calculation%c[FUNC][STR] Attack_Calculation%c[FUNC][CRC] Attack_Calculation%c[FUNC][ADDR] Establish_Main_Ring_Info%c[FUNC][SIZE] Establish_Main_Ring_Info%c[FUNC][STR] Establish_Main_Ring_Info%c[FUNC][CRC] Establish_Main_Ring_Info%c", param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		snprintf(buff_line, MAX_BUFF_SIZE, "[FUNC][ADDR] Update_Palettes_Status_Outline%c[FUNC][SIZE] Update_Palettes_Status_Outline%c[FUNC][STR] Update_Palettes_Status_Outline%c[FUNC][CRC] Update_Palettes_Status_Outline%c[FUNC][ADDR] Update_Last2colors_Status_Outline%c[FUNC][SIZE] Update_Last2colors_Status_Outline%c[FUNC][STR] Update_Last2colors_Status_Outline%c[FUNC][CRC] Update_Last2colors_Status_Outline%c[FUNC][ADDR] Establish_Current_Palette_Row_Default_And_Udpate%c[FUNC][SIZE] Establish_Current_Palette_Row_Default_And_Udpate%c[FUNC][STR] Establish_Current_Palette_Row_Default_And_Udpate%c[FUNC][CRC] Establish_Current_Palette_Row_Default_And_Udpate%c[FUNC][ADDR] Spell_Instruction_Status_Outline_Palette_Bits_Set%c[FUNC][SIZE] Spell_Instruction_Status_Outline_Palette_Bits_Set%c[FUNC][STR] Spell_Instruction_Status_Outline_Palette_Bits_Set%c[FUNC][CRC] Spell_Instruction_Status_Outline_Palette_Bits_Set%c[FUNC][ADDR] Update_Status_Outline_Status_Removal%c[FUNC][SIZE] Update_Status_Outline_Status_Removal%c[FUNC][STR] Update_Status_Outline_Status_Removal%c[FUNC][CRC] Update_Status_Outline_Status_Removal\n", param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer, param_opt.spacer);
		printf("%s", buff_line);
		fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		for(unsigned int j = 0; j < boydog_count; j++) {
			snprintf(buff_line, MAX_BUFF_SIZE, "%d%c\"%s\"%c", boydog_info[j].index, param_opt.spacer, boydog_info[j].unit, param_opt.spacer);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
			for (unsigned int k = 0; k < 99; k++) {
				snprintf(buff_line, MAX_BUFF_SIZE, "%d%c", boydog_info[j].exp_needed_per_level[k], param_opt.spacer);
				printf("%s", buff_line);
				fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
			}
			for (unsigned int k = 0; k < 111; k++) {
				snprintf(buff_line, MAX_BUFF_SIZE, "%d%c", boydog_info[j].evade_per_level[k], param_opt.spacer);
				printf("%s", buff_line);
				fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
			}
			for (unsigned int k = 0; k < 111; k++) {
				snprintf(buff_line, MAX_BUFF_SIZE, "%d%c", boydog_info[j].hit_per_level[k], param_opt.spacer);
				printf("%s", buff_line);
				fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
			}
			for (unsigned int k = 0; k < 111; k++) {
				snprintf(buff_line, MAX_BUFF_SIZE, "%d%c", boydog_info[j].maxhp_per_level[k], param_opt.spacer);
				printf("%s", buff_line);
				fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
			}
			for (unsigned int k = 0; k < 111; k++) {
				snprintf(buff_line, MAX_BUFF_SIZE, "%d%c", boydog_info[j].base_defense_per_level[k], param_opt.spacer);
				printf("%s", buff_line);
				fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
			}
			for (unsigned int k = 0; k < 111; k++) {
				snprintf(buff_line, MAX_BUFF_SIZE, "%d%c", boydog_info[j].base_attack_per_level[k], param_opt.spacer);
				printf("%s", buff_line);
				fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
			}
			for (unsigned int k = 0; k < 111; k++) {
				snprintf(buff_line, MAX_BUFF_SIZE, "%d%c", boydog_info[j].magic_defense_per_level[k], param_opt.spacer);
				printf("%s", buff_line);
				fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
			}
			snprintf(buff_line, MAX_BUFF_SIZE, "\"0x%06X-0x%06X\"%c%u%c\"%s\"%c0x%08X%c\"0x%06X-0x%06X\"%c%u%c\"%s\"%c0x%08X%c\"0x%06X-0x%06X\"%c%u%c\"%s\"%c0x%08X%c\"0x%06X-0x%06X\"%c%u%c\"%s\"%c0x%08X%c\"0x%06X-0x%06X\"%c%u%c\"%s\"%c0x%08X%c", boydog_info[j].level_up_func_startaddr, boydog_info[j].level_up_func_endaddr, param_opt.spacer, boydog_info[j].level_up_func_size, param_opt.spacer, boydog_info[j].level_up_func_str, param_opt.spacer, boydog_info[j].level_up_func_crc, param_opt.spacer, boydog_info[j].stats_established_func_startaddr, boydog_info[j].stats_established_func_endaddr, param_opt.spacer,  boydog_info[j].stats_established_func_size, param_opt.spacer, boydog_info[j].stats_established_func_str, param_opt.spacer, boydog_info[j].stats_established_func_crc, param_opt.spacer, boydog_info[j].weapon_exp_handling_func_startaddr, boydog_info[j].weapon_exp_handling_func_endaddr, param_opt.spacer,  boydog_info[j].weapon_exp_handling_func_size, param_opt.spacer, boydog_info[j].weapon_exp_handling_func_str, param_opt.spacer, boydog_info[j].weapon_exp_handling_func_crc, param_opt.spacer, boydog_info[j].attack_calculation_func_startaddr, boydog_info[j].attack_calculation_func_endaddr, param_opt.spacer,  boydog_info[j].attack_calculation_func_size, param_opt.spacer, boydog_info[j].attack_calculation_func_str, param_opt.spacer, boydog_info[j].attack_calculation_func_crc, param_opt.spacer, boydog_info[j].establish_main_ring_info_func_startaddr, boydog_info[j].establish_main_ring_info_func_endaddr, param_opt.spacer,  boydog_info[j].establish_main_ring_info_func_size, param_opt.spacer, boydog_info[j].establish_main_ring_info_func_str, param_opt.spacer, boydog_info[j].establish_main_ring_info_func_crc, param_opt.spacer);
			snprintf(buff_line, MAX_BUFF_SIZE, "\"0x%06X-0x%06X\"%c%u%c\"%s\"%c0x%08X%c\"0x%06X-0x%06X\"%c%u%c\"%s\"%c0x%08X%c\"0x%06X-0x%06X\"%c%u%c\"%s\"%c0x%08X%c\"0x%06X-0x%06X\"%c%u%c\"%s\"%c0x%08X%c\"0x%06X-0x%06X\"%c%u%c\"%s\"%c0x%08X\n", boydog_info[j].update_palettes_status_outline_func_startaddr, boydog_info[j].update_palettes_status_outline_func_endaddr, param_opt.spacer,  boydog_info[j].update_palettes_status_outline_func_size, param_opt.spacer, boydog_info[j].update_palettes_status_outline_func_str, param_opt.spacer, boydog_info[j].update_palettes_status_outline_func_crc, param_opt.spacer, boydog_info[j].update_last2colors_status_outline_func_startaddr, boydog_info[j].update_last2colors_status_outline_func_endaddr, param_opt.spacer,  boydog_info[j].update_last2colors_status_outline_func_size, param_opt.spacer, boydog_info[j].update_last2colors_status_outline_func_str, param_opt.spacer, boydog_info[j].update_last2colors_status_outline_func_crc, param_opt.spacer, boydog_info[j].establish_current_palette_row_default_and_udpate_func_startaddr, boydog_info[j].establish_current_palette_row_default_and_udpate_func_endaddr, param_opt.spacer,  boydog_info[j].establish_current_palette_row_default_and_udpate_func_size, param_opt.spacer, boydog_info[j].establish_current_palette_row_default_and_udpate_func_str, param_opt.spacer, boydog_info[j].establish_current_palette_row_default_and_udpate_func_crc, param_opt.spacer, boydog_info[j].spell_instruction_status_outline_palette_bits_set_func_startaddr, boydog_info[j].spell_instruction_status_outline_palette_bits_set_func_endaddr, param_opt.spacer,  boydog_info[j].spell_instruction_status_outline_palette_bits_set_func_size, param_opt.spacer, boydog_info[j].spell_instruction_status_outline_palette_bits_set_func_str, param_opt.spacer, boydog_info[j].spell_instruction_status_outline_palette_bits_set_func_crc, param_opt.spacer, boydog_info[j].update_status_outline_status_removal_func_startaddr, boydog_info[j].update_status_outline_status_removal_func_endaddr, param_opt.spacer,  boydog_info[j].update_status_outline_status_removal_func_size, param_opt.spacer, boydog_info[j].update_status_outline_status_removal_func_str, param_opt.spacer, boydog_info[j].update_status_outline_status_removal_func_crc);
			printf("%s", buff_line);
			fwrite(buff_line, 1, strlen(buff_line)*sizeof(char), idf);
		}
		fclose(idf);
	}
	printf("\n");
	////////////////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////////////////
	// TODO: [Misc Info] Section
	{
		// [Misc Info] Read data from ROM
		// [Misc Info] Write to file
	}
	////////////////////////////////////////////////////////////////////////////////////
	// [[End main procedure]]
	
	// Finalize
	printf("[+] SoE ROM Info extraction completed!!!\n");
	return 0;
}
