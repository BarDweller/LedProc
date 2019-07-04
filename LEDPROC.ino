#include<OctoWS2811.h>

#define MAX_BANK 256
#define MAX_REG  256
#define MAX_LED  (50*8)

const int ledsPerStrip = 50;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

int regs[MAX_REG];
int activebank=0;
int bankoffset=0;
int evalvalue=0;

//char *test = "R0:50J0L!0:!0,!0,!0U1W10S0:1JZ0:1JA0J1";
//char *test = "R0:100R1:0R5:25J0L!1,200,200,200R2:!1R3:1R10:10JA5J10L!2,100,100,100R2:!1R3:2R10:11JA5J11L!2,50,50,50R2:!1R3:3R10:12JA5J12L!2,0,0,0U1W1A1:1E!0JL!1:1R1:0S5:1JZ5:99J1JA0JA99J5S2:!3E0JG!2:6A2:!0J6JA!10J99";
//char *ltest = "L1:1,20,123L2:1,2,3L!0:1,2,300";
//char *rtest = "R0:1A0:2S0:1M0:4D0:2";

typedef struct ArgStruct {
  int val;
  boolean indirect; 
} Arg;

typedef void(*CmdPtr)(Arg args[]);

typedef struct CommandStruct {
  Arg args[4];
  CmdPtr cmd;
} Command;

Command *banks[MAX_BANK];
boolean vledEnabled=false;
short ledMap[MAX_LED];

void VL_CMD(Arg args[]) {
  vledEnabled = args[0].val > 0;
}
void MV_CMD(Arg args[]) {
  ledMap[args[0].val] = args[1].val;
}
void L_CMD(Arg args[]) {
  int target = args[0].indirect?regs[args[0].val]:args[0].val;
  int r = args[1].indirect?regs[args[1].val]:args[1].val;
  int g = args[2].indirect?regs[args[2].val]:args[2].val;
  int b = args[3].indirect?regs[args[3].val]:args[3].val;
  Serial.print("L_CMD: ");
  Serial.print(args[0].indirect?"!":"");
  Serial.print(args[0].val);
  Serial.print(":");
  Serial.print(args[1].indirect?"!":"");
  Serial.print(args[1].val);
  Serial.print(",");
  Serial.print(args[2].indirect?"!":"");
  Serial.print(args[2].val);
  Serial.print(",");
  Serial.print(args[3].indirect?"!":"");
  Serial.print(args[3].val);
  if(vledEnabled){
    Serial.print("REMAP:");
    Serial.print(target);
    target = ledMap[target];
    Serial.print("->");
    Serial.print(target);
  }
  if(target>=0 && target<MAX_LED){
    leds.setPixel(target,r,g,b);
  }
  Serial.println("");
}
void W_CMD(Arg args[]) {
  Serial.print("W_CMD: ");
  Serial.print(args[0].indirect?"!":"");
  Serial.print(args[0].val);
  int value = args[0].indirect?regs[args[0].val]:args[0].val;
  delayMicroseconds(value);
}
void U_CMD(Arg args[]) {
  Serial.print("U_CMD: ");
  while(leds.busy()){
    delayMicroseconds(30);
  }
  Serial.println("SHOW");
  leds.show();
}
void R_CMD(Arg args[]) {
  Serial.print("R_CMD: "); 
  int target = args[0].indirect?regs[args[0].val]:args[0].val;
  int value = args[1].indirect?regs[args[1].val]:args[1].val;
  Serial.print(args[0].indirect?"!":"");
  Serial.print(args[0].val);
  Serial.print(":");
  Serial.print(args[1].indirect?"!":"");
  Serial.print(args[1].val);   
  if(target>=0 && target<MAX_REG){
    regs[target]=value;
  }
  Serial.println("");  
}
void A_CMD(Arg args[]) {
  Serial.print("A_CMD: "); 
  int target = args[0].indirect?regs[args[0].val]:args[0].val;
  int value = args[1].indirect?regs[args[1].val]:args[1].val;
  Serial.print(args[0].indirect?"!":"");
  Serial.print(args[0].val);
  Serial.print(":");
  Serial.print(args[1].indirect?"!":"");
  Serial.print(args[1].val);   
  if(target>=0 && target<MAX_REG){
    regs[target]+=value;
  }
  Serial.println("");  
}
void S_CMD(Arg args[]) {
  Serial.print("S_CMD: "); 
  int target = args[0].indirect?regs[args[0].val]:args[0].val;
  int value = args[1].indirect?regs[args[1].val]:args[1].val;
  Serial.print(args[0].indirect?"!":"");
  Serial.print(args[0].val);
  Serial.print(":");
  Serial.print(args[1].indirect?"!":"");
  Serial.print(args[1].val);   
  if(target>=0 && target<MAX_REG){
    regs[target]-=value;
  }
  Serial.println("");  
}
void M_CMD(Arg args[]) {
  Serial.print("M_CMD: "); 
  int target = args[0].indirect?regs[args[0].val]:args[0].val;
  int value = args[1].indirect?regs[args[1].val]:args[1].val;
  Serial.print(args[0].indirect?"!":"");
  Serial.print(args[0].val);
  Serial.print(":");
  Serial.print(args[1].indirect?"!":"");
  Serial.print(args[1].val);   
  if(target>=0 && target<MAX_REG){
    regs[target]*=value;
  }
  Serial.println("");  
}
void D_CMD(Arg args[]) {
  Serial.print("D_CMD: "); 
  int target = args[0].indirect?regs[args[0].val]:args[0].val;
  int value = args[1].indirect?regs[args[1].val]:args[1].val;
  Serial.print(args[0].indirect?"!":"");
  Serial.print(args[0].val);
  Serial.print(":");
  Serial.print(args[1].indirect?"!":"");
  Serial.print(args[1].val);   
  if(target>=0 && target<MAX_REG){
    regs[target]/=value;
  }
  Serial.println("");  
}
void E_CMD(Arg args[]) {
  Serial.print("E_CMD: "); 
  int target = args[0].indirect?regs[args[0].val]:args[0].val;
  Serial.print(args[0].indirect?"!":"");
  Serial.print(args[0].val); 
  evalvalue=target;
  Serial.println("");  
}
void J_CMD(Arg args[]) {
  Serial.print("J_CMD: [label]");
  Serial.println(args[0].val);
}
int findJLabel(int label){
  int index=0;
  Command c = banks[activebank][index];
  while(c.cmd!=NULL){
    if(c.cmd==J_CMD && c.args[0].val==label){
      return index;
    }
    index++;
    c = banks[activebank][index];
  }
  return -1;
}
void JG_CMD(Arg args[]) {
  Serial.print("JG_CMD: ");
  int rhs = evalvalue;
  int lhs = args[0].indirect?regs[args[0].val]:args[0].val;
  int label = args[1].indirect?regs[args[1].val]:args[1].val;
  int offset = findJLabel(label);
  if(offset!=-1 && lhs>rhs){
    bankoffset=offset;
  }
  Serial.println(""); 
}
void JL_CMD(Arg args[]) {
  Serial.print("JL_CMD: ");
  int rhs = evalvalue;
  int lhs = args[0].indirect?regs[args[0].val]:args[0].val;
  int label = args[1].indirect?regs[args[1].val]:args[1].val; 
  Serial.print(lhs);
  Serial.print("<");
  Serial.print(rhs);
  Serial.print("->");
  Serial.print(label);
  int offset = findJLabel(label);
  if(offset!=-1 && lhs<rhs){
    bankoffset=offset;
  }
  Serial.println("");   
}
void JE_CMD(Arg args[]) {
  Serial.print("JE_CMD: ");
  int rhs = evalvalue;
  int lhs = args[0].indirect?regs[args[0].val]:args[0].val;
  int label = args[1].indirect?regs[args[1].val]:args[1].val;
  int offset = findJLabel(label);
  if(offset!=-1 && lhs==rhs){
    bankoffset=offset;
  } 
  Serial.println("");   
}
void JN_CMD(Arg args[]) {
  Serial.print("JN_CMD: ");
  int rhs = evalvalue;
  int lhs = args[0].indirect?regs[args[0].val]:args[0].val;
  int label = args[1].indirect?regs[args[1].val]:args[1].val;
  int offset = findJLabel(label);
  if(offset!=-1 && lhs!=rhs){
    bankoffset=offset;
  }
  Serial.println("");   
}
void JZ_CMD(Arg args[]) {
  Serial.print("JZ_CMD: ");
  int lhs = regs[args[0].val];
  int label = args[1].indirect?regs[args[1].val]:args[1].val;
  Serial.print(lhs);
  Serial.print(":");
  Serial.print(label);
  Serial.print("->");
  int offset = findJLabel(label);
  Serial.print(offset);  
  if(offset!=-1 && lhs==0){
    bankoffset=offset;
  }
  Serial.println(""); 
}
void JA_CMD(Arg args[]) {
  Serial.print("JA_CMD: ");
  int label = args[0].indirect?regs[args[0].val]:args[0].val;
  int offset = findJLabel(label);
  if(offset!=-1){
    bankoffset=offset;
  }  
  Serial.println("");   
}
void JB_CMD(Arg args[]) {
  Serial.print("JB_CMD: ");
  int bank = args[0].indirect?regs[args[0].val]:args[0].val;
  if(bank>=0 && bank<MAX_BANK){
    activebank=bank;
  }
  Serial.println("");     
}

boolean isAlpha(char c){
  return c>='A'&&c<='Z';
}

int countCommands(char *input, int maxlen) {
  if(!isAlpha(input[0])){
    Serial.print("Bad Input: ");
    Serial.println(input[0]);
    return 0;
  }  
  int count = 0;
  boolean inCmd = true;
  for(int i = 0; i<maxlen; i++){
    if(inCmd && !isAlpha(input[i])){
      inCmd = false;
      count++;
    }
    if(!inCmd && isAlpha(input[i])){
      inCmd = true;
    }
  }
  return count;
}

int readInt(char *input, int index, int maxIndex, Arg *arg){
  int newindex=index;  
  arg->val=0;
  while((input[newindex]>='0' && input[newindex]<='9') && newindex<maxIndex){
    arg->val = arg->val * 10;
    arg->val = arg->val + (input[newindex] - '0');
    newindex++;
  }
  Serial.print("Read Int [result]");
  Serial.print(arg->indirect?"!":"");
  Serial.print(arg->val);
  Serial.print("[index]");
  Serial.print(index);
  Serial.print("[newindex]");
  Serial.println(newindex);
  return newindex;
}

int readArg(char *input, int index, int maxIndex, Arg *arg){
  int newindex=index;
  arg->indirect=false;
  if(input[newindex]=='!'){
    arg->indirect=true;
    Serial.print("(Indirect) ");
    newindex+=1;
  }
  if(newindex<maxIndex){
    newindex = readInt(input,newindex,maxIndex,arg);
    return newindex;
  }else{
    return newindex;
  }
}

Command *parse(char *input, int maxlen){
  int count = countCommands(input,maxlen);
  if(count==0){
    return NULL;
  }
  Command *cmds = (Command *)malloc((count+1) * sizeof(Command));
  if(cmds==NULL){
    Serial.println("Malloc fail, cannot parse input");
    return NULL;
  }
  int commandIndex=0;
  int inputIndex=0;
  boolean inCmd = true;
  Serial.print("Parsing buffer len ");
  Serial.print(maxlen);
  Serial.print(" with ");
  Serial.print(count);
  Serial.println(" commands");
  while(inputIndex < maxlen) {
    switch(input[inputIndex]){
      case 'L':{ //LED
        inputIndex+=1;
        Serial.print("Command ");
        Serial.print(commandIndex);
        Serial.println(" is L_CMD");
        cmds[commandIndex].cmd = L_CMD;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
        inputIndex+=1;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
        inputIndex+=1;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[2]));
        inputIndex+=1;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[3]));
        commandIndex++;
        break;
      }
      case 'W':{ //WAIT
        inputIndex+=1;
        Serial.print("Command ");
        Serial.print(commandIndex);
        Serial.println(" is W_CMD");
        cmds[commandIndex].cmd = W_CMD;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));        
        break;
      }
      case 'U':{ //UPDATE
        inputIndex+=1;
        Serial.print("Command ");
        Serial.print(commandIndex);
        Serial.println(" is U_CMD");
        cmds[commandIndex].cmd = U_CMD;  
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));       
        break;
      }
      case 'R':{ //REG
        inputIndex+=1;
        Serial.print("Command ");
        Serial.print(commandIndex);
        Serial.println(" is R_CMD");        
        cmds[commandIndex].cmd = R_CMD;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
        inputIndex+=1;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
        commandIndex++;
        break;
      }
      case 'A':{ //ADD
        inputIndex+=1;
        Serial.print("Command ");
        Serial.print(commandIndex);
        Serial.println(" is A_CMD");        
        cmds[commandIndex].cmd = A_CMD;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
        inputIndex+=1;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
        commandIndex++;
        break;
      }
      case 'S':{ //SUB
        inputIndex+=1;
        Serial.print("Command ");
        Serial.print(commandIndex);
        Serial.println(" is S_CMD");        
        cmds[commandIndex].cmd = S_CMD;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
        inputIndex+=1;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
        commandIndex++;
        break;
      }
      case 'M':{ //MUL
        inputIndex+=1;
        Serial.print("Command ");
        Serial.print(commandIndex);
        Serial.println(" is M_CMD");        
        cmds[commandIndex].cmd = M_CMD;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
        inputIndex+=1;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
        commandIndex++;
        break;
      }
      case 'D':{ //DIV
        inputIndex+=1;
        Serial.print("Command ");
        Serial.print(commandIndex);
        Serial.println(" is D_CMD");        
        cmds[commandIndex].cmd = D_CMD;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
        inputIndex+=1;
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
        commandIndex++;
        break;
      }
      case 'E':{ //EVAL
        inputIndex+=1;
        Serial.print("Command ");
        Serial.print(commandIndex);
        Serial.println(" is E_CMD");        
        cmds[commandIndex].cmd = E_CMD;   
        inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
        commandIndex++;             
        break;
      }
      case 'B':{ //BANK
        break;
      }      
      case 'J':{ //JUMP
        Serial.print("Jidx ");
        Serial.println(inputIndex);
        inputIndex+=1;
        Serial.print("Command ");
        Serial.print(commandIndex);
        if(isAlpha(input[inputIndex])){
          switch(input[inputIndex]){
            case 'G':{
              Serial.println(" is JG_CMD"); 
              inputIndex+=1;
              cmds[commandIndex].cmd = JG_CMD;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
              inputIndex+=1;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
              commandIndex++;              
              break;
            }
            case 'L':{
              Serial.println(" is JL_CMD"); 
              inputIndex+=1;
              cmds[commandIndex].cmd = JL_CMD;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
              inputIndex+=1;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
              commandIndex++;                    
              break;
            }
            case 'E':{
              Serial.println(" is JE_CMD");
              inputIndex+=1;
              cmds[commandIndex].cmd = JE_CMD;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
              inputIndex+=1;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
              commandIndex++;                    
              break;
            }
            case 'N':{
              Serial.println(" is JN_CMD");
              inputIndex+=1;
              cmds[commandIndex].cmd = JN_CMD;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
              inputIndex+=1;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
              commandIndex++;                    
              break;
            }
            case 'B':{
              Serial.println(" is JB_CMD");
              inputIndex+=1;
              cmds[commandIndex].cmd = JB_CMD;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
              inputIndex+=1;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
              commandIndex++;                    
              break;
            }
            case 'A':{
              Serial.println(" is JA_CMD");
              inputIndex+=1;
              cmds[commandIndex].cmd = JA_CMD;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
              commandIndex++;                    
              break;
            }
            case 'Z':{
              Serial.println(" is JZ_CMD");
              inputIndex+=1;
              cmds[commandIndex].cmd = JZ_CMD;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
              dumpArg(cmds[commandIndex].args[0]);
              inputIndex+=1;
              inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[1]));
              dumpArg(cmds[commandIndex].args[2]);
              commandIndex++; 
              Serial.print(inputIndex);
              Serial.println("End");                      
              break;
            }                        
          }
        }else{
          Serial.println(" is J_CMD");
          cmds[commandIndex].cmd = J_CMD;   
          inputIndex=readArg(input,inputIndex,maxlen,&(cmds[commandIndex].args[0]));
          Serial.print("JLabel ");
          Serial.print(cmds[commandIndex].args[0].val);
          Serial.print(" is at ");
          Serial.println(commandIndex);
          
          commandIndex++;        
        }        
        break;
      }
    }
  }
  //add null terminating command.
  cmds[commandIndex].cmd=NULL;
  return cmds;
}

void dumpArg(Arg arg){
  Serial.print("ARG: (indirect)");
  Serial.print(arg.indirect);
  Serial.print(" (value) ");
  Serial.println(arg.val);
}
void dumpArgs(Arg args[]){
  for(int i=0;i<2;i++){
    dumpArg(args[i]);
  }
}
void dumpRegs(){
  for(int i=0; i<15; i++){
    Serial.print(i);
    Serial.print(":");
    Serial.print(regs[i]);
    Serial.print(" ");
  }
  Serial.println("");
}

void runnextinsfrombank(){
  if(banks[activebank]!=NULL){
    CmdPtr cp=banks[activebank][bankoffset].cmd;
    if(cp!=NULL){
      (*cp)(banks[activebank][bankoffset].args);
      bankoffset++;
    }else{
      Serial.println("End of bank");
      bankoffset=0;
    }
  }
}

void wrapnextins(){
  if(banks[activebank]!=NULL){
    Serial.print("BEFORE: ");
    dumpRegs();    
    runnextinsfrombank();
    Serial.print("AFTER : ");
    dumpRegs();
  }
}

char *readBuffer;

void setup() {
  
  Serial.begin(115200); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("Init OctoWS2811");
  leds.begin();
  leds.show();
  
  Serial.println("Init read buffer");
  readBuffer=NULL;
  Serial.println("Init regs");
  for(int i=0; i<MAX_REG; i++){
    regs[i]=0;
  }
  Serial.println("Init banks");
  for(int i=0; i<MAX_BANK; i++){
    banks[i]=NULL;
  }
  
//  Serial.println("Parsing test bank");
//  banks[0]=parse(test,strlen(test));
//
//  if(banks[0]!=NULL){
//    activebank=0;
//    bankoffset=0;
//    wrapnextins();
//    while(bankoffset!=0){
//      wrapnextins();
//    }
//  }else{
//    Serial.println("Parse fail");
//  }

  //Serial.println("-enter main loop, sleeping");
}

void h(char *arg){
  for(int i=0;i<strlen(arg);i++){
    if(arg[i]<0x10){
      Serial.print('0');
    }
    Serial.print(arg[i],HEX);
  }
}

int findLF(char* buffer){
  if(buffer!=NULL){
    for(int i=0;i<strlen(buffer);i++){
      if(buffer[i]==0x0A){
        return i;
      }
    }
  }
  return -1;  
}
int offset=0;

void loop() {

  //collect data from ser port
  int serdatacount=Serial.available();
  char *readptr=NULL;
  if(serdatacount){
    if(readBuffer==NULL){
      readBuffer=(char*)malloc(sizeof(char)*(serdatacount+1));
      readBuffer[serdatacount]=0;      
      readptr=readBuffer;
      offset=0;
    }else{      
      int oldlen=strlen(readBuffer);
      char *newBuffer=(char*)malloc(sizeof(char)*(serdatacount+oldlen+1));
      newBuffer[serdatacount+oldlen]=0;
      memcpy(newBuffer,readBuffer,oldlen);
      free(readBuffer);
      readptr=&newBuffer[oldlen];
      readBuffer=newBuffer;
    }    
    Serial.readBytes(readptr,serdatacount);    
  }

  //do we have data?
  int len=-1;
  if(readBuffer!=NULL){
    len=strlen(readBuffer);
  }
  //if we have unprocessed data.. 
  if(len>offset){
    int lf=findLF(&readBuffer[offset]);
    while(lf!=-1){
      h(readBuffer);
      Serial.println("");
      Serial.print("LF found at : ");
      Serial.print(lf);
      Serial.print(" offset ");
      Serial.println(offset);
      //parse chunk of buffer into commands
      Command *cmd = parse(&readBuffer[offset],lf);
      if(cmd!=NULL){
        //exec parsed commands now.
        int cmdidx=0;
        while(cmd[cmdidx].cmd!=NULL){
          (*(cmd[cmdidx].cmd))(cmd[cmdidx].args);
          cmdidx++;
        }
      }
      offset+=lf+1;
      lf=findLF(&readBuffer[offset+1]);
    }
  }

  //trim readBuffer back to unprocessed data.
  if(offset>=len){
    free(readBuffer);
    readBuffer=NULL;
    offset=0;
  }else{
    char *newbuf=(char*)malloc(sizeof(char)*(len-offset+1));
    newbuf[len-offset]=0;
    memcpy(newbuf,&readBuffer[offset],(len-offset));
    free(readBuffer);
    readBuffer=newbuf;
    offset=0;
  }

  //run next step from bank.
  runnextinsfrombank();

  //delay to let other stuff happen..
  delayMicroseconds(10);
}
