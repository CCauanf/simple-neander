#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "neander.h"

#define NOP   0x00
#define STA   0x10
#define LDA   0x20
#define ADD   0x30 
#define OR    0x40
#define AND   0x50
#define NOT   0x60
#define JMP   0x80
#define JN    0x90
#define JZ    0xA0
#define HLT   0xF0


//todo: ALU, JMP

struct mainRegisters{
  unsigned char program_counter;
  unsigned char accumulator;
  unsigned char instruction_reg;
  unsigned char mem_ad_reg;
  unsigned char mem_data_reg;
  unsigned char flags : 2;
};
typedef struct mainRegisters mregs;

//NEANDER
unsigned char main_memory[0xFF] = {0x00};
mregs registradores;
int instruction_access = 0;
int memory_access = 0;



void store_accumulator(){
	//Fetch de endereço
	get_data_from_address(registradores.program_counter);
	printf("			ciclo x - fetch de endereco\n");
	print_state();
	registradores.program_counter++;


	//Execute
	main_memory[registradores.mem_data_reg]	= registradores.accumulator; 
}

void load_accumulator(){
	//Fetch de endereço
	get_data_from_address(registradores.program_counter);
	printf("			ciclo x - fetch de endereco\n");
	print_state();
	registradores.program_counter++;


	//Execute
	get_data_from_address(registradores.mem_data_reg);
	registradores.accumulator = registradores.mem_data_reg;
}

void jump_address(){
	//Fetch de endereço
	get_data_from_address(registradores.program_counter);
	printf("			ciclo x - fetch de endereco\n");
	print_state();
	registradores.program_counter++;


	//Execute
	registradores.program_counter = registradores.mem_data_reg;
}


void condicional_jump(char mode){
	//Fetch de endereço
	get_data_from_address(registradores.program_counter);
	printf("			ciclo x - fetch de endereco\n");
	print_state();
	registradores.program_counter++;


	switch(mode){
		case 'n':
			if((registradores.flags & (unsigned char)0x2) != 0x0)
				registradores.program_counter = registradores.mem_data_reg;
		break;

		case 'z':
			if((registradores.flags & (unsigned char)0x1) != 0x0)
				registradores.program_counter = registradores.mem_data_reg;
		break;
	}

}

void alu_operations(unsigned char operation){
	if(operation == NOT){
		registradores.accumulator = ~registradores.accumulator;
		return;
	}
	
	//Fetch de endereço
	get_data_from_address(registradores.program_counter);
	printf("			ciclo x - fetch de endereco\n");
	print_state();
	registradores.program_counter++;


	//Execute
	get_data_from_address(registradores.mem_data_reg);

	switch(operation){
		case ADD:
			registradores.accumulator += registradores.mem_data_reg;
		break;
			
		case OR:
			registradores.accumulator |= registradores.mem_data_reg;
		break;

		case AND:
			registradores.accumulator &= registradores.mem_data_reg;
		break;

	}

}

void control_unit(unsigned char instruction){
	switch(instruction){
		case NOP:
			break;

		case STA:
			store_accumulator();
			break;

		case LDA:
			load_accumulator();
			break;

		case ADD:
			alu_operations(ADD);
			break;

		case OR:
			alu_operations(OR);
			break;

		case AND:
			alu_operations(AND);
			break;
			
		case NOT:
			alu_operations(NOT);
			break;

		case JMP:
			jump_address();
			break;

		case JN:
			condicional_jump('n');
			break;

		case JZ:
			condicional_jump('z');
			break;

		case HLT:
			break;

		default:
			printf("ERRO! INSTRUÇÃO DESCONHECIDA!");
			break;
	}

	if(registradores.accumulator >= 0x80)
		registradores.flags |= 2;
	else 
		registradores.flags &= 1;

	if(registradores.accumulator == 0)
		registradores.flags |= 1;
	else
		registradores.flags &= 2;

	printf("			Ciclo x - Execute de instrucao\n");
	print_state();
}

int mem_alloc(unsigned char* mem_req){
  main_memory[mem_req[0]] = mem_req[1];
  return 1;
}

void print_memory(unsigned char* main_memory){
  for(int i = 0; i <= (0xFF)/2; i++){
	printf("%02x : %02hhx           %02x : %02hhx\n", 
			i, main_memory[i], i + 0x80, main_memory[i + 0x80]);
  }

}


void print_state(){
	printf("		PC: %-2hhx, AC: %-2hhx, IR: %-2hhx, FLG: %-2hhx, MDR: %-2hhx, MAR: %-2hhx\n",
			registradores.program_counter,
			registradores.accumulator,
			registradores.instruction_reg,
			registradores.flags,
			registradores.mem_data_reg,
			registradores.mem_ad_reg);

	printf("			MA: %i, IA: %i\n",
			memory_access, instruction_access);
	printf("\n");
}

void get_data_from_address(unsigned char address){
	//povoa data reg com o conteúdo do endereço address,
	//colocando address em adress reg antes
	registradores.mem_ad_reg = address;	
	registradores.mem_data_reg = main_memory[registradores.mem_ad_reg];
	memory_access++;


}


void execute_program(unsigned char breakpoint, char mode){
	printf("			Programa rodando :)\n\n");
	printf("				Estado inicial\n");
	print_state();

	while(registradores.program_counter <= 0xFF &&
			registradores.program_counter != breakpoint){
		
		//Fetch instruction
		get_data_from_address(registradores.program_counter);
		registradores.instruction_reg = registradores.mem_data_reg;
		instruction_access++;

		registradores.program_counter++;

		printf("			ciclo x - fetch de instrucao\n");
		print_state();


		//DECODE
		if(registradores.instruction_reg == HLT)
			return;
		control_unit(registradores.instruction_reg);

		if(mode == 'p')
			system("pause");
		if(mode == 's')
			sleep(1);
	};
	  
}

int main(){
  unsigned char breakpoint;

  unsigned char mem_req[2];

  do{
	system("clear || cls");
	print_memory(main_memory);
	fflush(stdin);

	printf("Digite pos + dado a ser alocado: ");
	scanf(" %hhx %hhx", &mem_req[0], &mem_req[1]);

	if(mem_alloc(mem_req) == 1){
	  printf("%02x escrito na posicao %02x!", mem_req[1], mem_req[0]);
	  sleep(1);
	}
  }while(mem_req[0] != 0xFF || mem_req[1] != 0xFF);

  system("clear || cls");

  printf("Defina o breakpoint: ");
  scanf(" %hhx", &breakpoint);

  registradores.program_counter = 0x00;
  registradores.accumulator = 0x00;
  registradores.instruction_reg = 0x00;
  registradores.mem_ad_reg = 0x00;
  registradores.mem_data_reg = 0x00;
  registradores.flags = 0x00;

  char exec_mode;

  printf("Selecione o modo de execucao (s, f, p): ");
  scanf(" %c", &exec_mode);

	system("clear || cls");
  execute_program(breakpoint, exec_mode);

	print_memory(main_memory);


  return 0;
}
