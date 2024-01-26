/****************************************************************************
TITLE: Assembler C++ Program																																
AUTHOR:   		Dhruv Kumar Agrawal
ROLL NUMBER:	2101CS26
Declaration of Authorship
This .cpp file, assembler.cpp, is part of the assignment of CS209 at the 
department of Computer Science and Engineering, IIT Patna . 
*****************************************************************************/
#include <bits/stdc++.h>
using namespace std;
///
///
///
vector<string> lines;                        //to store the code in clean internal form
vector<pair<long int, string>> errors;       // to hold the errors in the code
map<string, pair<string, int>> op_code = {   // Storing OPcodes
        make_pair("data", make_pair("", 1)),
        make_pair("ldc", make_pair("00", 1)),
        make_pair("adc", make_pair("01", 1)),
        make_pair("ldl", make_pair("02", 2)),
        make_pair("stl", make_pair("03", 2)),
        make_pair("ldnl", make_pair("04", 2)),
        make_pair("stnl", make_pair("05", 2)),
        make_pair("add", make_pair("06", 0)),
        make_pair("sub", make_pair("07", 0)),
        make_pair("shl", make_pair("08", 0)),
        make_pair("shr", make_pair("09", 0)),
        make_pair("adj", make_pair("0A", 1)),
        make_pair("a2sp", make_pair("0B", 0)),
        make_pair("sp2a", make_pair("0C", 0)),
        make_pair("call", make_pair("0D", 2)),
        make_pair("return", make_pair("0E", 0)),
        make_pair("brz", make_pair("0F", 2)),
        make_pair("brlz", make_pair("10", 2)),
        make_pair("br", make_pair("11", 2)),
        make_pair("HALT", make_pair("12", 0)),
        make_pair("SET", make_pair("", 1))
    // No opcode for data and set
    // 0=No operand(eg.SET), 1=1 operand(eg.ldc), 2=1 operand & offset(eg.ldl)
    }; 
map<string, int> label_hold;				//Storing the labels declared and their line numbers
struct Instruction {
    /*Storing the asm code in a tabular form
    instruction label | instruction mnemonic | instruction operand | Operand type | IF label is present there*/
    string instLabel;
    string instMnemonic;
    string instOperand;
    int oprType;
    bool isLabelPresent;
};
vector<Instruction> instData;               //Store instruction in tabular form
vector<int> program_counter;				//This vector maintains Program counter on every line
bool halt_present = false;                  //Flag to check if halt is there
vector<pair<int, string>> mach_Code;	    //Stores the machine code 
///
///
///
string process_1(string hold);
string process_2(string hold, int line_num);
void add_error(int line_num, string type);
void first_pass();
void label_store();
int valid_name(string s);
void implementSET();
void separateDataSegment();
void fill_table();
void add_Data(int x, string temp1, string temp2, string temp3, int optype);
int OP_Type(string s);
void second_pass();
bool errors_check();
bool is_Hexa(string s);
bool is_Dec(string s);
bool is_Octal(string s);
void write_files();
void print_messages();
string appendZeros(string temp, int l = 6) {
    int n = temp.size();
    if (n >= l) {
        return temp;
    } else {
        string zeros(l - n, '0');
        return temp + zeros;
    }
}

///
///
///
int main() {
    string inputfilename;
    cout << "Enter file to Assemble: ";
    cin >> inputfilename;

    ifstream input_file(inputfilename);// input_file stream object; ifstream represents the input file stream and is used to read information to file;
    if (!input_file.is_open()) //Program terminates if there is some issue in opening the file
    {
        cout << "Cannot open or find file: " << inputfilename << endl;
        return 1;
    }


    string line;
    while (getline(input_file, line)) {
        string hold = process_1(line);
        hold = process_2(hold, lines.size());
        lines.push_back(hold);
    }

    // cout << "Contents of file:" << endl;
    // for (const auto& line : lines) {
    //     cout << line << endl;
    // }

    //  cout << "Contents of error file:" << endl;
    // for(auto to: errors){
	// 	cout << to.second << endl;
	// }

    first_pass();
    if(errors_check()){
		second_pass();
        write_files();
        print_messages();
	}
    return 0;

}
/// 
///
/// 
string process_1(string hold) {
    string hold_temp = "";

    // Remove trailing whitespace
    while (!hold.empty() && (hold.back() == ' ' || hold.back() == '\t')) {
        hold.pop_back();
    }

    // Remove leading whitespace
    reverse(hold.begin(), hold.end());             
    while (!hold.empty() && (hold.back() == ' ' || hold.back() == '\t')) 
    {
        hold.pop_back();
    }
    reverse(hold.begin(), hold.end());

    // we dont need comments in internal form
    for (char c : hold) {
        if (c == ';') {
            break;
        }
        hold_temp += c;
    }

    return hold_temp;
}
///
///
///
string process_2(string hold, int line_num)
{
/*function is used to handle spaces in between tokens in an input string,
and to add a space in front of label declarations*/
    string hold_temp = "";
    bool label_declaration = false;
    for (int i = 0; i < hold.size(); i++)
    {
        char c = hold[i];
        if (c == ':')
        { // Check if label declaration
            label_declaration = true;
            hold_temp += ':';
            if (i + 1 < hold.size() && hold[i + 1] != ' ')
            {
                hold_temp += ' ';
            }
        }
        else if (isspace(c))
        { // Check if whitespace character
            if (label_declaration)//add space infront of a label declaration
            {
                hold_temp += ' ';
            }
            else if(i+1!=hold.size())
            {
                hold_temp += ' ';
            }
            while (i + 1 < hold.size() && isspace(hold[i + 1]))//move to next non-space character index
            {
                i++;
            }
        }
        else
        { // Add non-whitespace character to output string
            hold_temp += c;
            label_declaration = false;
        }
    }

      long int spaces = 0;
    for (long int k = 0; k < hold_temp.size(); k++)
    {
        if (hold_temp[k] == ' ')
            spaces++;
    }
    /*checking number of spaces in a line
    loop: br loop && var1: data 0 are valid lines space wise
    loop: br loop loop is not*/
    if (spaces >= 3)
        add_error(line_num + 1, "Syntax error detected");
    return hold_temp;
}
///
///
///
void add_error(int line_num, string type) {
    // Construct error message
    string msg = "Error at line: " + to_string(line_num) + " -- Type: " + type;
    
    // Add error message to errors vector
    errors.push_back({line_num + 1, msg});
}
///
///
///
void first_pass()
{
    label_store();
    if(errors.empty())
    implementSET();		
    int data_size = (int)(lines.size());
    instData.resize(data_size);
    program_counter.resize(data_size);
    separateDataSegment();
    fill_table();
}
/// 
///
///
void label_store()
{
    /*this function reads internal form code in lines vector,
    searches for labels and then stores them in label_hold vector*/
    int line_num=0;
    while(line_num!=lines.size())//iterating on different lines in the vector
    {
        string current="";
        for(int t=0; t<lines[line_num].size(); t++)//iterating on every character of a line in the vector
        {
            if(lines[line_num][t]==':')
            {
                int flag=valid_name(current);//function to check if the label name used is valid or not
                if(flag==0)
                {
                    add_error(line_num+1, "Invalid label name detected");
                    break;
                }
                if(label_hold.count(current))//we check if any label has been declared multiple times, this prompts an error
                {
                    /*A label may be used multiple times in combination with SET or data. So we 'continue' when we encounter 
                    them*/
                    if(lines[line_num].size()>t+4)
                    {
                        if(lines[line_num].substr(t+2, 3)=="SET")
                        {
                            continue;
                        }
                    }
                    if(lines[line_num].size()>t+5)
                    {
                        if(lines[line_num].substr(t+2, 4)=="data")
                        {
                            label_hold[current]=line_num;
                            continue;
                        }
                    }
                    add_error(line_num+1, "Multiple label declaration detected");
                }
                if(lines[line_num].size()>t+4)
                {
                    if(lines[line_num].substr(t+2, 3)=="SET")
                    {
                        /*If the current declaration is a SET declaration,
                         we add a negative line number to indicate that it is a SET label*/
                        label_hold[current]=-line_num;
                        continue;
                    }
                }
                label_hold[current]=line_num;
                break;
            }
            current+=lines[line_num][t];
        }
        line_num++;
    }
    // 	for(map<string, int>::iterator it=label_hold.begin(); it!=label_hold.end(); ++it)
	// {
	// 	cout<<(*it).first<<" "<<(*it).second<<endl;
	// }
	// cout<<endl;
}
///
///
///
int valid_name(string s) {
    if (s.empty() || !isalpha(s[0]) && s[0] != '_') // a valid name cannot start with a digit
    {
        return 0;
    }
    for (size_t i = 1; i < s.size(); i++) // a valid name can contain 1~9, a~z, A~Z, _
    {
        if (!isalnum(s[i]) && s[i] != '_') {
            return 0;
        }
    }
    return 1;
}
/// 
///
///
void implementSET(){
    vector<string> modifiedCode; // create a new vector to hold modified code
    int i=0;
    while(i!=lines.size())
    {
        string curLine;
        bool isSetInstruction = false;
        for(int j = 0; j < lines[i].size(); j++){
            curLine += lines[i][j]; // copy each character of the current line to curLine
            if(lines[i][j] == ':'){ // check if the current character is a colon
                curLine.pop_back(); // remove the colon from curLine
                if(lines[i].size() > j + 5 and lines[i].substr(j + 2, 3) == "SET"){ // check if the line contains a SET instruction
                    isSetInstruction = true;
                    ///
                    ///
                    /*We check if the label with SET has been previously defined
                    if it hasn't been we just implement it with the data instruction
                    else we need to implement it using other mnemonics*/
                    ///
                    ///
                    if (abs(label_hold[curLine]) == i)
                    {
                        label_hold[curLine] = modifiedCode.size() - 1;                                                                    // update the label index
                        modifiedCode.push_back(lines[i].substr(0, j + 1) + " data " + lines[i].substr(j + 6, lines[i].size() - (j + 6))); // add the SET instruction to the modified code
                    }
                    else
                    {
                        // generating the equivalent instructions for the SET instruction and add them to the modified code
                        if (lines[i].size() <= j + 5)
                        {
                            break;
                        }
                        /*
                        1)We set the stack pointer to a random memory
                        2)Storing value currently stored in A using stl -1
                          memory[SP-1]=A;
                          A=B;
                        3)Storing value currently stored in B using stl 0 
                          memory[SP+0]=A(originally B);
                        */
                        modifiedCode.push_back("adj 1000"); // stack pointer at random memory
                        modifiedCode.push_back("stl -1");   // Here to load A
                        modifiedCode.push_back("stl 0");    // Here B
                        /*
                        4)Store the value specified in B and the location of the label in A
                          if there's val:
                                     ...
                                     val: set 66
                          ldc 66
                          ldc val
                        5)Then set mem[val+0]=66 using stnl
                        */
                        modifiedCode.push_back("ldc " + lines[i].substr(j + 6, lines[i].size() - (j + 6)));
                        modifiedCode.push_back("ldc " + curLine.substr(0, j));
                        modifiedCode.push_back("stnl 0"); 
                        /*
                        6)restore original A & B
                        7)restore original SP
                        */
                        modifiedCode.push_back("ldl 0");
                        modifiedCode.push_back("ldl -1");
                        modifiedCode.push_back("adj -1000"); // Adjust stack ptr
                    }
                    break;
                }
            }
        }
        if(!isSetInstruction and !lines[i].empty())
            {modifiedCode.push_back(lines[i]);} // add the current line to the modified code if it is not a SET instruction
            i++;
    }
    lines = modifiedCode; // update the original code with the modified code
    // cout<<endl;
	// for(map<string, int>::iterator it=label_hold.begin(); it!=label_hold.end(); ++it)
	// {
	// 	cout<<(*it).first<<" "<<(*it).second<<endl;
	// }
	// cout<<endl;
	// 	cout<<endl;
	// 	for(int i = 0; i < (int)lines.size(); i++)
	// 	{
	// 		cout<<lines[i]<<endl;
	// 	}
	// cout<<endl;
}
/// 
///
///
void separateDataSegment() {
    vector<string> instructions;
    vector<string> dataSegment;

    for (const auto& line : lines) {//range-based for loops to iterate over the lines vector
        bool isDataSegment = false;
        for (int i = 0; i < line.size(); i++) {//searching for "data" instruction
            if (line.substr(i, 4) == "data" && i + 4 < line.size()) {
                dataSegment.push_back(line);
                isDataSegment = true;
                break;
            }
            if (line.back() == ':' && i + 1 < lines.size() && lines[i + 1].substr(0, 4) == "data") {
                dataSegment.push_back(line);
                isDataSegment = true;
                break;
            }
        }
        if (!isDataSegment) {
            instructions.push_back(line);
        }
    }
    instructions.insert(instructions.end(), dataSegment.begin(), dataSegment.end());//Inserting the elements of datasegment at end of instructions
    lines = instructions;
    // cout<<endl;
	// 	for(int i = 0; i < (int)lines.size(); i++)
	// 	{
	// 		cout<<lines[i]<<endl;
	// 	}
	// cout<<endl;
}
///  
///
///
void fill_table()
{
    int temp_pc=0;
    int n=0;
   
    for(;n<(int)lines.size(); n++)
    { 
        // Initialize an array to store the label, mnemonic, operand, and operand type for the current line
        string hold[10]={"","","",""};
        string temp="";
        // Initialize pointer to 1 (hold[0] is the label)
        int index=1;
        for(int j=0; j<(int)lines[n].size(); j++)
        {
            if(lines[n][j]==':')
            {
                hold[0]=temp;
                temp="";
                j++;
                continue;
            }
            else if(lines[n][j]==' ')
            {
                hold[index++]=temp;
                temp="";
                continue;
            }
            temp+=lines[n][j];
            if(j==(int)lines[n].size()-1)
            hold[index++]=temp;
        }
        if(!hold[1].empty())
        {
            instData[n].isLabelPresent=true;
        }
        else
        {
            instData[n].isLabelPresent=false;
        }
        // If the mnemonic is "HALT", set a flag indicating that the program contains a HALT instruction
        if(hold[1]=="HALT")
        halt_present=true;
     	if(!hold[0].empty())// If a label is present, add it to the "labels" map with the current program counter value
			label_hold[hold[0]] = temp_pc;
         // Set the program counter value for the current line   
		program_counter[n] = temp_pc;
       
        // If there are no operands, the instruction is a zero operand instruction
        if(index==1)
        {
            add_Data(n, hold[0], "", "", 0);
            continue;
        }
        temp_pc++;
        // Cheking if the mnemonic is not a valid opcode
          if(!op_code.count(hold[1])){
			add_error(n + 1, "Invalid Mnemonic detected");
			continue;
		}
        //If the number of operands is not valid for this opcode, generate an error message
        if(min(op_code[hold[1]].second, 1) != index - 2){
			add_error(n + 1, "Invalid OPCode-Syntax combination found");
			continue;
		}
		add_Data(n, hold[0], hold[1], hold[2], OP_Type(hold[2]));
        // If the operand is a label and it is not found in the "labels" map, generate an error
		if(instData[n].oprType== 1 and !label_hold.count(instData[n].instOperand)){
			add_error(n + 1, "Specified label or data variable does not exist");
		}
        //if the operand type is invalid
		else if(instData[n].oprType == -1){
			add_error(n + 1, "Invalid number detected");
		}
        
       
    }
    // for (int  i = 0; i < instData.size(); i++)
	// {
	// 	cout<<instData[i].instLabel<<" "<<" "<<instData[i].instMnemonic<<" "<<instData[i].instOperand<<" "<<instData[i].oprType<<" "<<instData[i].isLabelPresent<<endl;
	// }
}
///
///
///
void add_Data(int x, string temp1, string temp2, string temp3, int optype)
{ // storing instructions in data vector in tabular and efficient way
    instData[x].oprType = optype;
    instData[x].instLabel = temp1;
    instData[x].instMnemonic = temp2;
    instData[x].instOperand = temp3;
}
///
///
///
int OP_Type(string hold) {
    // if (s.empty()) {
    //     return 0;
    // }

    // if (s[0] == '+' or s[0] == '-')
    // {
    //     reverse(s.begin(), s.end());
    //     s.pop_back();
    //     reverse(s.begin(), s.end());
    // }
    // if (s.empty())
    //     return -1;

    // // Check if the operand is a hexadecimal value
    // if (s.size() > 2 && s[0] == '0' && s[1] == 'x') {
    //     if (is_Hexa(s.substr(2))) {
    //         return 16; // Hexadecimal value
    //     }
    // }
    
    // // Check if the operand is an octal value
    // if (s.size() > 1 && s[0] == '0') {
    //     if (is_Octal(s.substr(1))) {
    //         return 8; // Octal value
    //     } 
    // }
    
    // // Check if the operand is a decimal value
    // if (is_Dec(s)) {
    //     return 10; // Decimal value
    // }
    // if(valid_name(s))
    // {
    //     return 1;
    // }
    // // Operand type cannot be determined
    // return -1;
    if (hold.empty())
        return 0;
    if (hold[0] == '+' || hold[0] == '-')
    {
        reverse(hold.begin(), hold.end());
        hold.pop_back();
        reverse(hold.begin(), hold.end());
    }
    if (hold.empty())
        return -1;
    if (is_Dec(hold))
        return 10;
    if (is_Octal(hold))
        return 8;
    if (is_Hexa(hold))
        return 16;
    if (valid_name(hold))
        return 1;
    return -1;
}
/// 
///
///
bool is_Dec(string s) {
    //check wether number is decimal or not  
    if (s.empty()) {
        return false;
    }
    
    // Check if the first character is a sign
    if (s[0] == '+' || s[0] == '-') {
        s = s.substr(1); // Remove the sign character
    }
    
    // Check if all remaining characters are decimal digits
    for (char c : s) {
        if (c < '0' || c > '9') {
            return false;
        }
    }
    
    return true;
}
/// 
///
///
bool is_Octal(string s) {
    //check whether string is octal or not
    if (s.empty() || s[0] != '0') {
        return false;
    }
    for (int i = 1; i < s.size(); i++) {
        if (s[i] < '0' || s[i] > '7') {//check if there are any invalid digits
            return false;
        }
    }
    return true;
}
///
///
///
bool is_Hexa(string s) {
    // Check if string starts with "0x" or "0X"
    if (s.size() < 3 || s.substr(0, 2) != "0x" && s.substr(0, 2) != "0X") {
        return false;
    }
    
    // Check if all characters are valid hexadecimal digits
    for (int i = 2; i < s.size(); i++) {
        if (!isxdigit(s[i])) {
            return false;
        }
    }
    
    return true;
}
/// 
///
///
string dec_To_Hex(int number, int add = 24) {
    if (add == 32) {
        /* If the 'add' parameter is 32, convert the unsigned integer value to a 32-bit hexadecimal string representation.
         We use bit shifting and masking to extract the individual hexadecimal digits
         from the input number*/
        unsigned int num = number;
        char hexNum[9]; // Allocate a character array to hold the resulting string
        for (int i = 0; i < 8; i++) {
            // Extracting the i-th hexadecimal digit from the number by shifting it right
            // by (28 - 4*i) bits and then masking it with 0xF.
            int digit = (num >> (28 - 4 * i)) & 0xF;
            hexNum[i] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        }
        hexNum[8] = '\0'; // Add a null terminator to the end of the string.
        return string(hexNum); 
    }

    if (number < 0)
        number += (1 << add); // Convert the two's complement representation of negative numbers.

       /*If the 'add' parameter is not 32, we assume that we want to convert
       the number to a hexadecimal string representation with 'add' bits.*/
    string hexNum;
    do {
        int remainder = number % 16; // Compute the remainder of the number divided by 16.
        if (remainder < 10)
            hexNum = char(remainder + '0') + hexNum; // Convert the digit to its ASCII representation.
        else
            hexNum = char(remainder - 10 + 'A') + hexNum;
        number /= 16; // Divide the number by 16 to get the next digit.
    } while (number > 0);

    // Pad the string representation with leading zeros if necessary.
    while (hexNum.length() < add / 4)
        hexNum = '0' + hexNum;

    return hexNum; 
}
/// 
///
///
void second_pass()
{
    /*
    second pass of the assmebler
    generates machine code and the lsiting code*/
    int n=0;
    for(;n<(int)instData.size(); n++)
    {
        if(lines[n].empty())
        {continue;}
        string inst_location=appendZeros(dec_To_Hex(program_counter[n]));
        if(instData[n].instMnemonic=="")
        {
            string current_macCode="        ";
            mach_Code.push_back({n, current_macCode});
            continue;
        }
        switch (instData[n].oprType)
        {
        case 0:{
        string current_macCode="000000"+op_code[instData[n].instMnemonic].first;
        mach_Code.push_back({n, current_macCode});
            break;}
        case 1:{
        int dec_form;
        if(op_code[instData[n].instMnemonic].second == 2){
				int val = label_hold[instData[n].instOperand];
				dec_form = val - (program_counter[n] + 1);	
			}
			else{
				dec_form = label_hold[instData[n].instOperand];
			} 
			string current_macCode = appendZeros(dec_To_Hex(dec_form)) + op_code[instData[n].instMnemonic].first;
			mach_Code.push_back({n, current_macCode});
        break;}
        default:
        {
            int size=6;
            int add=24;
            if(instData[n].instMnemonic=="data")
            {
                size=8;
                add=32;
            }
            int dec_form=stoi(instData[n].instOperand, 0, instData[n].oprType);
            string current_macCode=appendZeros(dec_To_Hex(dec_form, add), size)+op_code[instData[n].instMnemonic].first;
            mach_Code.push_back({n, current_macCode});
            break;
        }
        }
    }
    // 	for(int i=0; i<mach_Code.size(); i++)
	// {
	// 	cout<<mach_Code[i].first<<" "<<mach_Code[i].second<<endl;
	// }
}
///
///
///
bool errors_check() {
    ofstream logFile("logFile.log");
    if (errors.empty()) {
        if (!halt_present) {
            cout << "1 warning detected" << endl;
            logFile << "Warning: HALT instruction not found. Program may not exit as intended." << endl;
        }
        logFile << "Machine code generated in: assembled_Code.o" << endl;
        logFile << "Listing code generated in: list_Code.l" << endl;
        logFile.close();
        cout << "Assembly complete. No errors detected." << endl;
        return true;
    } else {
        sort(errors.begin(), errors.end());
        logFile << errors.size() << "Assembly failed with error(s):" << endl;
        for (const auto& error : errors) {
            logFile << error.second << endl;
        }
        logFile.close();
        cout << errors.size() << " errors encountered. See logFile.log" << endl;
        return false;
    }
}
///
///
///
void write_files()
{
    // This function writes machine code and listing code to files
    // named " assembled_Code.o" and "list_Code.l" respectively.
    
    // Open "list_Code.l" file for writing
    ofstream outList_code("list_Code.l");
    // Iterating through "mach_Code" data structure
    for(auto it = mach_Code.begin(); it != mach_Code.end(); ++it) {
        // Retrieving instruction and associated data
        string instruction = it->second;
        string assemblyCode = lines[it->first];
        // int programCounter = dec_To_Hex(it->first);
        
        // Writing line to "listCode.l" file

        outList_code << appendZeros(dec_To_Hex(program_counter[it->first])) << " " << instruction << " " << assemblyCode << endl;
    }
    // Close "listCode.l" file
    outList_code.close();
    ofstream out_MachineCode;
    out_MachineCode.open("assembled_Code.o",ios::binary | ios::out);
    /*The "ios::binary" flag sets the output mode to binary, which means that the data will be written to the file in binary format. 
    Used for writing binary data such as machine code or other types of raw data.
    The "ios::out" flag specifies that the file should be opened in output mode, which means that data can be written to the file.*/
    // Iterate through "mach_Code" 
	for(auto to: mach_Code){ 
		unsigned int x;
		if(to.second.empty() or to.second == "        ")
			continue;
            /*the code converts the value stored in to.second to a hexadecimal string representation, 
            and then converts it to a signed integer for output.*/
    	stringstream hold;
    	hold << hex << to.second;
    	hold >> x; // output it as a signed type
     	static_cast<int>(x);
        /*the code writes the binary representation of the unsigned int value stored in the x variable to the output file.*/
 		out_MachineCode.write((const char*)&x, sizeof(unsigned int));
	}
    out_MachineCode.close();
}
///
///
///
void print_messages()
{
   	cout << "Log code generated in: logFile.log" << endl;
	cout << "Machine code generated in: assembled_Code.o" << endl;
	cout << "Listing code generated in: list_Code.l" << endl;
}


