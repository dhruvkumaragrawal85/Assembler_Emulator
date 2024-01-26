/****************************************************************************
TITLE: Emulator C++ Program																																
AUTHOR:   		Dhruv Kumar Agrawal
ROLL NUMBER:	2101CS26
Declaration of Authorship
This .cpp file, emulator.cpp, is part of the assignment of CS209 at the 
department of Computer Science and Engineering, IIT Patna . 
*****************************************************************************/
#include <bits/stdc++.h>
using namespace std;
//initializes an array called emu_memory with a size of 2^24
int emu_memory[16777216]; //Main memory of code

int SP, PC, A, B;//registers
int index=0, inst_count=0;//index=last position of memory accessed
vector<string> hex_code;  // This stores the code
string dec_To_Hex(unsigned int num);
void readBinaryFile();
void displaymessages();
void emulator();
void memory();
void display_registers();
void instr_set();
void code_runner(int times);

void ldc(int val) {
	B = A;
	A = val;
}
void adc(int val) {
	A += val;
}
void ldl(int offset) {
	B = A;
	A = emu_memory[SP + offset];
}
void stl(int offset) {
	emu_memory[SP + offset] = A;
	A = B;
    if((SP+offset)>index)
    index=SP+offset;//index to hold last position of memory used 
}
void ldnl(int offset) {
	A = emu_memory[A + offset];
}
void stnl(int offset) {
	emu_memory[A + offset] = B;
    if((SP+offset)>index)
    index=SP+offset;//index to hold last position of memory used
}
void add() {
	A += B;
}
void sub() {
	A = B - A;
}
void shl() {
	A = B << A;
}
void shr() {
	A = B >> A;
}
void adj(int val) {
	SP += val;
}
void a2sp() {
	SP = A;
	A = B;
}
void sp2a() {
	B = A;
	A = SP;
}
void call(int offset) {
	B = A;
	A = PC;
	PC += offset;
}

void Return() {
	PC = A;
	A = B;
}
void brz(int offset) {
	if (A == 0) PC += offset;
}
void brlz(int offset) {
	if (A < 0) PC += offset;
}
void br(int offset) {
	PC += offset;
} 
int main()
{
    readBinaryFile();
    cout<<"--------------------------\n--------------------------\n";
    cout << "Emulator running" << endl;
    for (;;)
    {
        emulator();
    }
}
void readBinaryFile() {
    // Prompt user to enter filename
    string filename;
    cout << "Enter filename (e.g. assembled_Code.o): " << endl;
    cin >> filename;

    // Attempt to open file for reading
    ifstream input_file(filename, ios::in | ios::binary);
    if (!input_file) {
        cerr << "Error: Could not open file " << filename << endl;
        cout<<"Please provide a valid file.\n";
        cout<<"--------------------------\n--------------------------\n";
        main();
        return;
    }

    // Read contents of file and store in memory
    unsigned int data;
    int position = 0;
    while(input_file.read((char*)&data, sizeof(int))) { 
        // cout<<data<<endl;
        emu_memory[position++] = data;
        index=position;
        hex_code.push_back(dec_To_Hex(data));
    }

    // Close file
    input_file.close();
    //    for(int i=0; i<hex_code.size(); i++)
    // {
    //     cout<<hex_code[i]<<endl;
    // }
}
string dec_To_Hex(unsigned int num) {
    const int HEX_DIGITS = 8; // maximum number of hexadecimal digits
    const char HEX_TABLE[] = "0123456789ABCDEF"; // lookup table for hexadecimal conversion

    string hex = ""; // result string to be returned

    // Convert decimal number to hexadecimal
    while (num > 0 && hex.length() < HEX_DIGITS) {
        int digit = num % 16;
        hex = HEX_TABLE[digit] + hex;
        num /= 16;
    }

    // Add leading zeros to fill up to 8 digits
    while (hex.length() < HEX_DIGITS) {
        hex = "0" + hex;
    }

    return hex;
}


void emulator()
{
    // Interactive emulator loop
    cout << "Select function:" << endl;
    cout << "1) memory dump: -dump" << endl;
    cout << "2) Show registers values: -reg" << endl;
    cout << "3) Show instruction set using : -iset" << endl;
    cout << "4) Emulate code one line at time: using instruction: -t" << endl;
    cout << "5) Emulate complete code -run" << endl;
    cout << "Enter 0 to exit:" << endl;
    string command;
    cin >> command;
    switch (command[0]) {
        case '0':
            exit(0);
            break;
        case '-':
        {
            if (command == "-dump")
            {
                memory();
            }
            else if (command == "-t")
            {
                code_runner(1);
            }
             else if (command == "-reg")
            {
                display_registers();
            }
            else if (command == "-iset")
            {
                instr_set();
            }
            else if (command == "-run")
            {
               code_runner(1<<25);
            }
            else
            {
                cout << "Enter correct instruction" << endl;
            }
        }
            break;
        default:
            cout << "Enter correct instruction" << endl;
            break;
    }
    return;
}
void memory()
{
    int t=0;
    //for printing memory_dump
    while(t<=index)
    {
        if((t!=0) && (emu_memory[t]==0 && emu_memory[t+1]==0) &&
                     (emu_memory[t+2]==0 && emu_memory[t+3]==0) &&
                     (emu_memory[t+4]==0 && emu_memory[t+5]==0) &&
                     (emu_memory[t+6]==0 && emu_memory[t+7]==0) )
                     {
                        cout<<"....\n";
                        t+=8;
                        continue;
                     }
        cout << dec_To_Hex(t) << " ";
        for (int j = t; j < t+4; j++) {
            cout << dec_To_Hex(emu_memory[j]) << " ";
        }
        cout << endl;
            t+=4;
    }
}
void display_registers()
{
    //Displaying register contents
    cout<<"A: "<<" "<<dec_To_Hex(A);
    cout<<" B: "<<" "<<dec_To_Hex(B)<<endl;
    cout<<"SP:"<<" "<<dec_To_Hex(SP);
    cout<<" PC:"<<" "<<dec_To_Hex(PC)<<endl;
}
void instr_set()
{
    // print table header
    cout << "OpCode " << setw(12) << "Mnemonic " << "Operand" << endl;
    
    // print table rows
    cout << "0" << setw(14) << "ldc " << setw(10) << "value" << endl;
    cout << "1" << setw(14) << "adc " << setw(10) << "value" << endl;
    cout << "2" << setw(14) << "ldl " << setw(10) << "value" << endl;
    cout << "3" << setw(14) << "stl " << setw(10) << "value" << endl;
    cout << "4" << setw(14) << "ldnl" << setw(10) << "value" << endl;
    cout << "5" << setw(14) << "stnl" << setw(10) << "value" << endl;
    cout << "6" << setw(14) << "add " << setw(10) << "" << endl;
    cout << "7" << setw(14) << "sub " << setw(10) << "" << endl;
    cout << "9" << setw(14) << "shr " << setw(10) << "" << endl;
    cout << "10" << setw(13) << "adj " << setw(10) << "value" << endl;
    cout << "11" << setw(13) << "a2sp" << setw(11) << "" << endl;
    cout << "12" << setw(13) << "sp2a" << setw(11) << "" << endl;
    cout << "13" << setw(13) << "call" << setw(11) << "offset" << endl;
    cout << "14" << setw(15) << "return" << setw(11) << "" << endl;
    cout << "15" << setw(13) << "brz " << setw(11) << "offset" << endl;
    cout << "16" << setw(13) << "brlz" << setw(11) << "offset" << endl;
    cout << "17" << setw(13) << "br  " << setw(11) << "offset" << endl;
    cout << "18" << setw(13) << "HALT" << setw(11) << "" << endl;

    return;
}

void code_runner(int times)
{
    while(times--)
    {
        inst_count++;
        if(PC>hex_code.size()||inst_count>3e7)
        {
            cout<<"Segmentation fault\n"<<endl;
            return;
        }
        string current=hex_code[PC];
        if(stoi(current.substr(6,2),0, 16)==18)//encountered halt encountered
        {
            cout<<"HALT found.\n"<<inst_count<<" instructions executed\n";
            return;
        }
        //taking substring
        int op_code=stoi(current.substr(6,2),0, 16);
        int operand=stoi(current.substr(0,6),0, 16);
          //For adjusting negative values
          if(operand >= (1 << 23)){
            operand -= (1 << 24);
        }
        //Calling different opcodes;
        if (op_code == 0)
            ldc(operand);
        if (op_code == 1)
            adc(operand);
        if (op_code == 2)
            ldl(operand);
        if (op_code == 3)
            stl(operand);
        if (op_code == 4)
            ldnl(operand);
        if (op_code == 5)
            stnl(operand);
        if (op_code == 6)
            add();
        if (op_code == 7)
            sub();
        if (op_code == 8)
            shl();
        if (op_code == 9)
            shr();
        if (op_code == 10)
            adj(operand);
        if (op_code == 11)
            a2sp();
        if (op_code == 12)
            sp2a();
        if (op_code == 13)
            call(operand);
        if (op_code == 14)
            Return();
        if (op_code == 15)
            brz(operand);
        if (op_code == 16)
            brlz(operand);
        if (op_code == 17)
            br(operand);
               PC++;
        if(times==0)
        {display_registers();
        return;}
        if(PC==hex_code.size())
        return;
    }
    
}

