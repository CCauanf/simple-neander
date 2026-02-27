void load_accumulator();
void control_unit(unsigned char instruction);
int mem_alloc(unsigned char* mem_req);
void print_memory(unsigned char* main_memory);
void print_state();
unsigned char fetch_instruction(unsigned char pc);
void execute_program(unsigned char breakpoint, char mode);
void store_accumulator();
void address_fetch();
void jump_address();
unsigned char alu();
void condicional_jump(char mode);
void alu_operations(unsigned char operation);
void get_data_from_address(unsigned char address);


