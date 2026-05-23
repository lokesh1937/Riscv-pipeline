#include<bits/stdc++.h>
using namespace std;

class control_word{
public:
 bool Regread=false,Regwrite=false,Branch=false,jump=false;
 bool Memread=false,Memwrite=false,Mem2reg=false;
 bool ALUsrc=false;
 string ALUop="";
};

struct regentry{int value=0;};

class pc_reg{
   public:
   bool valid=true;
   int pc_value;
   pc_reg(){pc_value=0;}
};

class instr_fetch{
   public:
   bool stall=false;
   bool valid=true;
   int NPC,DPC;
   string IR;
};

class instr_decode{
    public:
    bool stall=false,valid=false;
    string func3,func7;
    int imm=0,rsl1=0,rsl2=0;
    int rs1=0,rs2=0;
    int rdl=0;int JPC=0,DPC=0,NPC=0;
    control_word CW;
};
class instr_exec{
    public:
    int ALUresult=0,SDvalue=0;
    bool stall=false,valid=false;
    int DPC=0,JPC=0,BPC=0,NPC=0;
    int rdl=0,rsl2=0,rsl1=0;
    bool branch;
    control_word CW;
};
class memory_oper{
    public:
    bool stall=false,valid=false;
    int LDresult=0,ALUresult=0;
    int DPC=0,BPC=0,JPC=0,NPC=0;
    int rdl=0;
    bool branch;
    control_word CW;
};

pc_reg PC;
instr_fetch IFID;
instr_decode IDEX;
instr_exec EXMO;
memory_oper MOWB;
vector<string>instructions;
vector<int>DM(2048,0);
vector<regentry>GPR(32);
bool prev_fetch=false,prev_decode=false,prev_execute=false,prev_mem=false;


// getbits: MSB is leftmost at inst[0]
string getbits(const string &inst,int l,int m){
    if(inst.size()!=32) return string("");
    string ans=""; for(int i=l;i<=m;i++) ans+=inst[i]; return ans;
}

int binToInt(const string &s){ 
    if(s.empty()) return 0; 
    int v=0; for(char c:s) v=(v<<1)|(c-'0'); return v; 
}

int genimm(const string &inst,const string &opcode){
    string imm="";
    if(opcode=="0110011") return 0; // Rtype
    else if(opcode=="0010011"||opcode=="1100111"){ // Itype immediae
         imm=getbits(inst,0,11); 
         return binToInt(imm); 
       }
    else if(opcode=="0000011"){ // Itype load
             imm=getbits(inst,0,11); 
             int val=binToInt(imm); 
             return val-(1<<12); 
      }
    else if(opcode=="0100011"){ // stype 
             imm=getbits(inst,0,6)+getbits(inst,20,24); 
             int val=binToInt(imm); 
             return val-(1<<12); 
        }
    else if(opcode=="1100011"){  //btype
        string imm12=getbits(inst,0,0),imm10_5=getbits(inst,1,6),imm4_1=getbits(inst,20,23),imm11=getbits(inst,24,24);
        string s=imm12+imm11+imm10_5+imm4_1; 
        return binToInt(s);
    }
    else if(opcode=="1101111"){ //jtype
        string imm20=getbits(inst,0,0), imm10_1=getbits(inst,1,10), imm11=getbits(inst,11,11), imm19_12=getbits(inst,12,19);
        string s=imm20+imm19_12+imm11+imm10_1; 
        cout<<binToInt(s)<<endl;
        return binToInt(s);
    }
    return 0;
}

int binaryto_decimal(const string &inst,int l,int m){ return binToInt(getbits(inst,l,m)); }

control_word controller(const string &opcode){
    control_word cw;
    if(opcode=="0110011"){ 
        cw.Regread=true; cw.Regwrite=true;
         cw.ALUsrc=false; cw.Memread=false;
          cw.Memwrite=false; cw.Mem2reg=false;
           cw.Branch=false; cw.jump=false; cw.ALUop="10"; 
        }
    else if(opcode=="0010011"){ 
        cw.Regread=true; cw.Regwrite=true; 
        cw.ALUsrc=true; cw.Memread=false; 
        cw.Memwrite=false; cw.Mem2reg=false; 
        cw.Branch=false; cw.jump=false; cw.ALUop="00"; 
    }
    else if(opcode=="0000011"){ 
        cw.Regread=true; cw.Regwrite=true; 
        cw.ALUsrc=true; cw.Memread=true; 
        cw.Memwrite=false; cw.Mem2reg=true; 
        cw.Branch=false; cw.jump=false; cw.ALUop="00"; 
    }
    else if(opcode=="0100011"){ 
        cw.Regread=true; cw.Regwrite=false; 
        cw.ALUsrc=true; cw.Memread=false; 
        cw.Memwrite=true; cw.Mem2reg=false; 
        cw.Branch=false; cw.jump=false; cw.ALUop="00"; 
    }
    else if(opcode=="1100011"){ 
        cw.Regread=true; cw.Regwrite=false; 
        cw.ALUsrc=false; cw.Memread=false; 
        cw.Memwrite=false; cw.Mem2reg=false; 
        cw.Branch=true; cw.jump=false; cw.ALUop="01"; 
    }
    else if(opcode=="1101111"){ 
        cw.Regread=false; cw.Regwrite=true; 
        cw.ALUsrc=false; cw.Memread=false; 
        cw.Memwrite=false; cw.Mem2reg=false; 
        cw.Branch=false; cw.jump=true; cw.ALUop="xx"; }
    return cw;
}

string ALUControl(const string &ALUOp,const string &func7,const string &func3){
    if(ALUOp=="00") return "ADD";
    if(ALUOp=="01") return "SUB";
    if(ALUOp=="10"){
        if(func3=="000"){
           if(func7=="0000000"){return "ADD";}
           else if(func7=="0100000"){return "SUB";}
           else if(func7=="0000001"){return "MUL";}
        }
        else if(func3=="111"){return "AND";}
        else if(func3=="110"){
           if(func7=="0000000"){return "OR";}
           else if(func7=="0000001"){return "REM";}
        }
        else if(func3=="100"){
          if(func7=="0000000"){return "XOR";}
          else if(func7=="0000001"){return "DIV";}
        }
        else if(func3=="010") return "SLT";
    }
    return "NOP";
}

int ALU(const string &sel,int a,int b){
    if(sel=="ADD") return a+b;
    else if(sel=="SUB") return a-b;
    else if(sel=="AND") return a&b;
    else if(sel=="OR")  return a|b;
    else if(sel=="XOR") return a^b;
    else if(sel=="SLT") return (a<b)?1:0;
    else if(sel=="MUL") return a*b;
    else if(sel=="DIV") return a/b;
    else if(sel=="REM") return a%b;
    return 0;
}

int outputSelect(bool Mem2reg,bool jump,int ALUresult,int LDresult,int DPC){
    if(Mem2reg) return LDresult;
    if(jump) return DPC;
    return ALUresult;
}

string instruction_mem(int pc){ return instructions[pc/4]; }

// Safe memory access read and write
int safe_mem_read(int byte_addr){
    int idx=abs(byte_addr)/ 4;
    cout<<"Read value:"<<DM[idx]<<" at "<<byte_addr<<endl;
    return DM[idx];
}
void safe_mem_write(int byte_addr,int rsl2){
    int value=GPR[rsl2].value;
    cout<<"memwrite "<<value<<" at "<<byte_addr<<endl;
    int idx = abs(byte_addr)/ 4;
    DM[idx] = value;
}

// ---- Pipeline stages ----
void inst_fetch(){
    if(IFID.stall||!PC.valid || PC.pc_value>=instructions.size()*4) return;
    IFID.IR = instruction_mem(PC.pc_value);
    IFID.NPC = PC.pc_value + 4;
    IFID.valid = true;
    PC.pc_value=PC.pc_value+4;
    prev_fetch=true;
    cout<<"[IF] Fetched: "<<IFID.IR<<endl;
}

void inst_decode(){
    if(IDEX.stall||!IFID.valid|| !prev_fetch) return;
    prev_fetch=false;
    string opcode = getbits(IFID.IR,25,31);
    IDEX.func3 = getbits(IFID.IR,17,19);
    IDEX.func7 = getbits(IFID.IR,0,6);
    int rs1_idx = binaryto_decimal(IFID.IR,12,16);
    int rs2_idx = binaryto_decimal(IFID.IR,7,11);
    IDEX.rdl = binaryto_decimal(IFID.IR,20,24);
    IDEX.imm = genimm(IFID.IR,opcode);
    IDEX.CW = controller(opcode);
    IDEX.rsl1=rs1_idx;
    IDEX.rsl2 =rs2_idx;
    
    IDEX.NPC=IFID.NPC;
    IFID.stall = false; IDEX.valid = true,prev_decode=true;
    cout<<"[ID] opcode="<<opcode<<" rs1="<<IDEX.rsl1<<" rs2="<<IDEX.rsl2
        <<" rd="<<IDEX.rdl<<" imm="<<IDEX.imm<<endl;
}

void inst_exec(){
    if(EXMO.stall||!IDEX.valid || !prev_decode) return;
    prev_decode=false;
    cout<<"rsl1: "<<IDEX.rsl1<<endl;
    cout<<"rsl2: "<<IDEX.rsl2<<endl;
    int alusrc1 = GPR[IDEX.rsl1].value;
    int alusrc2 = GPR[IDEX.rsl2].value;
   

    int rs1_value=alusrc1,rs2_value=alusrc2;

    
     if(EXMO.CW.Regwrite && EXMO.rdl!=0){
        cout<<"EXMO rdl: "<<EXMO.rdl<<endl;
        cout<<"rsl1: "<<IDEX.rsl1<<endl;
        cout<<"rsl2: "<<IDEX.rsl2<<endl;
        if(EXMO.rdl==IDEX.rsl1){
            rs1_value=EXMO.ALUresult;
        }
        if(EXMO.rdl==IDEX.rsl2){
            cout<<"EXMO ALU result: "<<EXMO.ALUresult<<endl;
            rs2_value=EXMO.ALUresult;
        }
    }
    cout<<"rs1 value "<<rs1_value<<endl;cout<<"rs2 value "<<rs2_value<<endl;
    
    if(MOWB.CW.Regwrite && MOWB.rdl!=0){
        if(IDEX.rsl1==MOWB.rdl){
            rs1_value=MOWB.LDresult;
        }
        if(IDEX.rsl2==MOWB.rdl){
            rs2_value=MOWB.LDresult;
        }
    }

    alusrc1=rs1_value;
    EXMO.SDvalue = rs2_value;
    alusrc2=((IDEX.CW.ALUsrc)?IDEX.imm:rs2_value);
    cout<<"alusrc1: "<<alusrc1<<endl;
    cout<<"alusrc2: "<<alusrc2<<endl;

    EXMO.CW = IDEX.CW;
    EXMO.rdl = IDEX.rdl;
    EXMO.NPC=IDEX.NPC;
    EXMO.rsl1=IDEX.rsl1,EXMO.rsl2=IDEX.rsl2;
    string ALUsel = ALUControl(IDEX.CW.ALUop,IDEX.func7,IDEX.func3);
    EXMO.ALUresult = ALU(ALUsel,alusrc1,alusrc2);
    
    EXMO.valid = true;
    cout<<"[EX] ALU="<<ALUsel<<" result="<<EXMO.ALUresult<<endl;
    bool branchTaken=false;
    // branch/jump logic if needed (user previously requested)
    if(EXMO.CW.Branch){
        int result=EXMO.ALUresult;
        if(IDEX.func3=="000"){ //BEQ equal
          branchTaken=((result==0)?1:0);
        }
        else if(IDEX.func3=="001"){// bne equal
           branchTaken=((result!=0)?1:0);
        }
       else if(IDEX.func3=="100"){ // blt
          branchTaken=(result<0)?1:0;
       } 
       else if(IDEX.func3=="101"){ //bge
         branchTaken=((result>=0)?1:0);
        }
      else if(IDEX.func3=="110"){ //bltu
         branchTaken=(result<0)?1:0;
       }
       else if(IDEX.func3=="111"){ //bgeu
         branchTaken=(result>=0)?1:0;
       }
       if(branchTaken){
            EXMO.BPC= IDEX.imm-4;
            PC.pc_value=EXMO.BPC;
        }
    }
    if(EXMO.CW.jump){
        branchTaken = true;
        EXMO.JPC= IDEX.imm-4;
        PC.pc_value=EXMO.JPC;
    }
    if(branchTaken && EXMO.CW.jump){ 
         IFID.valid=false; 
         IDEX.valid=false; 
         cout<<"[PIPELINE] Flushed -> New PC="<<EXMO.JPC<<endl; 
    }
    if(branchTaken && EXMO.CW.Branch){
       IFID.valid=false;
       IDEX.valid=false;
       cout<<"[PIPELINE] Flushed -> New PC="<<EXMO.BPC<<endl;
     }
     EXMO.branch=true;
     prev_execute=true;
}

void inst_mem(){
    if(MOWB.stall||!EXMO.valid || !prev_execute) return;
    prev_execute=false;
    if(EXMO.CW.Memread) MOWB.LDresult = safe_mem_read(EXMO.ALUresult);
    if(EXMO.CW.Memwrite) safe_mem_write(EXMO.ALUresult,EXMO.rsl2);
    MOWB.CW=EXMO.CW; 
    MOWB.ALUresult=EXMO.ALUresult; 
    MOWB.rdl=EXMO.rdl;
    MOWB.BPC=EXMO.BPC;
    MOWB.JPC=EXMO.JPC;
    MOWB.NPC=EXMO.NPC;
    MOWB.branch=EXMO.branch;
    MOWB.valid=true;
    prev_mem=true;
    cout<<"[MEM] result="<<MOWB.ALUresult<<endl;
}

void write_back(){
    if(!MOWB.valid || !prev_mem) return;
    prev_mem=false;
    if(MOWB.CW.Regwrite){
        cout<<"written back to register"<<endl;
        int val = outputSelect(MOWB.CW.Mem2reg,MOWB.CW.jump,MOWB.ALUresult,MOWB.LDresult,MOWB.DPC);
        GPR[MOWB.rdl].value = val;
        cout<<"[WB] x"<<MOWB.rdl<<" = "<<val<<endl;
    }
    else if(MOWB.CW.jump && MOWB.branch){
        cout<<"jump taken"<<endl;
    }
    else if(MOWB.CW.Branch && MOWB.branch){
        cout<<"branch taken"<<endl;
    }
    else{
        cout<<"store happened"<<endl;
    }
}

// ---- Main ----
int main(){
    ifstream infile("output.txt");
    if(!infile.is_open()){ cerr<<"Cannot open output.txt"<<endl; return 1;}
    string line;
    vector<string>pinst;
    while(getline(infile,line)){
        line.erase(remove_if(line.begin(),line.end(),::isspace),line.end());
        line.erase(remove_if(line.begin(),line.end(),[](char c){return c!='0'&&c!='1';}),line.end());
        if(line.size()==32) pinst.push_back(line);
        else if(!line.empty()) cerr<<"Skipping invalid line len="<<line.size()<<endl;
    }
    
    cout<<"Loaded "<<pinst.size()<<" instructions."<<endl;
    for(auto &r:GPR) r.value=0;
    
    int cycles=0;
    int n=pinst.size();
    for(int i=0;i<n;i++){
        if(i==n-1){
            instructions.push_back("00000000000000000000000000010011");
            instructions.push_back("00000000000000000000000000010011");
            instructions.push_back(pinst[i]);
        }
        else{instructions.push_back(pinst[i]);}
    }
    n=instructions.size();
    cout<<"Number of instructions: "<<n<<endl;
    int last_inst=(n-1)*4;
    cout<<"Last instr: "<<last_inst<<endl;
    int extra_cycles=5;
    
    //pipeline order
    while(PC.pc_value<4*n || extra_cycles>0){
        if(PC.pc_value==(4*n)){PC.valid=false;}
        if((PC.pc_value==last_inst) || (PC.pc_value==(n*4))){extra_cycles--;}
        cout<<"--"<<"cycle "<<cycles<<"---"<<endl;
        write_back();
        inst_mem();
        inst_exec();
        inst_decode();
        inst_fetch();
        cycles++;
    }
    
    // cout<<"\nFinal Register State:\n";
    // for(int i=0;i<32;i++) cout<<"x"<<i<<"="<<GPR[i].value<<" ";
    // cout<<endl;
    cout<<"Final result(which stored in memory): ";
    cout<<safe_mem_read(20)<<endl;
    return 0;
}
