#include<iostream>
#include<cmath>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<string>
#include<vector>
#include<unordered_map>
#include<map>
#include<bitset>
using namespace std;
/*
        INPUTS
*/


// input factorial code


/*
addi a5,x0,5
sw a5,-28(s0)
addi a5,x0,1
sw a5,-20(s0)
addi a5,x0,2
sw a5,-24(s0)
jal x1,15
lw a4,-20(s0)
lw a5,-24(s0)
mul a5,a4,a5
sw a5,-20(s0)
lw a5,-24(s0)
addi a5,a5,1
sw a5,-24(s0)
lw a4,-24(s0)
lw a5,-28(s0)
bge a5,a4,8
addi a5,x0,0
addi a0,a5,0
*/

// sum of n

/*
        addi    a5,x0,5
        sw      a5,-28(s0)
        sw      x0,-20(s0)
        addi    a5,x0,1
        sw      a5,-24(s0)
        jal     x1,14
        lw      a4,-20(s0)
        lw      a5,-24(s0)
        add     a5,a4,a5
        sw      a5,-20(s0)
        lw      a5,-24(s0)
        addi    a5,a5,1
        sw      a5,-24(s0)
        lw      a4,-24(s0)
        lw      a5,-28(s0)
        bge     a5,a4,7
        addi    a5,x0,0

*/


//prime checking

/*
    addi a5,x0,5
  sw a5,-24(s0)
  addi a5,x0,1
  sw a5,-28(s0)
  addi a5,x0,2
  sw a5,-20(s0)
  jal x1,16
  lw a4,-24(s0)
  lw a5,-20(s0)
  rem a5,a4,a5
  bne a5,x0,13
  sw x0,-28(s0)
  lw a5,-20(s0)
  addi a5,a5,1
  sw a5,-20(s0)
  lw a4 ,-20(s0)
  lw a5,-24(s0)
  blt a4,a5,8
  addi a5,x0,0
  
*/

/*
                instructions
*/
map<string,int>labeladdress;
map<string,int>Rtype={{"add",1},{"sub",1},{"xor",1},{"or",1},{"and",1},{"sll",1},{"srl",1},{"sra",1},{"slt",1},{"sltu",1}};
map<string,int>RMtype={{"mul",1},{"mulh",1},{"mulsu",1},{"mulu",1},{"div",1},{"divu",1},{"rem",1},{"remu",1}};

map<string,int>Itype={{"addi",1},{"xori",1},{"ori",1},{"andi",1},{"slli",1},{"srli",1},{"srai",1},{"slti",1},{"sltiu",1},{"lb",1},{"lh",1},{"lw",1},{"lbu",1},{"lhu",1},{"jalr",1}};
map<string,int>Stype={{"sb",1},{"sh",1},{"sw",1}};
map<string,int>Btype={{"beq",1},{"bne",1},{"blt",1},{"bge",1},{"bltu",1},{"bgeu",1}};
map<string,int>Jtype={{"jal",1}};

unordered_map<string,int>registers={
    {"x0",0},{"x1",1},{"x2",2},{"x3",3},{"x4",4},{"x5",5},{"x6",6},{"x7",7},{"x8",8},{"x9",9},{"x10",10},{"x11",11},{"x12",12},{"x13",13},
    {"x14",14},{"x15",15},{"x16",16},{"x17",17},{"x18",18},{"x19",19},{"x20",20},{"x21",21},{"x22",22},{"x23",23},{"x24",24},{"x25",25},{"x26",26},
    {"x27",27},{"x28",28},{"x29",29},{"x30",30},{"x31",31},
    {"zero",1},{"ra",1},{"sp",2},{"gp",3},{"tp",4},{"t0",5},{"t1",6},{"t2",7},{"s0",8},{"s1",9},{"a0",10},{"a1",11},{"a2",12},{"a3",13},{"a4",14},
    {"a5",15},{"a6",16},{"a7",17},{"s2",18},{"s3",19},{"s4",20},{"s5",21},{"s6",22},{"s7",23},{"s8",24},{"s9",25},{"s10",26},{"s11",27},{"t3",28},{"t4",29},{"t5",30},
    {"t6",31}
};




string remove_space(string &line){
    int i=0,j=line.length()-1;
    while(i<=j){
        if((line[i]!=' ') && (line[j]!=' ')){break;}
        if(line[i]==' '){i++;}
        else if(line[j]==' '){j--;}
    }
    int length=j-i+1;
    if(length==0) return "";
    string ans="";
    for(int k=i;k<=j;k++){ans+=line[k];}
    return ans;
}

string Remove_commas(string &line){
    int n=line.length();
    for(int i=0;i<n;i++){
        if(line[i]==','){line[i]=' ';}
        if(line[i]=='(' || line[i]==')'){
            line[i]=' ';
        }
    }
    
    return line;
}


/*
        FUNCTIONS
*/
int find_imm(string &instructions){
    int imm;
    string word;
    stringstream ss(instructions);
    while(ss>>word){
        if(Btype.find(word)!=Btype.end()){
            continue;
        }
        else if(registers.find(word)!=registers.end()){
            continue;
        }
        else{
            imm=stoi(word);
        }
    }
    return imm;
}

int find_immj(string &instructions){
    int imm;
    string word;
    stringstream ss(instructions);
    while(ss>>word){
        if(Jtype.find(word)!=Jtype.end()){
            continue;
        }
        else if(registers.find(word)!=registers.end()){
            continue;
        }
        else{
            imm=stoi(word);
        }
    }
    return imm;
}

/*

        INSTRUCTION FUNCTIONS OF ALL TYPES

*/
string Convert_to_Rtype(string &instructions){
   
    string opcode="0110011";
    string func7="0000000";
    string func3="";
    bitset<5>rs1,rs2,rd;
    
    stringstream ss(instructions);
    string word;int flag=0;
    
    while(ss>>word){
        if(Rtype.find(word)!=Rtype.end()){
            if(word=="add"){
                func3="000";
                func7="0000000";

            }
            else if(word=="sub"){
                func3="000";
                func7="0100000";

            }
            else if(word=="and"){
                func3="111";
                func7="0000000";

            }
            else if(word=="or"){
                func3="110";
                func7="0000000";

            }
            else if(word=="xor"){
                func3="100";
                func7="0000000";

            }
            else if(word=="sll"){
                func3="001";
                func7="0000000";

            }
            else if(word=="srl"){
                func3="101";
                func7="0000000";

            }
            else if(word=="sra"){
                func3="101";
                func7="0100000";

            }
            else if(word=="slt"){
                func3="010";
                func7="0000000";
                
            }
            else if(word=="sltu"){
                func3="011";
                func7="0000000";

            }
        }
        else if(registers.find(word)!=registers.end()){
            if(flag==0){rd=bitset<5>(registers[word]);}
            else if(flag==1){rs1=bitset<5>(registers[word]);}
            else if(flag==2){rs2=bitset<5>(registers[word]);}
            flag++;
        }
    }
    return (func7+rs2.to_string()+rs1.to_string()+func3+rd.to_string()+opcode);

}

string Convert_to_RMtype(string &instructions){
   
    string opcode="0110011";
    string func7="0000001";
    string func3="";
    bitset<5>rs1,rs2,rd;
    
    stringstream ss(instructions);
    string word;int flag=0;
    while(ss>>word){
        if(RMtype.find(word)!=RMtype.end()){
            if(word=="mul"){
                func3="000";

            }
            else if(word=="mulh"){
                func3="001";
            }
            else if(word=="mulsu"){
                func3="010";
            }
            else if(word=="mulu"){
                func3="011";

            }
            else if(word=="div"){
                func3="100";

            }
            else if(word=="divu"){
                func3="101";

            }
            else if(word=="rem"){
                func3="110";

            }
            else if(word=="remu"){
                func3="111";

            }
        }
        else if(registers.find(word)!=registers.end()){
            if(flag==0){rd=bitset<5>(registers[word]);}
            else if(flag==1){rs1=bitset<5>(registers[word]);}
            else if(flag==2){rs2=bitset<5>(registers[word]);}
            flag++;
        }
    }
     return (func7+rs2.to_string()+rs1.to_string()+func3+rd.to_string()+opcode);

}

string Convert_to_Itype(string &instructions){
   
 string opcode,func3;
 string rs1,rd,imm;
 string func7="";
 stringstream ss(instructions);
 string word;
 int flag=0;
 while(ss>>word){
     if(Itype.find(word)!=Itype.end()){
         if(word=="addi"){
             func3="000";
             opcode="0010011";
         }
         else if(word=="andi"){
             func3="111";
             opcode="0010011";
         }
         else if(word=="ori"){
             func3="110";
             opcode="0010011";
         }
         else if(word=="xori"){
             func3="100";
             opcode="0010011";
         }
         else if(word=="slti"){
             func3="010";
             opcode="0010011";
         }
         else if(word=="sltiu"){
             func3="011";
             opcode="0010011";
         }
         else if(word=="slli"){
             func3="001";
             opcode="0010011";
             func7="0000000";
         }
         else if(word=="srli"){
             func3="101";
             opcode="0010011";
             func7="0000000";
         }
         else if(word=="srai"){
             func3="101";
             opcode="0010011";
             func7="0100000";
         }
         
         
         
         
         
         
         else if(word=="lb"){
             func3="000";
             opcode="0000011";
         }
         else if(word=="lh"){
             func3="001";
             opcode="0000011";
         }
         else if(word=="lw"){
             func3="010";
             opcode="0000011";
         }
         else if(word=="lbu"){
             func3="100";
             opcode="0000011";
         }
         else if(word=="lhu"){
             func3="101";
             opcode="0000011";
         }
         else if(word=="jalr"){
             func3="000";
             opcode="1100111";
         }
     }
     else if(registers.find(word)!=registers.end()){
         int num =registers[word];
         if(flag==0){rd=bitset<5>(registers[word]).to_string();
             
         }
            else if(flag==1){rs1=bitset<5>(registers[word]).to_string();}
            flag++;
     }
     else{
         try{
         int imm_val=stoi(word);
         if(func7==""){
             bitset<12> imm_bits((imm_val<0) ? (imm_val + (1<<12)) : imm_val); 
             imm=imm_bits.to_string();
         }
         else {
             bitset<5> shamt_bits(imm_val & 0x1F); 
             string shamt=shamt_bits.to_string();
             imm=func7+shamt;
         }
         } catch(...){
             return "error at line";
         }
     }
 }
 return imm+rs1+func3+rd+opcode;

}

string Convert_to_Stype(string &instructions){
     string opcode,func3;
     opcode="0100011";
    string rs1,rs2,imm,imm_low;
    
    string func7="";
    stringstream ss(instructions);
    string word;
     int flag=0;
    while(ss>>word){
        if(Stype.find(word)!=Stype.end()){
            if(word=="sw"){
                func3="010";
            }
            else if(word=="sb"){
                func3="000";
            }
            else if(word=="sh"){
                func3="001";
            }
            else{
                return "error in this line";
            }
        }
        else if(registers.find(word)!=registers.end()){
            int num=registers[word];
            if(flag==0){
                rs2=bitset<5>(num).to_string();
            }
            else if(flag==1){
                rs1=bitset<5>(num).to_string();
            }
            flag++;
        }
        else{
         try{
         int imm_val=stoi(word);
         bitset<12> imm_bits((imm_val<0) ? (imm_val + (1<<12)) : imm_val);
         string imm_str=imm_bits.to_string();
         func7=imm_str.substr(0,7);
         imm_low=imm_str.substr(7,5);
         
         
         
         } catch(...){
             return "error at line";
         }
     }
    }
    return func7+rs2+rs1+func3+imm_low+opcode;
}

string Convert_to_Btype(string &instructions,int immed){
     string opcode,func3,rs1,rs2;
     string imm12,imm10_5,imm4_1,imm11;
     opcode="1100011";

    stringstream ss(instructions);
    string word;
     int flag=0;
    while(ss>>word){
        if(Btype.find(word)!=Btype.end()){
            if(word=="beq"){
                func3="000";
            }
            else if(word=="bne"){
                func3="001";
            }
            else if(word=="blt"){
                func3="100";
            }
            else if(word=="bge"){
                func3="101";
            }
            else if(word=="bltu"){
                func3="110";
            }
            else if(word=="bgeu"){
                func3="111";
            }
        }
        else if(registers.find(word)!=registers.end()){
            if(flag==0){
                int x=registers[word];
                rs1=bitset<5>(x).to_string();
                flag++;
            }
            else if(flag==1){
                int x=registers[word];
                rs2=bitset<5>(x).to_string();
                flag++;
            }
        }
        else {
            string imm=bitset<12>(immed).to_string();
            imm4_1=imm.substr(8,4);
            imm10_5=imm.substr(2,6);
            imm11=imm[1];
            imm12=imm[0];
        }
    }
    return imm12+imm10_5+rs2+rs1+func3+imm4_1+imm11+opcode;
    
}

string Convert_to_Jtype(string &instructions,int immed){
     string opcode,rd;
    
     opcode="1101111";
    string imm="";
    imm=bitset<20>(immed).to_string();
    string imm20,imm10_1,imm11,imm19_12;
    
    stringstream ss(instructions);
    string word;
     int flag=0;
    while(ss>>word){
    if(registers.find(word)!=registers.end()){
        rd=bitset<5>(registers[word]).to_string();
    }
    else{
        continue;
    }
    }
    imm20=imm[0];
    imm10_1=imm.substr(10,10);
    imm11=imm[9];
    imm19_12=imm.substr(1,8);
    

return imm20+imm10_1+imm11+imm19_12+rd+opcode;
}



int main(){
    ifstream infile("input.txt");
    ofstream outfile("output.txt");
    // if(!outfile.is_open()){
    //     cerr<<"failed to pen"<<endl;
    //     return 1;
    // }
    // outfile<<"hii"<<endl;
    // outfile.close();
    string line;
    vector<string>programs;
    int pc=0;
    /*
        parsing function
    */
    while(getline(infile,line)){

        if(line.find(';')!=string::npos){
            line=line.substr(0,line.find(';'));
        }
        line=remove_space(line);
        if(line.length()==0) continue;
        line=Remove_commas(line);
        stringstream ss(line);
        string word; ss>>word;
        if(word.back()==':'){string label=word.substr(0,word.length()-1);
        labeladdress[word]=pc;pc+=4;continue;}
        programs.push_back(line);
            
    }
      for(int i=0;i<programs.size();i++){
          string instructions=programs[i];
          stringstream ss(instructions);string word;
          ss>>word;
          string type=word;
          if(Rtype.find(type)!=Rtype.end()){
                string result=Convert_to_Rtype(instructions);
                outfile<<result<<endl;
               cout<<result<<endl;
            }
        else if(RMtype.find(word)!=RMtype.end()){
            string result=Convert_to_RMtype(instructions);
                outfile<<result<<endl;
               cout<<result<<endl;
        }
          else if(Itype.find(type)!=Itype.end()){
                 string result = Convert_to_Itype(instructions);
                  outfile<<result<<endl;
                  cout<<result<<endl;
              }
        
          else if(Stype.find(type)!=Stype.end()){
             string result = Convert_to_Stype(instructions);
              outfile<<result<<endl;
              cout<<result<<endl;
          }
          else if(Btype.find(type)!=Btype.end()){
              int immed_index=find_imm(instructions);
              int immed=(immed_index)*4;
              
             string result =  Convert_to_Btype(instructions,immed);
             outfile<<result<<endl;
             cout<<result<<endl;
          }
          else if(Jtype.find(type)!=Jtype.end()){
               int immed_index=find_immj(instructions);
              int immed=(immed_index)*4;
             string result = Convert_to_Jtype(instructions,immed);
             outfile<<result<<endl;
             cout<<result<<endl;
          }
          else{
              cerr<<"Not a valid instruction"<<"\n";
          }
      }
    
}
